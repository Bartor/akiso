#!/bin/bash
f=$(date +'%N') #
wget $(curl -s --request GET --url 'https://api.thecatapi.com/v1/images/search?format=json' --header 'Content-Type: application/json' --header 'x-api-key: 00fdd079-90ad-489c-8a4f-0e5bf01577bf' | jq -r '.[0].url') -q -O $f && img2txt ./$f && rm $f && echo ''
curl -s --request GET --url 'https://api.icndb.com/jokes/random?escape=javascript' |  jq -r '.value.joke' | sed 's/\///g'