eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testMessageQueue.pl
use Env;
system ("rm testMessageQueue");
system ("rm testMessageQueueDiff");
system ("../bin/${EPICS_HOST_ARCH}/testMessageQueue testMessageQueue testMessageQueueAux");
system ("diff testMessageQueue testMessageQueueGold >> testMessageQueueDiff");
if(-z "testMessageQueueDiff") {
    print "testMessageQueue OK\n";
} else {
    print "testMessageQueue Failed\n";
}
