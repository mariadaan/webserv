#!/usr/local/bin/python3

import cgi
import os
import sys

print('Content-type: text/plaintext', end='\r\n')
print('', end='\r\n')
for key in os.environ:
	print(f'{key}={os.environ[key]}')

for line in sys.stdin: # WAAROM WERKT DIT SOMS??
	print(f'{line[:-1]}', end='\r\n')

UPLOAD_DIRECTORY = "./root/var/www/uploads/"

print("Content-Type: text/html\n")

# os.environ['wsgi.input'].seek(0)

# Get the data from the form
form = cgi.FieldStorage()

# Loop through each field in the form and print its value
for field in form.keys():
	print(field)
	value = form[field].value
	print(f"{field}: {value}")

# Check if the form contains a file upload
if 'file' in form:
	# Get the file object
	fileitem = form['file']

	# Check if the file was successfully uploaded
	if fileitem.filename:
		# Get the file name
		filename = fileitem.filename

		# Get the file contents
		filecontents = fileitem.file.read()

		# Process the file contents
		# ...

	else:
		print('No file was uploaded.')

else:
	print('No file upload in the request.')


print("hallo?")


# # Get the file from the form
# file_item = form["filename"]

# if file_item.filename:
#	 # The file was uploaded
#	 filename = file_item.filename
#	 filepath = os.path.join(UPLOAD_DIRECTORY, filename)
#	 open(filepath, "wb").write(file_item.file.read())

#	 print("<html><body>")
#	 print(f"<p>File {filename} uploaded successfully!</p>")
#	 print("</body></html>")
# else:
#	 # The file was not uploaded
#	 print("<html><body>")
#	 print("<p>Error: no file was uploaded</p>")
#	 print("</body></html>")
