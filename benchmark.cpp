#ifdef ROL
#define GHASHSET_HASH(hash, data) hash_rol(hash, data)
#endif

#ifdef ROL_ASM
#define GHASHSET_HASH(hash, data) hash_rol_asm(hash, data)
#endif

#ifdef CRC32
#define GHASHSET_HASH(hash, data) hash_crc32(hash, data)
#endif

#ifndef GHASHSET_HASH
#define GHASHSET_HASH(hash, data) hash_crc32(hash, data)
#endif

#include "ghashset.h"
#include "gutils.h"
#include "assert.h"

#include "time.h"

struct aligned {
    char data[MAX_KEY_LEN] = {};
} __attribute__((aligned(64)));

#define FIND(h, k) ({               \
    struct aligned buf = {};         \
    char *res = NULL;                 \
    strcpy(buf.data, k);               \
    gHashSet_find(h, buf.data, &res);   \
    res;                                 \
})


static const size_t SEARCHES = 10000;
static const size_t ATTEMPTS = 10000;
static const size_t CAPACITY = 250;

volatile char *value = NULL;

int main()
{
    #ifdef __AVX512__
        fprintf(stderr, "AVX512 is on!\n");
    #endif
    #ifdef ROL
        fprintf(stderr, "ROL hash is on!\n");
    #endif
    #ifdef ROL_ASM
        fprintf(stderr, "ROL_ASM hash is on!\n");
    #endif
    #ifdef CRC32
        fprintf(stderr, "CRC32 hash is on!\n");
    #endif
    gHashSet hashset = {};
    gHashSet *h = &hashset;

    gHashSet_ctor(h, CAPACITY, NULL);
    FILE *inp = fopen("../texts/War-And-Peace.txt", "r");
    assert(inp);

    size_t len = (MAX_KEY_LEN / 10 + rand() % 60) % MAX_KEY_LEN;
    size_t new_len = len;
    size_t status = len;
    while (status == len) {
        len = new_len;
        struct aligned buf = {};
        status = fread(buf.data, sizeof(char), len, inp);
        gHashSet_insert(h, buf.data, NULL);
        new_len = (MAX_KEY_LEN / 10 + rand() % 60) % MAX_KEY_LEN;
    }

    FILE *out = fopen("stat.txt", "w");
    gHashSet_statistics(h, out);
    fclose(out);

    srand(clock());
    clock_t time = clock();

    for (size_t i = 0; i < SEARCHES; ++i) {
        struct aligned buf = {};
        char *res = NULL;
        size_t len = MAX_KEY_LEN / 2 + rand() % 10;
        for (size_t j = 0; j < len; ++j)
            buf.data[j] = rand() % 100 + 48;
        for (size_t j = 0; j < ATTEMPTS; ++j) {
            gHashSet_find(h, buf.data, &res);
            value = res;
        }
        if (i % (SEARCHES / 20) == 0)
            printf("%zu%\n", i / (SEARCHES / 100));
    }

    fprintf(stderr, "overall time = %lf sec\n", (double)(clock() - time) / CLOCKS_PER_SEC);

    gHashSet_dtor(h);
}
