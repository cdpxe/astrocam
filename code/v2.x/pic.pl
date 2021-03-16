#!/usr/bin/perl -wT

# pic.pl is distributed under the following license:
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

sub ReadForm {
	my %FORM;
	foreach my $field (param()) {
		$FORM{$field} = param($field);
		# check for some bad characters here
		unless ($FORM{$field} =~ /^[a-zA-Z0-9\.\:\ \/\-\+\_]+$/) { #\\[\]\{\(\)\&\|]+$/ ) {
			die "Insecure Character in \"$FORM{$field}\" found.";
		}
	}
	return %FORM;
}

my %FORM = ReadForm();
my $picfile = $FORM{"picfile"};
my $refreshtime = $FORM{"refreshtime"};

if ($refreshtime <= 0 || $refreshtime > 9999999) {
	die "refreshtime out of range!";
}

print header;
print '<html>
<head>
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="refresh" content="'.$refreshtime.';pic.pl?picfile='.$picfile.'&refreshtime='.$refreshtime.'">
	<link rel="stylesheet" type="text/css" href="/design.css" title="primary">
</head>
<body>
<table>
<tr>
	<td class="menu">
		<img src="'.$picfile.'" alt="current image">
	</td>
</tr>
</body>
</html>';
