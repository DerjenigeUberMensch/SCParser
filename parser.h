#ifndef _SIMPLE_CONFIG_PARSER_H
#define _SIMPLE_CONFIG_PARSER_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


enum SCType
{
    SCTypeNoType,
    SCTypeUCHAR,
    SCTypeCHAR,
    SCTypeUSHORT,
    SCTypeSHORT,
    SCTypeUINT,
    SCTypeINT,
    SCTypeLONG,
    SCTypeULONG,
    SCTypeSTRING,
};

enum SCParseCode
{
    ParseSuccess,
    ParseError,
    ParsePartialError,
    ParseOverflow,
    ParseEOF,
};


typedef struct _SP_PARSER_STRUT SCParser;
typedef struct _SP_PARSER_ITEM SCItem;

/* Quick time function, on failure use SCParserSearchSlow();
 *
 * RETURN: SCItem * on Success.
 * RETURN: NULL on Failure.
 */
SCItem *
SCParserSearch(
        SCParser *parser,
        const char *const NAME
        );

/* Quick time function, on failure use SCParserSearchSlow();
 *
 * RETURN: SCItem * on Success.
 * RETURN: NULL on Failure.
 */
SCItem *
SCParserSearchSlow(
        SCParser *parser,
        const char *const NAME
        );

/* If no type given data is memcpied to _return,
 * else data is formated as _optional_type.
 *
 * NOTE: If _optional_type specified as STRING User must free resulting char * copied into _return.
 * NOTE: usage of _optional_type greatly increases chance of data being correct.
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
SCParserLoad(
        SCItem *item,
        void *_return,
        const size_t bytescopy,
        const enum SCType _optional_type
        );

/* 
 *
 * NOTE: No checks are made for unresonable BASE_VAR_COUNT, this is purely a constant optimization.
 *
 * RETURN: SCParser * on success.
 * RETURN: NULL on Failure.
 */
SCParser *
SCPParserCreate(
        const char *const FILE_NAME,
        const uint32_t BASE_VAR_COUNT
        );

/*
 * Frees parser data, and itself, any references should be terminated.
 */
void
SCParserDestroy(
        SCParser *parser
        );

/* Replaces variable data from parser if VAR_NAME found and applicable.
 *
 * NOTE: Only searches Previously created variables.
 *
 * NOTE: ANY PREVIOUS DATA in parser IS DESTROYED, if present.
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
SCParserReadFile(
        SCParser *parser,
        const char *const FILE_NAME
        );


/*
 *
 * NOTE: READONLY_SECTION == 1, VAR_NAME is a const static variable no memory is allocated.
 * NOTE: READONLY_SECTION == 0, VAR_NAME is not a const static varaible is memory is allocated.
 *
 * NOTE: VAR_NAME_FULL_LENGTH is the full length of the string, including the nullbyte '0'.
 *
 * NOTE: size is not recomended for usage and unsupported for STRING types.
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
SCParserNewVar(
        SCParser *parser,
        const char *const VAR_NAME,
        const uint32_t VAR_NAME_FULL_LENGTH,
        const uint8_t READONLY_SECTION,
        const size_t size,
        const enum SCType _optional_type
        );

/*
 * RETURN: 0 if VAR_NAME exists.
 * RETURN: 1 if VAR_NAME doest exist.
 */
int
SCParserDelVar(
        SCParser *parser,
        const char *const VAR_NAME
        );
/*
 * NOTE: STRINGS are only saved by pointer reference, undefined behaviour may occur if freed.
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
SCParserSaveVar(
        SCParser *parser,
        const char *const VAR_NAME,
        void *data
        );



#endif
