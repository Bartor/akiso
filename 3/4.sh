#!/bin/bash
url=$1
interval=$2

function finish {
	rm .trackedpage
	rm -rf .git
}

trap finish EXIT

git init
lynx -dump $1 > .trackedpage
git add .trackedpage
git commit -q -m $(date +"%N")

while :
do
	sleep $2
	lynx -dump $1 > .trackedpage
	git diff
	git add .trackedpage
	git commit -q --untracked-files=no -m $(date +"%N") 1>/dev/null
done

