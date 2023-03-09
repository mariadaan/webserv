#!/usr/local/bin/python3

import cgi
import os
import sys

print("Content-type: text/html", end="\r\n\r\n")

# Get the data from the form
form = cgi.FieldStorage()

if "CONTENT_TYPE" in os.environ:
	ctype, pdict = cgi.parse_header(os.environ["CONTENT_TYPE"])
	if "boundary" in pdict:
		pdict["boundary"] = pdict["boundary"].encode()
	else:
		exit(0)
else:
	print("<html><body>")
	print("<p>Error: no file was uploaded</p>")
	print("</body></html>")
	exit(0)

cgi.parse_multipart(sys.stdin, pdict)

if "UPLOAD_PATH" in os.environ:
	UPLOAD_DIRECTORY = os.environ["UPLOAD_PATH"]
else:
	UPLOAD_DIRECTORY = "./root/var/www/uploads"

# Create directory if it does not exist
dir_exists = os.path.exists(UPLOAD_DIRECTORY)
if not dir_exists:
	os.makedirs(UPLOAD_DIRECTORY)

if "file" in form:
	# Get the file from the form
	file_item = form["file"]

	if file_item.filename:
		# The file was uploaded
		filename = file_item.filename
		filepath = os.path.join(UPLOAD_DIRECTORY, filename)
		open(filepath, "wb").write(file_item.file.read())

		print("<html><body>")
		print(f"<p>File {filename} uploaded to {UPLOAD_DIRECTORY} successfully!</p>")
		print("</body></html>")
	else:
		# The file was not uploaded
		print("<html><body>")
		print("<p>Error: no file was uploaded</p>")
		print("</body></html>")
else:
	print("No file")