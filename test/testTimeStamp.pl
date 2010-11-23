eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testTimeStamp.pl
use Env;
system ("rm testTimeStamp");
system ("rm testTimeStampDiff");
system ("../bin/${EPICS_HOST_ARCH}/testTimeStamp testTimeStamp testTimeStampAux");
system ("diff testTimeStamp testTimeStampGold >> testTimeStampDiff");
if(-z "testTimeStampDiff") {
    print "testTimeStamp OK\n";
} else {
    print "testTimeStamp Failed\n";
}
