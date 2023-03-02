#!/usr/local/bin/python3

import os

print("Content-type: text/html", end="\r\n\r\n")

print("<h1>CGI environment keys:</h1>")
print("<body>")
for key in os.environ:
	print(f"<p>{key}={os.environ[key]}</p>")
print("</body>")