#!/usr/bin/env python3

import os
from http import cookies

def print_headers(cookie_jar):
    print("Content-Type: text/html")
    # cookie_jar.output() returns all Set-Cookie headers, one per cookie
    print(cookie_jar.output())
    print("")  # blank line to end headers

def parse_cookies():
    cookie_str = os.environ.get("HTTP_COOKIE", "")
    cookie_jar = cookies.SimpleCookie()
    cookie_jar.load(cookie_str)
    return cookie_jar

def main():
    # Parse incoming cookies from environment
    cookies_received = parse_cookies()

    # Prepare cookies to set (all in one SimpleCookie object)
    cookies_to_set = cookies.SimpleCookie()
    cookies_to_set["sessionid"] = "abc123"
    cookies_to_set["sessionid"]["path"] = "/"
    cookies_to_set["sessionid"]["httponly"] = True

    cookies_to_set["test_cookie"] = "123"
    cookies_to_set["test_cookie"]["path"] = "/"

    cookies_to_set["user_pref"] = "dark_mode"
    cookies_to_set["user_pref"]["path"] = "/"

    # Output headers including all Set-Cookie headers
    print_headers(cookies_to_set)

    # Output HTML body
    print("<html><head><title>CGI Cookie Test</title></head><body>")
    print("<h1>CGI Environment Variables</h1>")
    print("<table border='1'><tr><th>Variable</th><th>Value</th></tr>")
    for key in sorted(os.environ.keys()):
        print("<tr><td>{}</td><td>{}</td></tr>".format(key, os.environ[key]))
    print("</table>")

    print("<h2>Received Cookies</h2>")
    if cookies_received:
        print("<ul>")
        for key, morsel in cookies_received.items():
            print("<li><b>{}</b> = {}</li>".format(key, morsel.value))
        print("</ul>")
    else:
        print("<p>No cookies received.</p>")

    print("</body></html>")

if __name__ == "__main__":
    main()
