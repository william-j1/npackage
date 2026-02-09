#include "utils.h"

int8_t arrcmp(uint8_t p[], uint8_t q[], uint64_t len)
{
    for ( uint64_t k = 0; k < len; k++ )
    {
        if ( p[k] == q[k] )
            continue;
        if ( p[k] < q[k] )
            return -1;
        if ( p[k] > q[k] )
            return 1;
    }
    return 0;
}
