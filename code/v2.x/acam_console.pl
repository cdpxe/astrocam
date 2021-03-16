#!/usr/bin/perl -w

# acam_console.pl is distributed under the following license:
#
# Copyright (C) 2007-2008 Steffen Wendzel, https://www.wendzel.de
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

use strict;
use IPC::Msg;
use IPC::SysV; # qw/IPC_PRIVATE S_IRWXU/;

sub ReadConfig {
	my %CONFIG;
	open(FP, "</etc/astrocam.conf") || die "Cannot open /etc/astrocam.conf!";
	while (my $zeile = <FP>) {
		(my $name, my $value) = split(/[\ \t]/, $zeile);
		$value =~ s/[\r\n]//g;
		$CONFIG{$name} = $value;
	}
	close(FP);
	return %CONFIG;
}

######################################################################################
# handle form data
######################################################################################

my %CONFIG = ReadConfig();

sub move
{
	my $action = $_[0];
	if (not ($action =~ m/^[lLrRud]$/)) {
		die "action value not allowed!";
	}
	# now send the $action value via IPC to astrocam daemon
	msgsnd(msgget($CONFIG{"ipckey"}, 0),
		pack("l! a*", 1, $action), 0);
}

print "This test programm moves the stepengine to left and right.\n";
print "Moving left ...\n";
move("L");
move("L");
move("L");
move("L");
move("L");
print "Moving right ...\n";
move("R");
move("R");
move("R");
move("R");
move("R");


