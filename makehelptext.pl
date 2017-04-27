#!/usr/bin/perl

#
# $Id: makehelptext.pl,v 1.7 1998/11/23 18:55:19 kurt Exp $
#
# this program takes a textfile with gf1-documentation, and converts
# it to an include-file
#

#    Copyright (C) 1998 Kurt Van den Branden
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  

if ($ARGV[0])
{
    $infile = $ARGV[0];
}
else
{
    $infile = 'gf1_doc.txt';
}

if ($ARGV[1])
{
    $outfile = $ARGV[1];
}
else
{
    $outfile = 'helptext.h';
}

open (IN, $infile) || die "ERROR: input not found ($infile)\n\n";
open (OUT, ">$outfile") || die "ERROR: can't create output ($outfile)\n\n";

print OUT "/*\n";
print OUT "** contains the text to display in the helpwindow from gf1\n";
print OUT "**\n";
print OUT "** this file is created automatically from the documentation\n";
print OUT "*/\n";
print OUT "/*\n";
print OUT "**    Copyright (C) 1998 Kurt Van den Branden\n";
print OUT "**\n";
print OUT "** This program is free software; you can redistribute it and/or modify\n";
print OUT "** it under the terms of the GNU General Public License as published by\n";
print OUT "** the Free Software Foundation; either version 2 of the License, or\n";
print OUT "** (at your option) any later version.\n";
print OUT "** \n";
print OUT "** This program is distributed in the hope that it will be useful,\n";
print OUT "** but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
print OUT "** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
print OUT "** GNU General Public License for more details.\n";
print OUT "** \n";
print OUT "** You should have received a copy of the GNU General Public License\n";
print OUT "** along with this program; if not, write to the Free Software\n";
print OUT "** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  \n";
print OUT "*/\n";
print OUT "\n";
print OUT "#ifndef _HELPTEXT_H_\n";
print OUT "#define _HELPTEXT_H_\n";
print OUT "\n";
print OUT "char * helplines[] = {\n";
print OUT "    \"\",\n";
print OUT "    \"\",\n";

$counter = 2;
$lasttext = 0;
@sections = ();

while ($line = <IN>)
{
    $counter++;

    chop ($line);
    # replace " with \"
    $line =~ s/\"/\\\"/g;
    # cut all spaces from the start of the line
    ($text) = $line =~ m/^\s*(.*)/;
    if ($counter == 3)
    {
	print OUT "    \"\@c\@m$line\",\n";
	print OUT "    \"\",\n";
	push (@sections, [$text, $counter]);
	$counter++;
    }
    elsif ($line =~ m/^\s*\d+\.\s+/)
    {
	print OUT "    \"\@m$line\",\n";
	push (@sections, ["  $text", $counter]);
    }
    elsif ($line =~ m/^\s*\d+\.\d+\.\s+/)
    {
	print OUT "    \"\@b$line\",\n";
	push (@sections, ["    $text", $counter]);
    }
    elsif ($line =~ m/^\s*\d+\.\d+\.\d+\.\s+/)
    {
	print OUT "    \"$line\",\n";
	push (@sections, ["      $text", $counter]);
    }
    else
    {
	print OUT "    \"$line\",\n";
    }

    if (length ($line) != 0)
    {
	$lasttext = $counter;
    }
}

$lasttext += 5;

print OUT "};\n\n";
print OUT "#define NRHELPLINES $lasttext\n\n";
print OUT "struct sectiondata {\n";
print OUT "    char line[100];\n";
print OUT "    int offset;\n";
print OUT "} sectionlines[] = {\n";

foreach $item (@sections)
{
    print OUT "    {\"$item->[0]\", $item->[1]},\n";
}

print OUT "};\n\n";
$t = $#sections + 1;
print OUT "#define NRSECTIONLINES $t\n\n";

print OUT "#endif";
exit;
