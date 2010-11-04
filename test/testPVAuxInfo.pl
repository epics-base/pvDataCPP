eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testPVAuxInfo.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("rm testPVAuxInfo");
system ("rm testPVAuxInfoDiff");
system ("../bin/${EPICS_HOST_ARCH}/testPVAuxInfo testPVAuxInfo");
system ("diff testPVAuxInfo testPVAuxInfoGold >> testPVAuxInfoDiff");
if(-z "testPVAuxInfoDiff") {
    print "testPVAuxInfo OK\n";
} else {
    print "testPVAuxInfo Failed\n";
}
