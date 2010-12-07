eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testQueue.pl
use Env;
system ("rm testQueue");
system ("rm testQueueDiff");
system ("../bin/${EPICS_HOST_ARCH}/testQueue testQueue testQueueAux");
system ("diff testQueue testQueueGold >> testQueueDiff");
if(-z "testQueueDiff") {
    print "testQueue OK\n";
} else {
    print "testQueue Failed\n";
}
