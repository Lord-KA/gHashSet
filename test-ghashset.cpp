#include "ghashset.h"

int main()
{
    gHashSet hashset = {};
    gHashSet *s = &hashset;
    gHashSet_ctor(s, 10, NULL);


    char val1[] = "one";
    char val2[] = "two";
    gHashSet_insert(s, "abc", val1);
    gHashSet_insert(s, "bca", val1);

    char *res1 = NULL;
    char *res2 = NULL;
    gHashSet_find(s, "abc", &res1);
    gHashSet_find(s, "bca", &res2);
    printf("$%s$\n$%s$\n", res1, res2);
    gHashSet_dtor(s);
}
