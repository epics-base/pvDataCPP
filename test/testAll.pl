eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testIntTypes.pl
use Env;
system ("./testPVType.pl");
system ("./testThread.pl");
system ("./testLinkedList.pl");
system ("./testIntrospect.pl");
system ("./testPVData.pl");
system ("./testPVStructureArray.pl");
system ("./testPVAuxInfo.pl");
system ("./testTimeStamp.pl");
system ("./testTimer.pl");
