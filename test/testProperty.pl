eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testProperty.pl
use Env;
system ("rm testProperty");
system ("rm testPropertyDiff");
system ("../bin/${EPICS_HOST_ARCH}/testProperty testProperty testPropertyAux");
system ("diff testProperty testPropertyGold >> testPropertyDiff");
if(-z "testPropertyDiff") {
    print "testProperty OK\n";
} else {
    print "testProperty Failed\n";
}
