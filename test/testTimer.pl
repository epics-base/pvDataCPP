eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testTimer.pl
use Env;
system ("rm testTimer");
system ("rm testTimerDiff");
system ("../bin/${EPICS_HOST_ARCH}/testTimer testTimer testTimerAux");
system ("diff testTimer testTimerGold >> testTimerDiff");
if(-z "testTimerDiff") {
    print "testTimer OK\n";
} else {
    print "testTimer Failed\n";
}
