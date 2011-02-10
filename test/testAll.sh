#!/bin/sh

./testBaseException.sh || exit $?
./testPVType.sh || exit $?
./testThread.sh || exit $?
./testLinkedList.sh || exit $?
./testBitSet.sh || exit $?
./testByteBuffer.sh || exit $?
./testIntrospect.sh || exit $?
./testPVData.sh || exit $?
./testPVStructureArray.sh || exit $?
./testPVAppend.sh || exit $?
./testPVAuxInfo.sh || exit $?
./testTimeStamp.sh || exit $?
./testTimer.sh || exit $?
./testQueue.sh || exit $?
./testMessageQueue.sh || exit $?
./testSerialization.sh || exit $?
