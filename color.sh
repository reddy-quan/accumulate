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
