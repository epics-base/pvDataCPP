eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testIntrospect.pl
use Env;
system ("rm testIntrospect");
system ("rm testIntrospectDiff");
system ("../bin/${EPICS_HOST_ARCH}/testIntrospect testIntrospect");
system ("diff testIntrospect testIntrospectGold >> testIntrospectDiff");
if(-z "testIntrospectDiff") {
    print "testIntrospect OK\n";
} else {
    print "testIntrospect Failed\n";
}
