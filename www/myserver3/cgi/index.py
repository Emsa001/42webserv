#!/usr/bin/env python3

import os
import cgi
import cgitb
import json
import sys
import urllib.parse

# Set up absolute upload directory path
UPLOAD_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../uploads"))
os.makedirs(UPLOAD_DIR, exist_ok=True)

def list_files():
    try:
        files = os.listdir(UPLOAD_DIR)
        return {"status": "success", "files": files}
    except Exception as e:
        return {"status": "fail", "error": str(e)}

def save_file(form):
    if "file" not in form:
        return {"status": "fail", "error": "No file field in form"}

    file_item = form["file"]
    if file_item.filename:
        filename = os.path.basename(file_item.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)

        try:
            with open(filepath, "wb") as f:
                f.write(file_item.file.read())
            return {"status": "success", "filename": filename}
        except Exception as e:
            return {"status": "fail", "error": str(e)}
    else:
        return {"status": "fail", "error": "No file selected"}

def delete_file():
    form = cgi.FieldStorage()
    filename = form.getfirst("filename")
    if not filename:
        return {"status": "fail", "error": "No filename specified"}

    filepath = os.path.join(UPLOAD_DIR, os.path.basename(filename))
    if os.path.exists(filepath):
        try:
            os.remove(filepath)
            return {"status": "deleted", "filename": filename}
        except Exception as e:
            return {"status": "fail", "error": str(e)}
    else:
        return {"status": "fail", "error": "File not found"}

def save_binary():
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    if content_length == 0:
        return {"status": "fail", "error": "No data received"}
    try:
        binary_data = sys.stdin.buffer.read(content_length)
        filename = "upload.bin"
        filepath = os.path.join(UPLOAD_DIR, filename)
        with open(filepath, "wb") as f:
            f.write(binary_data)
        return {"status": "success", "filename": filename, "size": len(binary_data)}
    except Exception as e:
        return {"status": "fail", "error": str(e)}

def main():
    print("Content-Type: application/json\n")

    method = os.environ.get("REQUEST_METHOD")
    content_type = os.environ.get("CONTENT_TYPE", "")

    if method == "POST":
        if content_type.startswith("application/octet-stream"):
            response = save_binary()
        else:
            form = cgi.FieldStorage()
            response = save_file(form)
    elif method == "GET":
        response = list_files()
    elif method == "DELETE":
        response = delete_file()
    else:
        response = {"status": "fail", "error": "Invalid request method"}

    print(json.dumps(response))

if __name__ == "__main__":
    main()