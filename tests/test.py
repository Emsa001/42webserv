import requests
import pytest
import unittest

BLU = "\033[94m"
GRN = "\033[92m"
RED = "\033[91m"
RES = "\033[0m"

host = "http://localhost:8080"

class I_url():
    def __init__(self, url, expected):
        self.url = url
        self.expected = expected
        

cgi_query = [
    I_url(f"{host}/cgi-test/query.py?param1=value1&param2=value2", 200),
]

valid_cgi = [
    I_url(f"{host}/cgi-bin/hello.py", 200),
    I_url(f"{host}/cgi-bin/hello.sh", 200),
    I_url(f"{host}/cgi-bin/hello.php", 200),
    I_url(f"{host}/cgi-bin/hello.pl", 200),
]

invalid_cgi = [
    I_url(f"{host}/cgi-test/", 403),
    I_url(f"{host}/cgi-test/hello", 404),
    I_url(f"{host}/cgi-bin/hello.py?param=invalid", 200),
    I_url(f"{host}/cgi-test/empty_body.py", 500),
    I_url(f"{host}/cgi-test/empty_headers.py", 500),
    I_url(f"{host}/cgi-test/empty_content_type.py", 500),
]

def test_url(url: list[I_url]):
    for i in url:
        response = requests.get(i.url)
        if response.status_code == i.expected:
            print(f"{GRN}OK {i.url} {response.status_code}{RES}")
        else:
            print(f"{RED}FAIL {i.url} {response.status_code}{RES}")
    
if __name__ == "__main__":
    print(f"{BLU}Testing valid CGI scripts...{RES}")
    test_url(valid_cgi)
    print(f"{BLU}Testing invalid CGI scripts...{RES}")
    test_url(invalid_cgi)
    print(f"{BLU}Testing cgi query...{RES}")
    test_url(cgi_query)