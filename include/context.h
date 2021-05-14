/*****************************************************************************
*                   Copyright (c) 2020-2021 Jack C. Lloyd.                   *
*                            All rights reserved.                            *
*****************************************************************************/

#ifndef __CONTEXT__
#define __CONTEXT__ 20210514 /**< Format: YYYY-MM-DD. */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
*                              Standard Library                              *
*****************************************************************************/

#include <float.h>
#include <limits.h>
#include <stddef.h>

/*****************************************************************************
*                                 Data Types                                 *
*****************************************************************************/

#define NATURAL_MIN (UINT_MIN) /**< The minimum value held by a natural. */
#define NATURAL_MAX (UINT_MAX) /**< The maximum value held by a natural. */
#define INTEGER_MIN (INT_MIN)  /**< The minimum value held by an integer. */
#define INTEGER_MAX (INT_MAX)  /**< The maximum value held by an integer. */
#define REAL_MIN    (DBL_MIN)   /**< The minimum value held by a real. */
#define REAL_MAX    (DBL_MAX)   /**< The maximum value held by a real. */

typedef _Bool    boolean_t;   /**< The boolean type. */
typedef unsigned natural_t;   /**< The natural number type. */
typedef signed   integer_t;   /**< The integer number type. */
typedef double   real_t;      /**< The real number type. */
typedef char     character_t; /**< The character type. */
typedef char *   string_t;    /**< The string type. */

/*****************************************************************************
*                                  Contexts                                  *
*****************************************************************************/

struct context;

/**
 * Allocate a context.
 * 
 * @return An initialised context on success, otherwise a null-pointer.
 * @see    ctxfree() and ctxreset().
 */
struct context *
ctxalloc (void);

/**
 * Free a context.
 * 
 * @param context The context to free.
 * @see           ctxalloc() and ctxreset().
 */
void
ctxfree (struct context *context);

/**
 * Reset a context.
 * 
 * @param context The context to reset.
 * @see           ctxalloc() and ctxfree().
 */
void
ctxreset (struct context *context);

/*****************************************************************************
*                                Push and Pop                                *
*****************************************************************************/

/**
 * Push a scope onto a context.
 * 
 * @param context The context to push a scope onto.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxpop() and ctxerror().
 */
int
ctxpush (struct context *context);

/**
 * Pop a scope from a context.
 * 
 * @param context The context to pop a scope from.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxpush() and ctxerror().
 */
int
ctxpop (struct context *context);

/*****************************************************************************
*                      Hash, Insert, Search, and Delete                      *
*****************************************************************************/

/**
 * The hashing function.
 * @param key The first half of a key-value pair to insert.
 * @return    The hash value of said key.
 * @see       ctxinsert(), ctxsearch(), and ctxdelete().
 * 
 */
size_t
ctxhash (char const *key);

/**
 * Insert into a context.
 * 
 * @param context The context to insert into.
 * @param key     The first half of a key-value pair to insert.
 * @param value   The second half of a key-value pair to insert.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxhash(), ctxsearch(), ctxdelete(),  and ctxerror().
 */
int
ctxinsert (struct context *context, char const *key, int value);

/**
 * Search through a context.
 * 
 * @param context The context to search through.
 * @param key     The first half of a key-value pair with which to search.
 * @param value   A pointer to the second half of said key-value pair.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxhash(), ctxinsert(), ctxdelete(), and ctxerror().
 */
int
ctxsearch (struct context *context, char const *key, int *value);

/* int
ctxdelete (struct context *context, char const *key); */

/*****************************************************************************
*                                   Errors                                   *
*****************************************************************************/

#define EXIT_NULLPTR   (-0x01) /**< "Null pointer." */
#define EXIT_MALLOC    (-0x02) /**< "Memory allocation failure." */
#define EXIT_MINIMISED (-0x04) /**< "Memory is minimised." */
#define EXIT_MAXIMISED (-0x08) /**< "Memory is maximised." */
#define EXIT_REDEFINED (-0x10) /**< "Redefined." */
#define EXIT_UNDEFINED (-0x20) /**< "Undefined." */

/****************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !__CONTEXT__ */