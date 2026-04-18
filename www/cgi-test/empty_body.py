#!/usr/bin/env python3

import cgi
import datetime

print("Content-Type: text/html\n")

def get_date():
    now = datetime.datetime.now()
    formatted_date = now.strftime("%Y-%m-%d %H:%M:%S")
    return formatted_date

current_date = get_date()
