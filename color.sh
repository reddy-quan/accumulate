#!/bin/sh

green_back()
{
	echo -e "\e[1;42m$*\e[0m"
}

red_out()
{
	echo -e "\e[1;31m$*\e[0m"
}


red_out "hello world!"
green_back "This is a test"


POS=15
echo -n "Doing ... "
for((i=0;i<=100;i++))
do
echo -en "\\033[${POS}G $i % completed" 
sleep 0.1
done
echo -ne "\n"