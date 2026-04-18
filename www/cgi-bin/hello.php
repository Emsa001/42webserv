#!/usr/bin/env php-cgi
<?php

function get_current_date() {
    return date("Y-m-d H:i:s");
}
$current_date = get_current_date();

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP CGI</title>
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
            color: #6a0dad;
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
            background-color: #6a0dad;
            padding: 10px 20px;
            border-radius: 5px;
            font-size: 16px;
        }
        .button a:hover {
            background-color: #5a0c9d;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>PHP CGI</h1>
        <p><strong>Current CGI Script:</strong> PHP is handling this request!</p>
        <p><strong>Server Time:</strong> <?php echo $current_date; ?></p>
        <div class="button">
            <a href="http://localhost:8080/cgi-bin/hello.py">Go to Python CGI Script</a>
        </div>
    </div>
</body>
</html>