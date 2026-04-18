#!/usr/bin/env python3

import cgi
import os

# Directory to save uploaded files
UPLOAD_DIR = "./"

# Ensure the upload directory exists
os.makedirs(UPLOAD_DIR, exist_ok=True)

def main():
    print("Content-Type: text/html\n")
    
    form = cgi.FieldStorage()
    
    # If no file is uploaded, display the upload form
    if "file" not in form:
        print("""
        <html>
        <head>
            <title>File Upload</title>
        </head>
        <body>
            <h1>Upload a File</h1>
            <form method="post" enctype="multipart/form-data">
                <label for="file">Choose a file:</label>
                <input type="file" name="file" id="file">
                <br><br>
                <input type="submit" value="Upload">
            </form>
        </body>
        </html>
        """)
    else:
        file_item = form["file"]
        if file_item.filename:
            # Get the filename and save the file
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(UPLOAD_DIR, filename)
            with open(filepath, "wb") as f:
                f.write(file_item.file.read())
            print(f"""
            <html>
            <head>
                <title>Upload Result</title>
            </head>
            <body>
                <h1>File '{filename}' uploaded successfully!</h1>
                <a href="/cgi-test/upload.py">Upload another file</a>
            </body>
            </html>
            """)
        else:
            print("""
            <html>
            <head>
                <title>Upload Error</title>
            </head>
            <body>
                <h1>Error: No file was selected</h1>
                <a href="/cgi-test/upload.py">Try again</a>
            </body>
            </html>
            """)

if __name__ == "__main__":
    main()