#include "ghashset.h"
#include "gutils.h"
#include "assert.h"

int main()
{
    gHashSet hashset = {};
    gHashSet *h = &hashset;

    gHashSet_ctor(h, 250, NULL);
    FILE *inp = fopen("../War-And-Peace.txt", "r");

    size_t len = MAX_KEY_LEN / 10 + rand() % 60;
    size_t new_len = len;
    size_t status = len;
    while (status == len) {
        len = new_len;
        fprintf(stdout, "len = %zu\n", len);
        char key[MAX_KEY_LEN] = {};
        status = fread(key, sizeof(char), len, inp);
        gHashSet_insert(h, key, NULL);
        new_len = MAX_KEY_LEN / 10 + rand() % 60;
    }

    gHashSet_print(h, stderr);
    FILE *out = fopen("stat.txt", "w");
    gHashSet_statistics(h, out);
    fclose(out);

    gHashSet_dtor(h);
}
