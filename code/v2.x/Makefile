# astrocam Makefile v. 2.0
# Version: Mar-29-2007
#
# See LICENSE-file for license.
#
# (C) 2001-2009 by Steffen Wendzel
#
# contact: see https://www.wendzel.de

CC=gcc
SOURCEFILES=astrocam.c
CFLAGS=-O2 -Wall -Wshadow
RM=rm -f
#SECCHECK=-g -lefence -W

all : astrocam

astrocam : $(SOURCEFILES)
	@make clean
	$(CC) $(CFLAGS) -o astrocam -DMETAVERSION="2.7" -DVERSION="\"2.7.6\"" -DPATCHLEVEL="\"p1\"" astrocam.c $(SECCHECK)

install : ./astrocam
	@echo "executing install.sh"
	/bin/bash ./install.sh

count :
	wc -l $(SOURCEFILES) *.php *.pl *.sh | sort -bg

clean :
	rm -f astrocam *~ install *.core

