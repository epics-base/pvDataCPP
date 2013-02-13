#include <iostream>
#include <pv/mb.h>

MB_DECLARE_EXTERN(test);
MB_DECLARE(test, 1000);

int main()
{
    MB_INIT;
    
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 100; j++)
            MB_POINT_ID(test, i, j);

    return 0;
}

