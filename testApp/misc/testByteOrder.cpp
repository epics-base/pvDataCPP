#include <epicsEndian.h>
#include <stdio.h>

int main()
{
printf("EPICS_BYTE_ORDER: %s\n", (EPICS_BYTE_ORDER == EPICS_ENDIAN_LITTLE) ? "little" : "big");
printf("EPICS_FLOAT_WORD_ORDER: %s\n", (EPICS_FLOAT_WORD_ORDER == EPICS_ENDIAN_LITTLE) ? "little" : "big");
return 0;
}

