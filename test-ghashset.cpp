#include "ghashset.h"

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

int main()
{
    gHashSet hashset = {};
    gHashSet *s = &hashset;
    gHashSet_ctor(s, 10, NULL);


    char val1[] = "one";
    char val2[] = "two";
    char val3[] = "three";
    char val4[] = "four";
    char val5[] = "five";
    gHashSet_insert(s, "abc", val1);
    gHashSet_insert(s, "bca", val2);
    gHashSet_insert(s, "hello", val3);
    gHashSet_insert(s, "fuck", val4);
    gHashSet_insert(s, "abcd", val5);

    char *res1 = NULL;
    char *res2 = NULL;
    res1 = FIND(s, "abc");
    res2 = FIND(s, "bca");
    printf("$%s$\n$%s$\n", res1, res2);

    printf("$%s$\n", FIND(s, "fuck"));
    gHashSet_erase(s, "bca");

    printf("$%s$\n", FIND(s, "abcd"));
    gHashSet_print(s, stdout);
    gHashSet_dtor(s);
}
