#!/usr/local/bin/python3

import cgi
import os
import sys

UPLOAD_DIRECTORY = "./root/var/www/uploads"

print("Content-Type: text/html\n")

# Get the data from the form
form = cgi.FieldStorage()

# Get the file from the form
file_item = form["file"]

if file_item.filename:
    # The file was uploaded
    filename = file_item.filename
    filepath = os.path.join(UPLOAD_DIRECTORY, filename)
    open(filepath, "wb").write(file_item.file.read())

    print("<html><body>")
    print(f"<p>File {filename} uploaded successfully!</p>")
    print("</body></html>")
else:
    # The file was not uploaded
    print("<html><body>")
    print("<p>Error: no file was uploaded</p>")
    print("</body></html>")
