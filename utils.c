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
    endian test - dereference and test the   
    first 8 bit subset given a 32-bit set
    */
    int32_t _be = 1;
    if ( *(unsigned char*)&_be == 1 )
        _be = 0;
    return (uint8_t)_be;
}

uint64_t u64swap_endian(uint64_t v)
{
#ifdef __GNUC__
    return __builtin_bswap64(v);
#elif _MSC_VER
    return _byteswap_uint64(v);
#else
    return v >> 56
         | ((v >> 40) & 0xff00)
         | ((v >> 24) & 0xff0000)
         | ((v >> 8) & 0xff000000)
         | ((v << 8) & 0xff00000000)
         | ((v << 24) & 0xff0000000000)
         | ((v << 40) & 0xff000000000000)
         | v << 56;
#endif
}
