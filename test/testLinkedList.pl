eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testLinkedList.pl
$EPICS_HOST_ARCH = "linux-x86";
system ("rm testLinkedList");
system ("rm testLinkedListDiff");
system ("../bin/${EPICS_HOST_ARCH}/testLinkedList testLinkedList");
system ("diff testLinkedList testLinkedListGold >> testLinkedListDiff");
if(-z "testLinkedListDiff") {
    print "testLinkedList OK\n";
} else {
    print "testLinkedList Failed\n";
}
