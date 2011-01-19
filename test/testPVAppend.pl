eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testPVAppend.pl
use Env;
system ("rm testPVAppend");
system ("rm testPVAppendDiff");
system ("../bin/${EPICS_HOST_ARCH}/testPVAppend testPVAppend");
system ("diff testPVAppend testPVAppendGold >> testPVAppendDiff");
if(-z "testPVAppendDiff") {
    print "testPVAppend OK\n";
} else {
    print "testPVAppend Failed\n";
}
