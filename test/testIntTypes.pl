eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testIntTypes.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("rm testIntTypes");
system ("rm testIntTypesDiff");
system ("../bin/${EPICS_HOST_ARCH}/testIntTypes testIntTypes");
system ("diff testIntTypes testIntTypesGold >> testIntTypesDiff");
if(-z "testIntTypesDiff") {
    print "testIntTypes OK\n";
} else {
    print "testIntTypes Failed\n";
}
