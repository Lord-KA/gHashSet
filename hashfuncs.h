#include <stdio.h>
#include <stdint.h>

size_t hash_one(size_t hash, char data)
{
    return 1;
}

size_t hash_ascii(size_t hash, char data)
{
    if (hash == 0)
        hash = data;
    return hash;
}

size_t hash_sum(size_t hash, char data)
{
    return hash + data;
}

size_t hash_len(size_t hash, char data)
{
    return hash + 1;
}

size_t hash_rol(size_t hash, char data)
{
    return (hash >> 31) + (hash << 1) + data;
}

size_t hash_rol_asm(size_t hash, char data)
{
    __asm__ __volatile__ (".intel_syntax noprefix;"
        "rol rbx, 1;"
        "add rbx, rcx;"
        ".att_syntax prefix;"
        : "=b" (hash)
        : "b" (hash), "c" (data)
    );
    return hash;
}

size_t hash_crc32(size_t hash, char data)
{
    return _mm_crc32_u8(hash, (uint8_t)(data));
}

