#!/usr/bin/env php-cgi
<?php
// Read incoming cookie
$incoming_value = isset($_COOKIE['test_cookie']) ? $_COOKIE['test_cookie'] : 'None';

// Set a cookie
setcookie("test_cookie", "12345", time() + 3600, "/");

// Output
header("Content-Type: text/html");
echo "<html><body>";
echo "<h1>Cookie Test</h1>";
echo "<p>Received cookie: <strong>$incoming_value</strong></p>";
echo "</body></html>";
?>
