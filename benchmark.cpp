#include "ghashset.h"
#include "gutils.h"
#include "assert.h"

#include "time.h"

static const size_t SEARCHES = 100;
static const size_t ATTEMPTS = 1000;
static const size_t CAPACITY = 250;

int main()
{
    gHashSet hashset = {};
    gHashSet *h = &hashset;

    gHashSet_ctor(h, CAPACITY, NULL);
    FILE *inp = fopen("../War-And-Peace.txt", "r");

    size_t len = MAX_KEY_LEN / 10 + rand() % 60;
    size_t new_len = len;
    size_t status = len;
    while (status == len) {
        len = new_len;
        char key[MAX_KEY_LEN] = {};
        status = fread(key, sizeof(char), len, inp);
        gHashSet_insert(h, key, NULL);
        new_len = MAX_KEY_LEN / 10 + rand() % 60;
    }

    FILE *out = fopen("stat.txt", "w");
    gHashSet_statistics(h, out);
    fclose(out);

    srand(clock());
    clock_t time = clock();

    for (size_t i = 0; i < SEARCHES; ++i) {
        char key[MAX_KEY_LEN] = {};
        size_t len = MAX_KEY_LEN / 2 + rand() % 10;
        for (size_t j = 0; j < len; ++j)
            key[j] = rand() % 100 + 48;
        char *value = NULL;
        for (size_t j = 0; j < ATTEMPTS; ++j)
            gHashSet_find(h, key, &value);
        if (i % (SEARCHES / 100) == 0)
            printf("%zu%\n", i / (SEARCHES / 100));
    }

    fprintf(stderr, "overall time = %lf sec\n", (double)(clock() - time) / CLOCKS_PER_SEC);

    gHashSet_dtor(h);
}
