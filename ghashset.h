#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <algorithm>

#include <nmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>


#include "hashfuncs.h"


static const size_t MAX_KEY_LEN = 64;       // for avx512 intrin

#pragma pack(push, 1)
typedef struct {
    char key[MAX_KEY_LEN];
    size_t hash;
    char *value;
    char alignment[8];                      // This way sizeof(gObjPool_Node) == 128
} gHashSet_Node;
#pragma pack(pop)

#define GLIST_TYPE gHashSet_Node
#define GLIST_PRINTF_CODE

#include "glist.h"

#ifndef GHASHSET_HASH
#define GHASHSET_HASH(hash, x) hash_rol_asm(hash, x)
#endif

typedef struct {
    gList  *list;
    size_t *table;
    size_t  capacity;
    size_t  size;
    FILE   *logStream;
} gHashSet;

/**
 * @brief status codes for gHashSet
 */
enum gHashSet_status
{
    gHashSet_status_OK,
    gHashSet_status_AllocErr,
    gHashSet_status_BadCapacity,
    gHashSet_status_BadStructPtr,
    gHashSet_status_BadId,
    /* WANINIG: statuses above must be compatible with gObjPool */

    gHashSet_status_BadPos,
    gHashSet_status_BadOutPtr,
    gHashSet_status_BadNodePtr,
    gHashSet_status_BadDumpOutPtr,
    gHashSet_status_BadData,
    gHashSet_status_BadRestoration,
    gHashSet_status_FileErr,
    gHashSet_status_ZeroCap,
    gHashSet_status_Cnt,
};


/**
 * @brief status codes explanations and error msgs for logs
 */
static const char gHashSet_statusMsg[gHashSet_status_Cnt][MAX_MSG_LEN] = {
    "OK",
    "Allocation error",
    "Bad capacity error",
    "Bad structure pointer provided",
    "Bad id provided",
    "Bad position requested",
    "WARNING: Bad param_out ptr provided",
    "Bad node pointer provided",
    "Bad FILE pointer provided to graphViz dump",
    "Error during data restoration",
    "Error during ctx restoration",
    "Error, hashset has not been initialised",
    "Error in file IO",
};


/**
 * @brief macro that checks if objPool status is OK and convers error code to compatible gHashSet_status otherwize
 */
#define GHASHSET_CHECK_POOL(status) ({                                 \
    if ((status) != gObjPool_status_OK)                                 \
        GHASHSET_ASSERT_LOG(false, (gHashSet_status)status);             \
})


/**
 * @brief Local version of ASSERT_LOG macro
 */
#ifndef NLOGS
#define GHASHSET_ASSERT_LOG(expr, errCode) ({                                                                   \
    if (!(expr)) {                                                                                               \
        fprintf((ctx->logStream),  "%s in %s on line %d!\n", gHashSet_statusMsg[(errCode)], __func__, __LINE__);  \
        return (gHashSet_status)(errCode);                                                                         \
    }                                                                                                               \
})
#else
#define GHASHSET_ASSERT_LOG(...)
#endif


/**
 * @brief Macro for easier and more secure node access in gObjPool
 */
#define GHASHSET_NODE_BY_ID(id) ({                                  \
    GHASHSET_ID_VAL(id);                                             \
    GOBJPOOL_VAL_BY_ID_UNSAFE(ctx->list->pool, id);                   \
})

/**
 * @brief Macro for handy and secure allocation
 */
#define GHASHSET_POOL_ALLOC() ({                                                                   \
    size_t macroId = -1;                                                                            \
    GHASHSET_CHECK_POOL(gObjPool_alloc(ctx->list->pool, &macroId));                                  \
    gHashSet_Node *macroNode = GHASHSET_NODE_BY_ID(macroId);                                          \
    macroNode->sibling = -1;                                                                           \
    macroNode->parent  = -1;                                                                            \
    macroNode->child   = -1;                                                                             \
    macroId;                                                                                              \
})


/**
 * @brief Macro for handy and secure deallocation
 */
#define GHASHSET_POOL_FREE(id) ({                                                            \
    GHASHSET_CHECK_POOL(gObjPool_free(ctx->list->pool, id));                                  \
})


/**
 * @brief Macro to check if expression or status is OK
 */
#define GHASHSET_IS_OK(expr) ({                                                      \
    gHashSet_status macroStatus = (expr);                                             \
    GHASHSET_ASSERT_LOG(macroStatus == gHashSet_status_OK, macroStatus);               \
})

/**
 * @brief Macro to check if expression or status is OK
 */
#define GHASHSET_LIST_OK(expr) ({                                                      \
    gList_status macroStatus = (expr);                                                  \
    GHASHSET_ASSERT_LOG(macroStatus == gList_status_OK, macroStatus);                    \
})

#ifndef NDEBUG
#define GHASHSET_ID_VAL(id) GHASHSET_ASSERT_LOG(gObjPool_idValid(ctx->list->pool, id), gHashSet_status_BadId)
#else
#define GHASHSET_ID_VAL(...)
#endif

#define GHASHSET_SELF_CHECK(ctx) ({                                                                                 \
    ASSERT_LOG(gPtrValid(ctx), gHashSet_status_BadStructPtr,                                                         \
                gHashSet_statusMsg[gHashSet_status_BadStructPtr], stderr);                                            \
    ASSERT_LOG((ctx->capacity != 0 && ctx->capacity != -1 && ctx->size <= ctx->capacity), gHashSet_status_BadCapacity, \
                gHashSet_statusMsg[gHashSet_status_BadCapacity], stderr);                                               \
})


static gHashSet_status gHashSet_ctor(gHashSet *ctx, size_t capacity, FILE *newLogStream)
{
    if (!gPtrValid(ctx)) {
        FILE *out;
        if (!gPtrValid(newLogStream))
            out = stderr;
        else
            out = newLogStream;
        ASSERT_LOG(false, gHashSet_status_BadStructPtr, gHashSet_statusMsg[gHashSet_status_BadStructPtr], out);
    }
    assert(sizeof(gObjPool_Node) % 64 == 0);

    ctx->logStream = stderr;
    if (gPtrValid(newLogStream))
        ctx->logStream = newLogStream;

    ctx->list = gList_new(ctx->logStream, capacity);
    GHASHSET_ASSERT_LOG(ctx->list != NULL, gHashSet_status_AllocErr);
    ctx->table = (size_t*)calloc(capacity, sizeof(size_t));
    GHASHSET_ASSERT_LOG(ctx->table != NULL, gHashSet_status_AllocErr);
    memset(ctx->table, 0xFF, capacity * sizeof(size_t));
    ctx->capacity = capacity;
    ctx->size = 0;

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_dtor(gHashSet *ctx)
{
    GHASHSET_SELF_CHECK(ctx);

    free(ctx->table);
    ctx->list = gList_delete(ctx->list);
    if (ctx->list != NULL)
        fprintf(ctx->logStream, "Failed to delete list!\n");
    ctx->capacity = -1;
    ctx->size = -1;
    ctx->table = NULL;

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_insert(gHashSet *ctx, char *key, char *value)
{
    GHASHSET_SELF_CHECK(ctx);

    size_t hash = 0;
    char *iter = key;
    while (*iter != 0)
        hash = GHASHSET_HASH(hash, *(iter++));
    hash %= ctx->capacity;

    size_t lastId = -1;
    if (ctx->table[hash] == -1)
        lastId = GHASHSET_NODE_BY_ID(ctx->list->zero)->prev;
    else
        lastId = GHASHSET_NODE_BY_ID(ctx->table[hash])->prev;

    gHashSet_Node node = {};
    node.value = value;
    node.hash = hash;
    strncpy(node.key, key, MAX_KEY_LEN - 1);

    GHASHSET_LIST_OK(gList_insertByNode(ctx->list, lastId, node));
    ctx->table[hash] = GHASHSET_NODE_BY_ID(lastId)->next;

    return gHashSet_status_OK;
}

#ifdef __AVX512__
inline static bool gHashSet_keyEq(char *one, char *other)
{
    __m512i f = _mm512_load_epi64(one);
    __m512i s = _mm512_load_epi64(other);

    __mmask8 res = _mm512_cmpeq_epu64_mask(f, s);
    return res - 255;
}
#endif

volatile inline static gHashSet_status gHashSet_find(gHashSet *ctx, char *key, char **value_out)
{
    GHASHSET_SELF_CHECK(ctx);

    size_t hash = 0;
    char *iter = key;
    while (*iter != 0)
        hash = GHASHSET_HASH(hash, *(iter++));
    hash %= ctx->capacity;

    if (ctx->table[hash] == -1) {
        *value_out = NULL;
        return gHashSet_status_OK;
    }

    size_t curId = ctx->table[hash];
    gList_Node *node = GHASHSET_NODE_BY_ID(curId);
    #ifdef __AVX512__
    while (curId != ctx->list->zero && gHashSet_keyEq(key, node->data.key)) {
    #else
    while (curId != ctx->list->zero && memcmp(key, node->data.key, MAX_KEY_LEN)) {
    #endif
        curId = node->next;
        node = GHASHSET_NODE_BY_ID(curId);
    }

    if (curId == ctx->list->zero)
        *value_out = NULL;
    else
        *value_out = node->data.value;

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_erase(gHashSet *ctx, char *key)
{
    GHASHSET_SELF_CHECK(ctx);

    size_t hash = 0;
    char *iter = key;
    while (*iter != 0)
        hash = GHASHSET_HASH(hash, *(iter++));
    hash %= ctx->capacity;

    if (ctx->table[hash] == -1)
        return gHashSet_status_OK;

    size_t curId = ctx->table[hash];
    gList_Node *node = GHASHSET_NODE_BY_ID(curId);
    while (curId != ctx->list->zero && strncmp(node->data.key, key, MAX_KEY_LEN)) {
        curId = node->next;
        node = GHASHSET_NODE_BY_ID(curId);
    }

    if (curId == ctx->list->zero)
        return gHashSet_status_OK;

    if (ctx->table[hash] == curId)
        ctx->table[hash] = node->next;
    GHASHSET_LIST_OK(gList_popByNode(ctx->list, curId, NULL));

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_print(gHashSet *ctx, FILE *out)
{
    GHASHSET_SELF_CHECK(ctx);

    for (size_t i = 0; i < ctx->capacity; ++i)
        fprintf(out, "%zu | id = %zu\n", i, ctx->table[i]);
    gList_Node *node = GHASHSET_NODE_BY_ID(ctx->list->zero);
    do {
        GHASHSET_LIST_OK(gList_getNextNode(ctx->list, node->id, &node));
        fprintf(out, "id = %zu | $%s$ | $%s$\n", node->id, node->data.key, node->data.value);
    } while (node->id != ctx->list->zero);

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_statistics(gHashSet *ctx, FILE *out)
{
    GHASHSET_SELF_CHECK(ctx);
    fprintf(stderr, "gHashSet statistics report:\n");
    size_t *data = (size_t*)calloc(ctx->capacity, sizeof(data));
    size_t curId = ctx->list->zero;
    GHASHSET_LIST_OK(gList_getNextId(ctx->list, curId, &curId));
    size_t cnt = 0;
    while (curId != ctx->list->zero) {
        gList_Node *node = GHASHSET_NODE_BY_ID(curId);
        ++data[node->data.hash];
        curId = node->next;
        ++cnt;
    }

    fprintf(stderr, "sizeof(gObjPool_Node) = %zu\n", sizeof(gObjPool_Node));
    size_t avg = 0;
    for (size_t i = 0; i < ctx->capacity; ++i) {
        avg += data[i];
        if (gPtrValid(out))
            fprintf(out, "%zu\n", data[i]);
    }
    avg /= ctx->capacity;

    std::sort(data, data + ctx->capacity);
    size_t min = data[0];
    size_t max = data[ctx->capacity - 1];
    size_t mid = data[ctx->capacity / 2];

    fprintf(stderr, "min = %zu\nmax = %zu\nagv = %zu\nmid = %zu\noverall = %zu\n", min, max, avg, mid, cnt);

    free(data);
    return gHashSet_status_OK;
}
