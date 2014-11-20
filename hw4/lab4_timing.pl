#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;
my $program_txt = "";
my $skip_val = 0;

GetOptions ("program_txt=s" => \$program_txt,
                "skip_val=i" => \$skip_val);

if (!$program_txt || !$skip_val) {
    die "$0 -program_txt -skip_val
        Need to supply program_txt, containing comma seperated programs to run
        Need to supply number of samples to skip\n";
}

sub time_program {

    my ($prgm_to_run, $skip_val) = @_;
#Need to open Report file, and append the values to the report 
    my $FH;
    open($FH, '>>', 'report.txt') or die;
    
#call usr/bin/time for each thread variation
    my @num_threads = (1, 2, 4);
    my @timing_result;
    my $desc_string;
    my $output_file;

    foreach my $pt (@num_threads) {
        $desc_string = "For $prgm_to_run, with $pt thread(s) and skipping $skip_val samples: The elapsed time is %E\n";   
        $output_file = "${prgm_to_run}_Thread${pt}_Skip${skip_val}.out"; 
#Running /usr/bin/time
        @timing_result =  `/usr/bin/time -f "$desc_string" ./$prgm_to_run $pt $skip_val > $output_file`;
        system("sort -n $output_file > s_$output_file");
        unlink($output_file);
#Printing to file
        print {$FH} @timing_result;
    } 
    
    close($FH);

}

#compile list of programs to time
open(PRGM_LIST, "<", $program_txt) or die;
my $program_list;
chomp($program_list = <PRGM_LIST>);
my @programs = split(',', $program_list);

print("start\n");
foreach my $prgm (@programs) {
    print "Beginning timing for $prgm\n";
    time_program($prgm, $skip_val);
    print "Timing for $prgm complete\n";
}
print("end\n");




