eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testIntTypes.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("./testIntTypes.pl");
system ("./testIntrospect.pl");
