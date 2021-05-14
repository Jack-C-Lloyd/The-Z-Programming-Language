/*****************************************************************************
*                   Copyright (c) 2020-2021 Jack C. Lloyd.                   *
*                            All rights reserved.                            *
*****************************************************************************/

#include "../include/context.h"

/*****************************************************************************
*                              Standard Library                              *
*****************************************************************************/

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
*                                 Data Types                                 *
*****************************************************************************/

#define KEY_LENGTH (256)
#define MAP_LENGTH (256)

/**
 * A key-value pair data structure, to be contained in a map (hash table).
 */
struct pair
{
  char key[KEY_LENGTH]; /**< The first half of a key-value pair. */
  int  value;           /**< The second half of a key-value pair. */
};

/**
 * A map data structure, implemented as a hash table, containing pairs.
 */
struct map
{
  struct pair pairs[MAP_LENGTH]; /**< An array of pairs, forming the map. */
  size_t size;                   /**< The size of the map; not the length. */
};

/**
 * A stack data structure, implemented as a linked list, containing a map.
 */
struct stack
{
  struct stack *tail; /**< The next item(s) in the stack. */
  struct map map;     /**< The map. */
};

/*****************************************************************************
*                                  Contexts                                  *
*****************************************************************************/

/**
 * A context data structure, containing a stack of maps, forming scopes.
 */
struct context
{
  struct stack *head; /**< The head of the stack. */
  size_t size;        /**< The size of said stack. */
};

/**
 * Allocate a context.
 * 
 * @return An initialised context.
 * @see    ctxfree()
 */
struct context *
ctxalloc (void)
{
  struct context *context = (struct context *) malloc (sizeof (struct context));

  if (context == NULL)
    {
      return (NULL);
    }

  context->head = (struct stack *) malloc (sizeof (struct stack));
  context->size = 1;

  if (context->head == NULL)
    {
      free (context);

      return (NULL);
    }

  for (size_t i = 0; i < MAP_LENGTH; i++)
    {
      memset (context->head->map.pairs[i].key, 0, KEY_LENGTH);
              context->head->map.pairs[i].value = 0;
    }

  context->head->map.size = 0;

  return (context);
}

/**
 * Free a context.
 * 
 * @param context The context to free.
 * @see           ctxalloc()
 */
void
ctxfree (struct context *context)
{
  if (context == NULL)
    {
      return;
    }
  
  struct stack *head = context->head;
  struct stack *tail = context->head->tail;

  for ( ; ; )
    {
      free (head);

      if (tail == NULL)
        {
          return;
        }
      
      head = tail;
      tail = head->tail;
    }
}

/**
 * Reset a context.
 * 
 * @param context The context to reset.
 * @see           ctxalloc() and ctxfree().
 */
void
ctxreset (struct context *context)
{
  if (context == NULL)
    {
      return;
    }
  
  struct stack *head = context->head;
  struct stack *tail = context->head->tail;

  while (tail != NULL)
    {
      free (head);
      
      head = tail;
      tail = head->tail;
    }
  
  for (size_t i = 0; i < MAP_LENGTH; i++)
    {
      memset (context->head->map.pairs[i].key, 0, KEY_LENGTH);
              context->head->map.pairs[i].value = 0;
    }
}

/*****************************************************************************
*                                Push and Pop                                *
*****************************************************************************/

/**
 * Push a scope onto a context.
 * 
 * @param context The context to push a scope onto.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxpop()
 */
int
ctxpush (struct context *context)
{
  if (context == NULL)
    {
      return (EXIT_NULLPTR);
    }
  
  struct stack *head = (struct stack *) malloc (sizeof (struct stack));

  if (head == NULL)
    {
      return (EXIT_MALLOC);
    }

  for (size_t i = 0; i < MAP_LENGTH; i++)
    {
      memset (head->map.pairs[i].key, 0, KEY_LENGTH);
              head->map.pairs[i].value = 0;
    }
  
  head->map.size = 0;
  head->tail = context->head;

  context->head = head;
  context->size++;

  return (EXIT_SUCCESS);
}

/**
 * Pop a scope from a context.
 * 
 * @param context The context to pop a scope from.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxpush()
 */
int
ctxpop (struct context *context)
{
  if (context == NULL)
    {
      return (EXIT_NULLPTR);
    }

  if (context->size <= 1)
    {
      return (EXIT_MINIMISED);
    }

  struct stack *head = context->head;

  context->head = context->head->tail;
  context->size--;

  free (head);

  return (EXIT_SUCCESS);
}

/*****************************************************************************
*                      Hash, Insert, Search, and Delete                      *
*****************************************************************************/

/**
 * The hashing function.
 * @param key The first half of a key-value pair to insert.
 * @return    The hash value of said key.
 * @see       ctxinsert(), ctxsearch(), and ctxdelete()
 * 
 */
size_t
ctxhash (char const *key)
{
  size_t hash = 0;

  for (char const *it = key; *it != '\0'; it++)
    {
      hash += (size_t)*it;
      hash *= (size_t)*it;
      hash %= MAP_LENGTH;
    }
  
  return (hash);
  //return (hash % MAP_LENGTH);
}

/**
 * Insert into a context.
 * 
 * @param context The context to insert into.
 * @param key     The first half of a key-value pair to insert.
 * @param value   The second half of a key-value pair to insert.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxhash(), ctxsearch(), and ctxdelete()
 */
int
ctxinsert (struct context *context, char const *key, int value)
{
  if (context == NULL || key == NULL)
    {
      return (EXIT_NULLPTR);
    }

  if (context->head->map.size >= MAP_LENGTH) // ==
    {
      return (EXIT_MAXIMISED);
    }

  struct map *map = &(context->head->map);

  for (size_t i = ctxhash (key); /* 1 */; i = (i + 1) % MAP_LENGTH)
    {
      if (map->pairs[i].key[0] == '\0')
        {
          strcpy (map->pairs[i].key, key);
                  map->pairs[i].value = value;
                  map->size++;
          break;
        }

      if (strcmp (map->pairs[i].key, key) == EXIT_SUCCESS)
        {
          return (EXIT_REDEFINED);
        }
    }
  
  return (EXIT_SUCCESS);
}

/**
 * Search through a context.
 * 
 * @param context The context to search through.
 * @param key     The first half of a key-value pair with which to search.
 * @param value   A pointer to the second half of said key-value pair.
 * @return        Zero on success, otherwise an error code.
 * @see           ctxhash(), ctxinsert(), and ctxdelete()
 */
int
ctxsearch (struct context *context, char const *key, int *value)
{
  if (context == NULL || key == NULL || value == NULL)
    {
      return (EXIT_NULLPTR);
    }
  
  size_t const hash = ctxhash (key);

  for (struct stack *head = context->head; head != NULL; head = head->tail)
    {
      if (head->map.size <= 0) // ==
        {
          continue;
        }

      size_t index = hash;

      for (size_t count = 0; count < MAP_LENGTH; count++)
        {
          if (head->map.pairs[index].key[0] == '\0')
            {
              break;
            }
          
          if (strcmp (head->map.pairs[index].key, key) == EXIT_SUCCESS)
            {
              *value = head->map.pairs[index].value;
              
              return (EXIT_SUCCESS);
            }

          index = (index + 1) % MAP_LENGTH;
        }
    }
  
  return (EXIT_UNDEFINED);
}

/* int
ctxdelete (struct context *context, char const *key); */