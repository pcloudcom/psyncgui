#!/bin/bash

if [ -z "$1" ]; then
	echo Usage: $0 debfile
	exit 2
fi

if [ -x "`which gdebi-gtk`" ]; then
	inst=`which gdebi-gtk`
elif [ -x "`which qapt-deb-installer`" ]; then
	inst=`which qapt-deb-installer`
else
	echo Both gdebi-gtk and qapt-deb-installer are not installed, failing.
	exit 3
fi

$inst "$1"
