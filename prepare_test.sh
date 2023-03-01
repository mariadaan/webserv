#!/bin/bash

# Download the cgi_test executable
# https://projects.intra.42.fr/uploads/document/document/13687/tester
# https://projects.intra.42.fr/uploads/document/document/13689/cgi_tester

# Create the directory YoupiBanane
mkdir YoupiBanane

# Create the required files and directories
touch YoupiBanane/youpi.bad_extension
touch YoupiBanane/youpi.bla
mkdir YoupiBanane/nop
touch YoupiBanane/nop/youpi.bad_extension
touch YoupiBanane/nop/other.pouic
mkdir YoupiBanane/Yeah
touch YoupiBanane/Yeah/not_happy.bad_extension

# Set up the configuration file
echo 'server {
	listen 80 default_server;
	root   ./root/var/www;

	location / {
		index   index.html;
		request_method GET;
	}

	location /put_test/ {
		request_method PUT;
		client_body_temp_path ./root/var/www/uploads;
		client_body_in_file_only on;
		client_body_buffer_size 128K;
		client_max_body_size 10M;
	}

	location ~ \.bla$ {
		request_method POST;
		index /usr/lib/cgi-bin cgi_tester
	}

	location /post_body {
		request_method POST;
		client_max_body_size 100;
	}

	location /directory/ {
		request_method GET;
		root ./YoupiBanane;
		autoindex on;
		autoindex_exact_size off;
		autoindex_localtime on;
		index youpi.bad_extension;
	}
	cgi bla
}
' > root/conf/tester.conf

# Before starting please follow the next few steps (files content can be anything and will be shown to you by the test):
# - Download the cgi_test executable on the host
# - Create a directory YoupiBanane with:
# -a file name youpi.bad_extension
# -a file name youpi.bla
# -a sub directory called nop
# -a file name youpi.bad_extension in nop
# -a file name other.pouic in nop
# -a sub directory called Yeah
# -a file name not_happy.bad_extension in Yeah
# press enter to continue

# Setup the configuration file as follow:
# - / must answer to GET request ONLY
# - /put_test/* must answer to PUT request and save files to a directory of your choice
# - any file with .bla as extension must answer to POST request by calling the cgi_test executable
# - /post_body must answer anything to POST request with a maxBody of 100
# - /directory/ must answer to GET request and the root of it would be the repository YoupiBanane and if no file are requested, it should search for youpi.bad_extension files