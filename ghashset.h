#include <stdio.h>
#include <stdlib.h>


static const size_t MAX_KEY_LEN = 100;      // This way sizeof(gObjPool_Node) == 128

typedef struct {
    char key[MAX_KEY_LEN];
    char *value;
} gHashSet_Node;

#define GLIST_TYPE gHashSet_Node

#include "glist.h"

#define GHASHSET_HASH(hash, x) hash + x

typedef struct {
    gList *list;
    size_t *table;
    size_t capacity;
    size_t size;
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
#define GHASHSET_CHECK_POOL(status) ({                             \
    if ((status) != gObjPool_status_OK)                                 \
        GHASHSET_ASSERT_LOG(false, (gHashSet_status)status, ctx->logStream);  \
})


/**
 * @brief Local version of ASSERT_LOG macro
 */
#ifndef NLOGS
#define GHASHSET_ASSERT_LOG(expr, errCode, logStream) ({                                                  \
    if (!(expr)) {                                                                                      \
        fprintf((logStream),  "%s in %s on line %d!\n", gHashSet_statusMsg[(errCode)], __func__, __LINE__); \
        return (gHashSet_status)(errCode);                                                                   \
    }                                                                                                      \
})
#define GHASHSET_ASSERT_LOG(expr, errCode) ({                                                  \
    if (!(expr)) {                                                                                      \
        fprintf((ctx->logStream),  "%s in %s on line %d!\n", gHashSet_statusMsg[(errCode)], __func__, __LINE__); \
        return (gHashSet_status)(errCode);                                                                   \
    }                                                                                                      \
})
#else
#define GHASHSET_ASSERT_LOG(...)
#endif


/**
 * @brief Macro for easier and more secure node access in gObjPool
 */
#define GHASHSET_NODE_BY_ID(id) ({                                     \
    gHashSet_Node *node;                                                \
    GHASHSET_CHECK_POOL(gObjPool_get(&ctx->list->pool, id, &node));    \
    node;                                                              \
})


/**
 * @brief Macro for handy and secure allocation
 */
#define GHASHSET_POOL_ALLOC() ({                                                                      \
    size_t macroId = -1;                                                                            \
    gHashSet_Node *macroNode = NULL;                                                                    \
    GHASHSET_CHECK_POOL(gObjPool_alloc(&ctx->list->pool, &macroId));                                   \
    GHASHSET_CHECK_POOL(gObjPool_get(&ctx->list->pool, macroId, &macroNode));                           \
    macroNode->sibling = -1;                                                                            \
    macroNode->parent  = -1;                                                                             \
    macroNode->child   = -1;                                                                              \
    macroId;                                                                                               \
})


/**
 * @brief Macro for handy and secure deallocation
 */
#define GHASHSET_POOL_FREE(id) ({                                                            \
    GHASHSET_CHECK_POOL(gObjPool_free(&ctx->list->pool, id));                               \
})


/**
 * @brief Macro to check if expression or status is OK
 */
#define GHASHSET_IS_OK(expr) ({                                                      \
    gHashSet_status macroStatus = (expr);                                             \
    GHASHSET_ASSERT_LOG(macroStatus == gHashSet_status_OK, macroStatus, ctx->logStream); \
})


#define GHASHSET_ID_VAL(id) GHASHSET_ASSERT_LOG(gObjPool_idValid(&ctx->list->pool, id), gHashSet_status_BadId, ctx->logStream)

#define GHASHSET_SELF_CHECK(ctx) ({ \
    GHASHSET_ASSERT_LOG(gPtrValid(ctx), gHashSet_status_BadStructPtr, stderr);  \
    GHASHSET_ASSERT_LOG(ctx->capacity != 0 && ctx->size <= ctx->capacity, gHashSet_status_BadCapacity); \
})

/**
 * @brief gHashSet constructor that initiates objPool and logStream and creates zero node
 * @param ctx pointer to structure to construct on
 * @param newLogStream new log stream, could be `NULL`, then logs will be written to `stderr`
 * @return gHashSet status code
 */
static gHashSet_status gHashSet_ctor(gHashSet *ctx, size_t capacity, FILE *newLogStream)
{
    if (!gPtrValid(ctx)) {
        FILE *out;
        if (!gPtrValid(newLogStream))
            out = stderr;
        else
            out = newLogStream;
        GHASHSET_ASSERT_LOG(false, gHashSet_status_BadStructPtr)
    }

    ctx->logStream = stderr;
    if (gPtrValid(newLogStream))
        ctx->logStream = newLogStream;

    ctx->list = gList_new(ctx->logStream, capacity);
    GHASHSET_ASSERT_LOG(ctx->list != NULL, gHashSet_status_AllocErr);
    ctx->table = (size_t*)calloc(capacity, sizeof(size_t));
    GHASHSET_ASSERT_LOG(ctx->table != NULL, gHashSet_status_AllocErr);
    memfill(ctx->table, 0xFF, capacity);
    assert(ctx->table[0] == -1);    //TODO
    assert(ctx->table[1] == -1);
    assert(ctx->table[2] == -1);
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
    while (*key != 0)
        hash += GHASHSET_HASH(*(key++));
    hash %= ctx->capacity;

    size_t lastId = -1;
    if (ctx->table[hash] == -1)
        lastId = GHASHSET_NODE_BY_ID(ctx->list->zero)->prev;
    else
        lastId = GHASHSET_NODE_BY_ID(ctx->table[hash])->prev;

    gHashSet_Node node = {};
    node->value = value;
    strncpy(node->key, key, MAX_KEY_LEN - 1);

    gList_insertByNode(ctx->list, lastId, node);        //TODO check status

    return gHashSet_status_OK;
}

static gHashSet_status gHashSet_find(gHashSet *ctx, char *key, char **value_out)
{
    GHASHSET_SELF_CHECK(ctx);

    size_t hash = 0;
    while (*key != 0)
        hash = GHASHSET_HASH(hash, *(key++));
    hash %= ctx->capacity;

    if (ctx->table[hash] == -1) {
        *value_out = NULL;
        return gHashSet_status_OK;
    }

    size_t curId = ctx->table[hash];
    gList_Node node = GHASHSET_NODE_BY_ID(curId);
    while (curId != ctx->list->zero && !strncmp(node->data->key, key, MAX_KEY_LEN)) {
        curId = node->next;
        node = GHASHSET_NODE_BY_ID(curId);
    }

    if (curId == ctx->list->zero)
        *value_out = NULL;
    else
        *value_out = node->value;

    return gHashSet_status_OK;
}
