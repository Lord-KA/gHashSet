#include "ghashset.h"

int main()
{
    gHashSet hashset = {};
    gHashSet *s = &hashset;
    gHashSet_ctor(s, 10, NULL);


    char val1[] = "one";
    char val2[] = "two";
    char val3[] = "three";
    char val4[] = "four";
    gHashSet_insert(s, "abc", val1);
    gHashSet_insert(s, "bca", val2);
    gHashSet_insert(s, "hello", val3);
    gHashSet_insert(s, "fuck", val4);

    char *res1 = NULL;
    char *res2 = NULL;
    gHashSet_find(s, "abc", &res1);
    gHashSet_find(s, "bca", &res2);
    printf("$%s$\n$%s$\n", res1, res2);

    gHashSet_find(s, "fuck", &res1);
    printf("$%s$\n", res1);
    gHashSet_erase(s, "bca");

    gHashSet_print(s, stdout);
    gHashSet_dtor(s);
}
