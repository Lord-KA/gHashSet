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
        "mov rax, %0;"
        "mov rbx, %1;"
        "rol rax, 1;"
        "add rax, rbx;"
        "mov %0, rax;"
        ".att_syntax prefix;"
        : "=r" (hash)
        : "r" (hash), "b" (data)
        // : "rax", "rbx"
    );
    return hash;
}
/*
size_t hash_sum_asm(size_t hash, char data)
{
    size_t sum = 0;
    char s = '1';
    __asm__ __volatile__ (".intel_syntax noprefix;"
            "push %1;"
            "mov ecx, 32;"
            "xor eax, eax;"
            "lop%=:"
            "cmp ecx, 0;"
            "je end%=;"
            "add eax, [%1];"
            "inc %1;"
            "dec ecx;"
            "jmp lop%=;"
            "end%=:"
            "mov %0, eax;"
            "pop %1;"
            ".att_syntax prefix;"
            : "=r" (sum)
            : "r" (s)
            : "ecx", "eax");
    return sum;
}
*/
size_t hash_crc32(size_t hash, char data)
{
    // return _mm_crc32_u8(hash, (uint8_t)(data));
}

