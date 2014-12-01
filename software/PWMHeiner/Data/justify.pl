#! /usr/bin/perl
#Version=1.0.0.0
use strict;
use warnings;
#use LWP;


open(FILE1, $ARGV[0]);
my $c=0;
while (<FILE1>) {
    my $line=$_;
    #printf("line %2d: %s", $c++, $line);
    #my $ref=$1 if /^Referer: (.*)/;
    my $prn = $line;
    $prn =~ s/PWM: //;
    $prn =~ s/Batt: //;
    $prn =~ s/Load: //;
    $prn =~ s/ISet: //;
    $prn =~ s/I: //;

    $prn =~ s/,1,1,1970//;
    $prn =~ s/DIS\|//;

    $prn =~ s/\./,/g;
    $prn =~ s/\|/;/g;
    
    
    printf("%s", $prn);
}
