#!/bin/bash
width=$1
height=$2
scale=$3

i=0
while [ $i -lt $width ]; do
	networkInArray+=(0)
	networkOutArray+=(0)
	let i=i+1
done

function displaytime {
  local T=$1
  local D=$((T/60/60/24))
  local H=$((T/60/60%24))
  local M=$((T/60%60))
  local S=$((T%60))
  (( $D > 0 )) && printf '%d days ' $D
  (( $H > 0 )) && printf '%d hours ' $H
  (( $M > 0 )) && printf '%d minutes ' $M
  (( $D > 0 || $H > 0 || $M > 0 )) && printf 'and '
  printf '%d seconds\n' $S
}

function convertBytes {
	local B=$1
	local KB=$((B/1024))
	local MB=$((KB/1024))
	if [ $MB -gt 0 ]; then
		printf '%d MB' $MB
	elif [ $KB -gt 0 ]; then
		printf '%d KB' $KB
	else
		printf '%d B' $B 
	fi
}

function shiftIn {
	local val=$1
	for (( i=0; i<$width; i++ )); do
		let j=i+1
		networkInArray[$i]=${networkInArray[$j]}
	done
	networkInArray[-1]=$val
}

function shiftOut {
	local val=$1
	for (( i=0; i<$width; i++ )); do
		let j=i+1
		networkOutArray[$i]=${networkOutArray[$j]}
	done
	networkOutArray[-1]=$val
}

function maxIn {
	local max=0
	for (( i=0; i<$width; i++ )); do
		if [ ${networkInArray[$i]} -gt $max ]; then
			max=${networkInArray[$i]}
		fi
	done
	echo $max
}
function maxOut {
	local max=0
	for (( i=0; i<$width; i++ )); do
		if [ ${networkOutArray[$i]} -gt $max ]; then
			max=${networkOutArray[$i]}
		fi
	done
	echo $max
}


function drawIn {
	local max=$(maxIn)
	local levelHeight=$(echo "$max/$height" | bc)
	local line="╔"
	for (( i=1; i<$(($width*$scale+1)); i++ )); do
		line+="═"
	done
	line+="╗"
	echo $line
	for (( i=$height; i>0; i-- )); do
		local result="║\e[34m"
		local currentThreshold=$(echo "$levelHeight*$i" | bc)
		for (( j=0; j<$width; j++ )); do
			local currentHeight=${networkInArray[$j]}
			if [ $currentHeight -ge $currentThreshold ]; then
				for (( k=0; k<$scale; k++ )); do
					result+="█"
				done
			else
				for (( k=0; k<$scale; k++ )); do
					result+=" "
				done
			fi
		done
		echo -e "$result\e[39m║$(convertBytes $currentThreshold)"
	done
	line="╚"
	for (( i=1; i<$(($width*$scale+1)); i++ )); do
		line+="═"
	done
	line+="╝"
	echo $line
}

function drawOut {
	local max=$(maxOut)
	local levelHeight=$(echo "$max/$height" | bc)
	local line="╔"
	for (( i=1; i<$(($width*$scale+1)); i++ )); do
		line+="═"
	done
	line+="╗"
	echo $line
	for (( i=$height; i>0; i-- )); do
		local result="║\e[32m"
		local currentThreshold=$(echo "$levelHeight*$i" | bc)
		for (( j=0; j<$width; j++ )); do
			local currentHeight=${networkOutArray[$j]}
			if [ $currentHeight -ge $currentThreshold ]; then
				for (( k=0; k<$scale; k++ )); do
					result+="█"
				done
			else
				for (( k=0; k<$scale; k++ )); do
					result+=" "
				done
			fi
		done
	echo -e "$result\e[39m║$(convertBytes $currentThreshold)"
	done
	line="╚"
	for (( i=1; i<$(($width*$scale+1)); i++ )); do
		line+="═"
	done
	line+="╝"
	echo $line
}

clear
networkString=$(cat /proc/net/dev | grep 'eth0' | sed 's/  */ /g')
networkArray=($networkString)
oldBytesIn=${networkArray[1]}
oldBytesOut=${networkArray[9]}
while :
do
	sleep 1
	tput cup 0 0
	networkString=$(cat /proc/net/dev | grep 'eth0' | sed 's/  */ /g')
	uptimeSeconds=$(cat /proc/uptime | sed 's/\..*$//')
	loadString=$(cat /proc/loadavg | sed 's/ .*$//')
  loadArray=($loadString)
	networkArray=($networkString)
	
	bytesIn=${networkArray[1]}
	bytesOut=${networkArray[9]}
	
	shiftIn $((bytesIn-oldBytesIn))
	echo Input graph
	drawIn
	
	shiftOut $((bytesOut-oldBytesOut))
	echo Output graph
	drawOut
	
	echo ⇓ `convertBytes $((bytesIn-oldBytesIn))` ⇑ `convertBytes $((bytesOut-oldBytesOut))                             `
	echo `displaytime $uptimeSeconds                            `
	echo Load ${loadArray[0]}

	oldBytesIn=$bytesIn
	oldBytesOut=$bytesOut
done
