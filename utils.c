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

uint8_t is_be()
{
    /*
    endian test - deference and test the   
    first 8 bit subset given a 32-bit set
    */
    int32_t _be = 1;
    if ( *(unsigned char*)&_be == 1 )
        _be = 0;
    return (uint8_t)_be;
}

uint64_t u64fswap_endian(uint64_t v)
{
#ifdef __GNUC__
    return __builtin_bswap64(v);
#endif
    return ((v & 0x00000000000000FFULL) << 56) |
           ((v & 0x000000000000FF00ULL) << 40) |
           ((v & 0x00000000FF000000ULL) << 24) |
           ((v & 0x00FF000000000000ULL) << 8)  |
           ((v & 0x000000FF00000000ULL) >> 8)  |
           ((v & 0x0000FF0000000000ULL) >> 24) |
           ((v & 0xFF00000000000000ULL) >> 40) |
           ((v & 0x000000000000FF00ULL) >> 56);
}
