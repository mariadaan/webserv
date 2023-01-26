#!/usr/local/bin/python3
import os
import time
import sys

# time.sleep(2)

print('Content-type: text/plaintext', end='\r\n')
print('', end='\r\n')
for key in os.environ:
	print(f'{key}={os.environ[key]}')
print('Hello, world! Test\nThis is a test\n', end='\r\n')

for line in sys.stdin:
	print(f'Got : {line[:-1]}', end='\r\n')
# s = sys.stdin.read(20)
# print(f'Got """{s}""" from stdin', end='\r\n')
