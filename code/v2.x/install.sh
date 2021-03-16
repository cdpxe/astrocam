#!/bin/bash

# v.2.3p0-non-auto, 23-Jan-2009
# 
# astrocam installation script by Steffen Wendzel
# (C) 2004-2009 by steffen wendzel

### astrocam >= 2.7.5 install script ###

ID=`id -u`

if [ "$ID" != "0" ]; then
   echo "need root access to install astrocam!"
   exit
fi

###########################
#### create default values
###########################

HTDOCS_P[0]="/var/www/htdocs"
HTDOCS_P[1]="/var/www"
HTDOCS_P[2]="/usr/local/apache/htdocs"
HTDOCS_P[3]="/usr/local/httpd/htdocs"
HTDOCS_P[4]="/usr/apache/htdocs"
HTDOCS_P[5]="/usr/httpd/htdocs"
HTDOCS_P[6]="/var/web"			# myserver

CGI_P[0]="/usr/lib/cgi-bin"
CGI_P[1]="${HTDOCS_P[0]}/../cgi-bin"
CGI_P[2]="${HTDOCS_P[1]}/../cgi-bin"
CGI_P[3]="${HTDOCS_P[2]}/../cgi-bin"
CGI_P[4]="${HTDOCS_P[3]}/../cgi-bin"
CGI_P[5]="${HTDOCS_P[6]}/cgi-bin/"	# myserver

MAN_P[0]="/usr/man"
MAN_P[1]="/usr/share/man/"
MAN_P[2]="/usr/local/man/"

DOC_P[0]="/usr/share/doc"
DOC_P[1]="/usr/local/share/doc"
DOC_P[2]="/usr/doc"

HTDOCS="not set"
CGI="not set"
MANPATH="not set"

for value in ${HTDOCS_P[*]}; do if [ -d $value ]; then HTDOCS=$value; break; fi; done
for value in ${CGI_P[*]}; do if [ -d $value ]; then CGI=$value; break; fi; done
for value in ${MAN_P[*]}; do if [ -d $value ]; then MANPATH=$value; break; fi; done
for value in ${DOC_P[*]}; do if [ -d $value ]; then DOCPATH=$value; break; fi; done

#############################
#### main script starts here
#############################

echo "       ---:{   AstroCam Installer Version 2.0   }:---"
echo
echo "I hope you read the INSTALL-file and modified astrocam.h (if needed) ..."
echo


# htdocs
echo -n "Please enter your web site directory [default is $HTDOCS]: "
read new
if [ "$new" != "" ]; then
	while [ ! -d "$new" ]; do
		echo "error: directory does not exist!."
		echo -n "please enter a valid path: "
		read new
	done
	HTDOCS="${new}/astrocam"
fi

# cgi
echo -n "Please enter your CGI directory [default is $CGI]: "
read new
if [ "$new" != "" ]; then
	while [ ! -d "$new" ]; do
		echo "error: directory does not exist!."
		echo -n "please enter a valid path: "
		read new
	done
	CGI="${new}"
fi

# manpath
echo -n "Please enter your manpage path [default is $MANPATH]: "
read new
if [ "$new" != "" ]; then
	while [ ! -d "$new" ]; do
		echo "error: directory does not exist!."
		echo -n "please enter a valid path: "
		read new
	done
	MANPATH="${new}/man7"
fi

# docpath
echo -n "Please enter your documentation path [default is $DOCPATH]: "
read new
if [ "$new" != "" ]; then
	while [ ! -d "$new" ]; do
		echo "error: directory does not exist!."
		echo -n "please enter a valid path: "
		read new
	done
	DOCPATH="${new}"
fi

# config file
cp -v astrocam.conf /etc/astrocam.conf
chmod 644 /etc/astrocam.conf

# php+content
if [ ! -d ${HTDOCS}/astrocam ]; then
	mkdir -p $HTDOCS/astrocam
fi

cp -v *.php *.css *.gif $HTDOCS/astrocam/
chmod -R ugo+rx $HTDOCS/astrocam

# cgi scripts
cp -v astrocam.pl pic.pl $CGI/
chmod 755 $CGI/astrocam.pl  $CGI/pic.pl

# binary
echo "installing astrocam daemon binary and acam_console.pl in /usr/local/sbin"
if [ ! -d /usr/local/sbin ]; then
	mkdir /usr/local/sbin
fi
cp -v astrocam acam_console.pl /usr/local/sbin/
chmod 500 /usr/local/sbin/astrocam /usr/local/sbin/acam_console.pl

# manpages
if [ ! -d ${MANPATH}/man7 ]; then
	mkdir -p $MANPATH
fi
cp -v astrocam.7 $MANPATH/man7/astrocam.7

# docs
if [ -d ${DOCPATH}/astrocam ]; then
	# remove old content
	rm -rf ${DOCPATH}/astrocam/*
fi
if [ ! -d ${DOCPATH}/astrocam ]; then
	mkdir -p ${DOCPATH}/astrocam || exit 1
fi
cp -v AUTHOR BUGS LICENSE CHANGELOG INSTALL ${DOCPATH}/astrocam/

# last msg. done.
echo
echo "Installation done!"
echo "    Now just edit /etc/astrocam.conf and run 'astrocam' to"
echo "    start the daemon. Thank you for using astrocam!"
echo

