#!/usr/bin/env python3
import cgi
import os
import datetime
import sys
import urllib.parse

print("Content-Type: text/html\n")

def get_date():
    now = datetime.datetime.now()
    formatted_date = now.strftime("%Y-%m-%d %H:%M:%S")
    return formatted_date

def list_files():
    files = os.listdir(".")
    return "<br>".join(files)

def create_file(filename):
    try:
        with open(filename, 'w') as f:
            f.write("This is a new file.")
        return f"File '{filename}' created successfully."
    except Exception as e:
        return f"Error creating file: {e}"

def delete_file(filename):
    try:
        os.remove(filename)
        return f"File '{filename}' deleted successfully."
    except Exception as e:
        return f"Error deleting file: {e}"

def parse_post_body():
    if os.environ.get('REQUEST_METHOD', '') != 'POST':
        return {}

    # Read the POST body from stdin
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    except (ValueError, TypeError):
        content_length = 0

    post_data = sys.stdin.read(content_length)
    return urllib.parse.parse_qs(post_data)

if os.environ['REQUEST_METHOD'] == 'POST':
    post_fields = parse_post_body()

    action = post_fields.get('action', [None])[0]
    filename = post_fields.get('filename', [None])[0]
elif os.environ['REQUEST_METHOD'] == 'DELETE':
    input_data = sys.stdin.read()
    filename = input_data.split("=")[-1]
    action = "delete"
else:
    # Parse only the query string for GET requests
    query_params = urllib.parse.parse_qs(os.environ.get("QUERY_STRING", ""))
    action = query_params.get("action", [None])[0]
    filename = query_params.get("filename", [None])[0]


response_message = ""

if action == "create" and filename:
    create_file(filename)
elif action == "delete" and filename:
    delete_file(filename)

response_message = list_files()
current_date = get_date()

print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 20px;
        }}
        .container {{
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px gray;
            display: inline-block;
        }}
        h1 {{
            color: #007BFF;
        }}
        p {{
            color: #333;
        }}
        .button {{
            margin-top: 20px;
        }}
        .button a {{
            text-decoration: none;
            color: white;
            background-color: #007BFF;
            padding: 10px 20px;
            border-radius: 5px;
            font-size: 16px;
        }}
        .button a:hover {{
            background-color: #0056b3;
        }}
        .response {{
            margin-top: 20px;
            color: #333;
            font-size: 14px;
            text-align: left;
        }}
    </style>
    <script>
        function deleteFile() {{
            const filename = document.getElementById('delete-filename').value;
            if (!filename) {{
                alert('Please enter a filename to delete.');
                return;
            }}
            fetch('', {{
                method: 'DELETE',
                body: `filename=${{filename}}`,
                headers: {{
                    'Content-Type': 'application/x-www-form-urlencoded'
                }}
            }})
            .then(response => response.text())

            .catch(error => {{
                console.error('Error:', error);
            }});
        }}
    </script>
</head>
<body>
    <div class="container">
        <h1>Python CGI</h1>
        <p><strong>Current CGI Script:</strong> Python is handling this request!</p>
        <p><strong>Server Time:</strong> {current_date}</p>
        <div class="button">
            <form method="get">
                <input type="hidden" name="action" value="list">
                <button type="submit">List Files</button>
            </form>
            <form method="post">
                <input type="hidden" name="action" value="create">
                <input type="text" name="filename" placeholder="Enter filename" required>
                <button type="submit">Create File</button>
            </form>
            <div>
                <input type="text" id="delete-filename" placeholder="Enter filename" required>
                <button type="button" onclick="deleteFile()">Delete File</button>
            </div>
        </div>
        <div class="response" id="response">
            <p><strong>Response:</strong></p>
            <p>{response_message}</p>
        </div>
    </div>
</body>
</html>
""")