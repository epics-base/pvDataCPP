eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testPVStructureArray.pl
use Env;
system ("rm testPVStructureArray");
system ("rm testPVStructureArrayDiff");
system ("../bin/${EPICS_HOST_ARCH}/testPVStructureArray testPVStructureArray");
system ("diff testPVStructureArray testPVStructureArrayGold >> testPVStructureArrayDiff");
if(-z "testPVStructureArrayDiff") {
    print "testPVStructureArray OK\n";
} else {
    print "testPVStructureArray Failed\n";
}
