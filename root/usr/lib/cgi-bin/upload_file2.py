#!/usr/local/bin/python3

import cgi
import io
import sys
import os

# Read the request body from sys.stdin
request_body = sys.stdin.buffer.read()

# Create a file-like object from the request body
fp = io.BytesIO(request_body)

# Parse the form data using cgi.FieldStorage
form = cgi.FieldStorage(fp=fp, environ=os.environ, keep_blank_values=True)

# Loop through the form fields and print their names and values
for field in form.keys():
    field_item = form[field]
    if field_item.filename:
        # This is a file upload
        file_contents = field_item.file.read()
        print(f"Uploaded file {field_item.filename} with contents:\n{file_contents}")
    else:
        # This is a regular form field
        print(f"Form field {field} with value {field_item.value}")
