#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

int8_t arrcmp8(uint8_t p[], uint8_t q[], uint64_t len);
int8_t arrcmp32(int32_t p[], int32_t q[], uint64_t len);
uint8_t is_be();
int32_t *make_string(wchar_t *str);
int32_t i32swap_endian(int32_t v);
uint32_t u32swap_endian(uint32_t v);
uint64_t u64swap_endian(uint64_t v);
