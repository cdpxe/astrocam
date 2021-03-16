#!/usr/bin/perl -wT

# astrocam.pl is distributed under the following license:
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
use CGI qw(:standard);
use CGI::Carp qw(warningsToBrowser fatalsToBrowser);
use IPC::Msg;
use IPC::SysV; # qw/IPC_PRIVATE S_IRWXU/;

sub ReadForm {
	my %FORM;
	foreach my $field (param()) {
		$FORM{$field} = param($field);
		# check for some bad characters here
		unless ($FORM{$field} =~ /^[a-zA-Z0-9\.\-\+\_]+$/ ) {
			die "Insecure Character in \"$FORM{$field}\" found.";
		}
	}
	return %FORM;
}

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

# check if all needed vars are available for us
foreach my $value ("imagesize-x", "imagesize-y", "ipckey", "design", "picfile", "refreshtime", "contenturl") {
	if ($CONFIG{$value} eq "") {
		die "at least one needed value ($value) in astrocam.conf is not set!";
	}
}

my %FORM = ReadForm();
my $action = $FORM{"a"};

if ($action) {
	if (not ($action =~ m/^[lLrRud]$/)) {
		die "action value not allowed!";
	}
	# now send the $action value via IPC to astrocam daemon
	msgsnd(msgget($CONFIG{"ipckey"}, 0), pack("l! a*", 1, $action), 0);
}

######################################################################################
# print website content
######################################################################################

print header;
print '<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=iso-8859-1">
	<link rel="stylesheet" type="text/css" href="'.$CONFIG{"contenturl"}.'/'.$CONFIG{"design"}.'.css" title="prim">
	<title>AstroCam Webinterface</title>
</head>
<body>
<table align="center">
<tr>
	<td colspan="2">
		<img src="'.$CONFIG{"contenturl"}.'/spacer.gif" height="20px" width="1px" alt=".">
	</td>
</tr>
<tr>
	<td align="center" valign="top" class="menu">
		<img src="'.$CONFIG{"contenturl"}.'/spacer.gif" alt="." width="80px" height="1px"><br>
		<b>[<a href="?a=l">&lt;</a>]</b>
		<b>[<a href="?a=r">&gt;</a>]</b>
		<br>
		<b>[<a href="?a=L">&lt;&lt;</a>]</b>
		<b>[<a href="?a=R">&gt;&gt;</a>]</b>
		<p>
		<b>[<a href="?a=u">up</a>]</b><br>
		<b>[<a href="?a=d">down</a>]</b>		
	</td>
	<td align="center" valign="center" class="pic"
	     width="'.$CONFIG{"imagesize-x"}.'" height="'.$CONFIG{"imagesize-y"}.'">
		<iframe border="0" width="'.$CONFIG{"imagesize-x"}.'" height="'.$CONFIG{"imagesize-y"}.'"
		   src="pic.pl?refreshtime='.$CONFIG{"refreshtime"}.'&picfile='.$CONFIG{"picfile"}.'"
		   scrolling="no" marginwidth="0" marginheight="0">Your browser does not support iframes!
		</iframe>
	</td>
</tr>
<tr>
	<td align="right" colspan="2" class="menu">
		<div style="font-size:9pt">
			<b>astrocam webinterface</b>
			[<a href="https://www.wendzel.de">info</a>]
		</div>
	</td>
</tr>
</table>
</body>
</html>';

