#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>  /* isdigit() */

#include "parser.h"


#include "khash.h"

/* hashing */
KHASH_MAP_INIT_STR(__STR__TABLE__, SCItem *)

struct 
_SP_PARSER_STRUT
{
    FILE *fr;
    SCItem *items;
    uint32_t item_len;
    uint32_t index;
    khash_t(__STR__TABLE__) *strtable;
};

struct
_SP_PARSER_ITEM
{
    char *name;
    uint32_t name_len;
    uint8_t allocated;
    uint8_t type;
    uint16_t size;
};



static const char *const
__SC_GET_FORMAT_FROM_TYPE(const enum SCType t)
{
    switch(t)
    {
        case SCTypeNoType:
            return NULL;
        case SCTypeCHAR:
            return "%c";
        case SCTypeUCHAR:
            return "%d";
        case SCTypeSHORT:
            return "%d";
        case SCTypeUSHORT:
            return "%d";
        case SCTypeINT:
            return "%d";
        case SCTypeUINT:
            return "%u";
        case SCTypeLONG:
            return "%ld";
        case SCTypeULONG:
            return "%lu";
        case SCTypeSTRING:
            return "%s";
    }
    return NULL;
}

static const unsigned int
__SC_GET_SIZE_FROM_TYPE(const enum SCType t)
{
    switch(t)
    {
        case SCTypeNoType:
            return 0;
        case SCTypeCHAR:
            return sizeof(int8_t);
        case SCTypeUCHAR:
            return sizeof(uint8_t);
        case SCTypeSHORT:
            return sizeof(int16_t);
        case SCTypeUSHORT:
            return sizeof(uint16_t);
        case SCTypeINT:
            return sizeof(int32_t);
        case SCTypeUINT:
            return sizeof(uint32_t);
        case SCTypeLONG:
            return sizeof(int64_t);
        case SCTypeULONG:
            return sizeof(uint64_t);
        case SCTypeSTRING:
            return 0;
    }
    return 0;
}

SCItem *
SCParserSearch(
        SCParser *parser,
        const char *const NAME
        )
{
    if(!parser)
    {   return NULL;
    }
    khint_t k = kh_get(__STR__TABLE__, parser->strtable, NAME);
    if(k != kh_end(parser->strtable))
    {   return kh_val(parser->strtable, k);
    }
    return NULL;
}

/* TODO */
SCItem *
SCParserSearchSlow(
        SCParser *parser,
        const char *const NAME
        )
{
    if(!parser)
    {   return NULL;
    }
    return NULL;
}

int
SCParserLoad(
        SCItem *item,
        void *_return,
        const size_t bytescopy,
        const enum SCType _optional_type
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;
    if(!item || !_return)
    {   return FAILURE;
    }

    const char *const format = __SC_GET_FORMAT_FROM_TYPE(_optional_type);
    const int SSCANF_CHECKSUM = 1;
    const int DATA_SIZE = 32;
    uint8_t check;
    uint8_t negative;
    uint8_t data[DATA_SIZE];
    int32_t i;

    /* type handler */
    if(_optional_type == SCTypeNoType)
    {   goto NOTYPE;
    }

    /* else we got a type */
    if(_optional_type == SCTypeSTRING)
    {
        const uint32_t len = item->name_len;
        const size_t size = len * sizeof(char);
        char *str = malloc(size);
        if(str)
        {   
            memcpy(str, item->name, size);
            memcpy(_return, &str, sizeof(char *));
            return SUCCESS;
        }
        return FAILURE;
    }
    if(!format)
    {   goto NOTYPE;
    }
    /* clear memory */
    memset(data, 0, sizeof(uint8_t) * DATA_SIZE);
    check = sscanf(item->name, format, &data);
    if(check == SSCANF_CHECKSUM)
    {   
        memcpy(_return, data, bytescopy);
        return SUCCESS;
    }
    return FAILURE;
NOTYPE:
    /* check if negative */
    negative = item->name[0] == '-';
    /* if its negative skip the negative sign duh */
    i = negative;
    for(; i < item->name_len - 1; ++i)
    {
        if(!isdigit(item->name[i]))
        {   break;
        }
    }
    memset(data, 0, sizeof(uint8_t) * DATA_SIZE);
    if(negative)
    {   check = sscanf(item->name, "%ld", (uint64_t *)&data);
    }
    else
    {   check = sscanf(item->name, "%lu", (uint64_t *)&data);
    }
    if(check == SSCANF_CHECKSUM)
    {
        memcpy(_return, data, bytescopy);
        return SUCCESS;
    }
    return FAILURE;
}

SCParser * 
SCPParserCreate(
        const char *const FILE_NAME,
        const uint32_t BASE_VAR_COUNT
        )
{
    SCParser *p = malloc(sizeof(SCParser));
    if(p)
    {
        p->fr = fopen(FILE_NAME, "r");
        if(!p->fr)
        {   
            free(p);
            return NULL;
        }
        p->strtable = kh_init(__STR__TABLE__);

        if(!p->strtable)
        {   
            fclose(p->fr);
            free(p);
            return NULL;
        }
        p->items = malloc(BASE_VAR_COUNT * sizeof(SCItem));
        if(!p->items)
        {
            fclose(p->fr);
            kh_destroy(__STR__TABLE__, p->strtable);
            free(p);
            return NULL;
        }
        p->item_len = BASE_VAR_COUNT;
        p->index = 0;
    }
    return p;
}

void
SCParserDestroy(
        SCParser *parser
        )
{
    uint32_t i;
    SCItem *item;
    for(i = 0; i < parser->index; ++i)
    {
        item = parser->items + i;
        if(item->allocated)
        {   free(item->name);
        }
    }
    free(parser->items);
    free(parser);
}

int
SCParserReadFile(
        SCParser *parser,
        const char *const FILE_NAME
        )
{
}

int
SCParserNewVar(
        SCParser *parser,
        const char *const VAR_NAME,
        const uint32_t VAR_NAME_FULL_LENGTH,
        const uint8_t READONLY_SECTION,
        const size_t size,
        const enum SCType _optional_type
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;
    if(!parser || !VAR_NAME)
    {   return FAILURE;
    }
    if(!size && !__SC_GET_FORMAT_FROM_TYPE(_optional_type))
    {   return FAILURE;
    }

    /* increase array size if too small */
    if(parser->index > parser->item_len)
    {
        const float INCREASE_FACTOR = 1.5f;
        const uint32_t newlen = parser->item_len * INCREASE_FACTOR;
        void *rec = realloc(parser->items, (newlen) * sizeof(SCItem));
        if(rec)
        {
            parser->items = rec;
            parser->item_len = newlen;
        }
        else
        {   return FAILURE;
        }
    }

    SCItem *items = parser->items;
    SCItem *item = parser->items + parser->index;

    if(READONLY_SECTION)
    {
        item->name = VAR_NAME;
        item->allocated = 0;
    }
    else
    {
        item->name = malloc(VAR_NAME_FULL_LENGTH * sizeof(char));
        item->allocated = 1;
        if(!item->name)
        {   return FAILURE;
        }
        memcpy(item->name, VAR_NAME, VAR_NAME_FULL_LENGTH);
    }
    item->name_len = VAR_NAME_FULL_LENGTH;

    if(_optional_type == SCTypeSTRING)
    {
        item->size = sizeof(char *);
        item->type = _optional_type;
    }
    else if(__SC_GET_FORMAT_FROM_TYPE(_optional_type))
    {   
        item->size = __SC_GET_SIZE_FROM_TYPE(_optional_type);
        item->type = _optional_type;
    }
    else if(size)
    {
        item->size = size;
        item->type = SCTypeNoType;
    }
    else
    {   return FAILURE;
    }
    /* update index */
    ++parser->index;

    /* add to table */

    int err = 3;
    khint_t k = kh_put(__STR__TABLE__, parser->strtable, item->name, &err);
    enum
    {
        __KHASH_BAD_OPERATION = -1,
        __KHASH_ALREADY_PRESENT = 0,
        __KHASH_FIRST_HASH = 1,
        __KHASH_PREVIOUSLY_DELETED = 2,
    };
    /* set value to client */
    switch(err)
    {
        case __KHASH_BAD_OPERATION:
            /* FALLTHROUGH */
        case __KHASH_ALREADY_PRESENT:
            break;

        case __KHASH_FIRST_HASH:
            /* FALLTHROUGH */
        case __KHASH_PREVIOUSLY_DELETED:
            /* FALLTHROUGH */
        default:
            /* bounds check */
            if(kh_end(parser->strtable) > k)
            {   kh_value(parser->strtable, k) = NULL;
            }
            break;
    }
    ++parser->index;
    return SUCCESS;
}

int
SCParserDelVar(
        SCParser *parser,
        const char *const VAR_NAME
        )
{
    SCItem *item = SCParserSearch(parser, VAR_NAME);
    if(!item)
    {   item = SCParserSearchSlow(parser, VAR_NAME);
    }
    if(item)
    {   
        if(item->allocated)
        {   free(item->name);
        }
        item->name = NULL;
        item->allocated = 0;
        item->type = 0;
        item->name_len = 0;
        item->size = 0;
    }
    return !item;
}

int
SCParserSaveVar(
        SCParser *parser,
        const char *const VAR_NAME,
        void *data,
        const size_t bytescopy,
        const enum SCType _optional_type
        )
{
}


