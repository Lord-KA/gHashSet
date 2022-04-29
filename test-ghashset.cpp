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
#include "gtest/gtest.h"

#include <random>
#include <map>

std::mt19937 rnd(179);

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

#define ERASE(h, k) ({              \
    struct aligned buf = {};         \
    char *res = NULL;                 \
    strcpy(buf.data, k);               \
    gHashSet_erase(h, buf.data);        \
})

TEST(manual, basics)
{
    gHashSet hashset = {};
    gHashSet *h = &hashset;
    gHashSet_ctor(h, 10, NULL);

    char val1[] = "one";
    char val2[] = "two";
    char val3[] = "three";
    char val4[] = "four";
    char val5[] = "five";
    char val6[] = "six";
    gHashSet_insert(h, "abc", val1);
    gHashSet_insert(h, "bca", val2);
    gHashSet_insert(h, "hello", val3);
    gHashSet_insert(h, "fuck", val4);
    gHashSet_insert(h, "abcd", val5);
    gHashSet_insert(h, "cab", val6);

    char *res1 = NULL;
    char *res2 = NULL;
    res1 = FIND(h, "abc");
    res2 = FIND(h, "bca");
    EXPECT_STREQ(res1, "one");
    EXPECT_STREQ(res2, "two");
    printf("$%s$\n$%s$\n", res1, res2);

    gHashSet_statistics(h, NULL);
    gHashSet_print(h, stdout);
    printf("abc: $%s$\n", FIND(h, "abc"));
    printf("cab: $%s$\n", FIND(h, "cab"));
    printf("bca: $%s$\n", FIND(h, "bca"));
    printf("abcd: $%s$\n", FIND(h, "abcd"));

    res2 = FIND(h, "fuck");
    EXPECT_STREQ(res2, "four");
    printf("$%s$\n", res2);
    ERASE(h, "cab");

    res2 = FIND(h, "abcd");
    EXPECT_STREQ(res2, "five");
    printf("$%s$\n", res2);
    printf("abc: $%s$\n", FIND(h, "abc"));
    printf("cab: $%s$\n", FIND(h, "cab"));
    printf("bca: $%s$\n", FIND(h, "bca"));
    printf("abcd: $%s$\n", FIND(h, "abcd"));
    gHashSet_print(h, stdout);
    gHashSet_statistics(h, NULL);
    gHashSet_dtor(h);
}

TEST(automatic, general)
{
    gHashSet hashset = {};
    gHashSet *h = &hashset;
    gHashSet_ctor(h, 10, NULL);
    std::map<std::string, char*> check;

    for (size_t i = 0; i < 4000; ++i) {
        char key[MAX_KEY_LEN] = {};
        size_t len = MAX_KEY_LEN / 2 + rnd() % 10;
        for (size_t j = 0; j < len; ++j)
            key[j] = rnd() % 100 + 48;
        char *val = (char*)rnd();
        check[key] = val;
        gHashSet_insert(h, key, val);
    }
    for (auto it=check.begin(); it!=check.end(); ++it) {
        char *k = (char*)(it->first.c_str());
        char *v1 = it->second;
        char *v2 = FIND(h, k);
        EXPECT_EQ(v1, v2);
        EXPECT_TRUE(v1 != NULL);
    }

    gHashSet_statistics(h, NULL);

    for (size_t i = 0; i < 3000; ++i) {
        size_t pos = rnd() % check.size();
        auto it = check.begin();
        for (size_t j = 0; j < pos; ++j)
            ++it;
        char *k = (char*)it->first.c_str();
        char *v1 = it->second;
        char *v2 = FIND(h, k);
        EXPECT_EQ(v1, v2);
        ERASE(h, k);
        v2 = FIND(h, k);
        check.erase(it);
        EXPECT_TRUE(v2 == NULL);

        if (i % 10 == 0)
           for (auto it = check.begin(); it != check.end(); ++it) {
                char *k = (char*)(it->first.c_str());
                char *v1 = it->second;
                char *v2 = FIND(h, k);
                EXPECT_EQ(v1, v2);
                if (v1 != v2) {
                    gHashSet_statistics(h, NULL);
                    gHashSet_print(h, stdout);
                }
                EXPECT_TRUE(v1 != NULL);
            }
    }

    for (size_t i = 0; i < 4000; ++i) {
        char key[MAX_KEY_LEN] = {};
        size_t len = MAX_KEY_LEN / 2 + rnd() % 10;
        for (size_t j = 0; j < len; ++j)
            key[j] = rnd() % 100 + 48;
        char *val = (char*)rnd();
        check[key] = val;
        gHashSet_insert(h, key, val);
    }

    gHashSet_statistics(h, NULL);

    for (size_t i = 0; i < 300; ++i) {
        size_t pos = rnd() % check.size();
        auto it = check.begin();
        for (size_t j = 0; j < pos; ++j)
            ++it;
        char *k = (char*)it->first.c_str();
        char *v1 = it->second;
        char *v2 = FIND(h, k);
        EXPECT_EQ(v1, v2);
        ERASE(h, k);
        v2 = FIND(h, k);
        check.erase(it);
        EXPECT_TRUE(v2 == NULL);

        for (auto it = check.begin(); it != check.end(); ++it) {
            char *k = (char*)(it->first.c_str());
            char *v1 = it->second;
            char *v2 = FIND(h, k);
            EXPECT_EQ(v1, v2);
            EXPECT_TRUE(v1 != NULL);
        }
    }

    gHashSet_statistics(h, NULL);

    gHashSet_dtor(h);
}
