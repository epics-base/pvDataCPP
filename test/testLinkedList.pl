eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # testLinkedList.pl
use Env;
system ("rm testLinkedList");
system ("rm testLinkedListDiff");
system ("../bin/${EPICS_HOST_ARCH}/testLinkedList testLinkedList testLinkedListAux");
system ("diff testLinkedList testLinkedListGold >> testLinkedListDiff");
if(-z "testLinkedListDiff") {
    print "testLinkedList OK\n";
} else {
    print "testLinkedList Failed\n";
}
