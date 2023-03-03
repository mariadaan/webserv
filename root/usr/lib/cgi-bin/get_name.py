#!/usr/local/bin/python3

import cgi

form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

print('HTTP/1.1 200 OK')
print('Content-Type: text/html\r\n\r\n')
print('<html>')
print('<head>')
print('<title>Reverse name</title>')
print('</head>')
print('<body>')
print('<h1>Reverse name</h1>')
if not first_name or not last_name:
	print('<h2>Please enter a first and last name!</h2>')
else:
	rev_first_name = first_name[::-1].lower().capitalize()
	rev_last_name = last_name[::-1].lower().capitalize()
	print('<h2>Your name in reverse is \"%s %s\"</h2>' % (rev_first_name, rev_last_name))
print('</body>')
print('</html>')