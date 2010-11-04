eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testPVType.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("rm testPVType");
system ("rm testPVTypeDiff");
system ("../bin/${EPICS_HOST_ARCH}/testPVType testPVType");
system ("diff testPVType testPVTypeGold >> testPVTypeDiff");
if(-z "testPVTypeDiff") {
    print "testPVType OK\n";
} else {
    print "testPVType Failed\n";
}
