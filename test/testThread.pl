eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testThread.pl
use Env;
system ("rm testThread");
system ("rm testThreadDiff");
system ("../bin/${EPICS_HOST_ARCH}/testThread testThread testThreadAux");
system ("diff testThread testThreadGold >> testThreadDiff");
if(-z "testThreadDiff") {
    print "testThread OK\n";
} else {
    print "testThread Failed\n";
}
