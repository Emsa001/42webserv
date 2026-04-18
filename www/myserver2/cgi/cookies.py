#!/usr/bin/env python3

import os
import http.cookies

# Parse incoming cookies
cookies = http.cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookies.load(os.environ['HTTP_COOKIE'])

# Read an existing cookie (if any)
incoming_cookie = cookies.get('test_cookie')
incoming_value = incoming_cookie.value if incoming_cookie else "None"

# Set a cookie
new_cookie = http.cookies.SimpleCookie()
new_cookie['test_cookie'] = '12345'
new_cookie['test_cookie']['path'] = '/'
# Optional: new_cookie['test_cookie']['max-age'] = 3600

# Output headers
print("Content-Type: text/html")
print(new_cookie.output())  # This prints Set-Cookie header
print()

# Output body
print(f"""
<html>
<head><title>Cookie Test</title></head>
<body>
    <h1>Cookie Test</h1>
    <p>Received cookie: <strong>{incoming_value}</strong></p>
</body>
</html>
""")
