#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()

form = cgi.FieldStorage()

print("Content-Type: text/html\n")

print(f"""
<!DOCTYPE html>
<html>
<body>
    <h1>Received Parameters:</h1>
    <ul>
""")

# Iterate over all fields and display them
for key in form.keys():
    value = form.getvalue(key, 'Not provided')
    print(f"        <li>{key}: {value}</li>")

print("""
    </ul>
</body>
</html>
""")

# http://localhost:8080/cgi-bin/query.py?param1=value1&param2=value2