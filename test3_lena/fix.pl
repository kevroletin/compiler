#!/bin/perl

use IO::Dir;
use IO::File;

sub Fix{
    $file = shift;
    open my $in, "<", $file;
    while (<$in>) {
        next if /(\|\s*)\1*$/;  
        s/\|\_{3}/    /g;
        s/\| {3}/    /g;
        s/ {2}/ /g;
        s/'(.*)'/\1/g;
        print;     
        }
    }

$fin = shift;
Fix $fin;