eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testPVData.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("rm testPVData");
system ("rm testPVDataDiff");
system ("../bin/${EPICS_HOST_ARCH}/testPVData testPVData");
system ("diff testPVData testPVDataGold >> testPVDataDiff");
if(-z "testPVDataDiff") {
    print "testPVData OK\n";
} else {
    print "testPVData Failed\n";
}
