#!/usr/bin/env perl
# filepath: /home/btvildia/Desktop/webserver/www/cgi-bin/hello.pl

use strict;
use warnings;
use POSIX qw(strftime);

# Print the HTTP header
print "Content-Type: text/html\n\n";

# Get the current date and time
my $current_date = strftime "%Y-%m-%d %H:%M:%S", localtime;

# Print the HTML content
print <<HTML;
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Perl CGI</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 20px;
        }
        .container {
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px gray;
            display: inline-block;
        }
        h1 {
            color: #007BFF;
        }
        p {
            color: #333;
        }
        .button {
            margin-top: 20px;
        }
        .button a {
            text-decoration: none;
            color: white;
            background-color: #007BFF;
            padding: 10px 20px;
            border-radius: 5px;
            font-size: 16px;
        }
        .button a:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Perl CGI</h1>
        <p><strong>Current CGI Script:</strong> Perl is handling this request!</p>
        <p><strong>Server Time:</strong> $current_date</p>
        <div class="button">
            <a href="http://localhost:8080/cgi-bin/hello.sh">Go to Shell CGI Script</a>
        </div>
    </div>
</body>
</html>
HTML