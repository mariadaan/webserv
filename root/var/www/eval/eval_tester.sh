#!/bin/bash

#!/bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

PINK="\033[38;2;255;105;180m"
GREEN="\033[38;2;57;181;74m"
RED="\033[38;2;222;56;43m"
RESET="\033[0m"

printf "$PINK"
echo "${bold}TEST 1"
printf "$GREEN"
echo "curl --resolve le_site.com:8080:127.0.0.1 http://le_site.com/${normal}"
printf "$RESET"
curl --resolve le_site.com:8080:127.0.0.1 http://le_site.com/

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl localhost:8080/blabla_random${normal}"
printf "$RESET"
curl localhost:8080/blabla_random

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "http://localhost:8080/posting -X POST -H "Content-Type: plain/text" --data "This body is too long because in the /posting location, only 40 bytes are allowed"${normal}"
printf "$RESET"
curl http://localhost:8080/posting -X POST -H "Content-Type: plain/text" --data "This body is too long because in the /posting location, only 40 bytes are allowed"

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "http://localhost:8080/posting -X POST -H "Content-Type: plain/text" --data "This body is not too long"${normal}"
printf "$RESET"
curl http://localhost:8080/posting -X POST -H "Content-Type: plain/text" --data "This body is not too long"

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X DELETE localhost:8080/posting${normal}"
printf "$RESET"
curl -X DELETE localhost:8080/posting

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X DELETE localhost:8080/uploads/${normal}"
printf "$RESET"
curl -X DELETE localhost:8080/uploads/

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl localhost:8080/images/${normal}"
printf "$RESET"
curl localhost:8080/images/

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl localhost:8080${normal}"
printf "$RESET"
curl localhost:8080

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X POST localhost:8080/posting --data "hallo"${normal}"
printf "$RESET"
curl -X POST localhost:8080/posting --data "hallo"

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X DELETE localhost:8080/uploads/${normal}"
printf "$RESET"
curl -X DELETE localhost:8080/uploads/

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X UNKNOWN localhost:8080${normal}"
printf "$RESET"
curl -X UNKNOWN localhost:8080

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl -X RANDOM localhost:8080${normal}"
printf "$RESET"
curl -X RANDOM localhost:8080

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl localhost:8080/error.py${normal}"
printf "$RESET"
curl localhost:8080/error.py

printf "$PINK"
echo "${bold}TEST 2"
printf "$GREEN"
echo "curl localhost:8080/infinite_loop.py${normal}"
printf "$RESET"
curl localhost:8080/infinite_loop.py



