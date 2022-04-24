#include <stdio.h>

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
