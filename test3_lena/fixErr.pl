#!/bin/perl

use IO::Dir;
use IO::File;

sub FixErr{
    my ($fans, $fres) = @_;
    open $res, "<", $fres;
    open $ans, "<", $fans; 
    if (<$ans> =~ /.*(Invalid|Error|Unclosed|Unexpected|Empty|expected).*/ and <$res> =~ /.*ERROR.*/) {
        print "matched";
    }
}

$fans = shift;
$fres = shift;
FixErr($fans, $fres);