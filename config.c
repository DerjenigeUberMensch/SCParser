/* MIT License
 *
 * Copyright (c) 2024- Joseph
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"



typedef union __Generic __Generic;

union __Generic
{
    void *v;
    void **vv;

    uint8_t uc;
    int8_t c;
    uint16_t us;
    int16_t s;
    int32_t i;
    uint32_t ui;
    int64_t ii;
    uint64_t uii;

    float f;
    double d;
};

static int /* FILE READ,   Buffer FILL data, Buffer Length */
__FILE_GET_NEW_LINE(FILE *fr, char *buff, unsigned int bufflength)
{
    char *nl;
    if(fgets(buff, bufflength, fr))
    {
        nl = strchr(buff, '\n');
        if(!nl)
        {
            if(!feof(fr))
            {   return ParseOverflow;
            }
        } /* remove new line char */
        else
        {   *nl = '\0';
        }
        return ParseSuccess;
    }
    else
    {
        if(ferror(fr))
        {   return ParseError;
        }
        return ParseEOF;
    }
}


static int 
__attach(CFG *cfg, CFGItem *item)
{
    if(!cfg)
    {   return 1;
    }
    if(!cfg->items)
    {   
        cfg->items = malloc(sizeof(CFGItem));
        cfg->items_len = 0;
    }
    else
    {   
        size_t size = (cfg->items_len + 1) * sizeof(CFGItem);
        void *rec = realloc(cfg->items, size);
        if(rec)
        {   cfg->items = rec;
        }
        else
        {   return 1;
        }
    }

    if(!cfg->items)
    {   return 1;
    }

    memcpy(cfg->items + cfg->items_len, item, sizeof(CFGItem));
    ++cfg->items_len;
    return 0;
}

static char *
__REMOVE_WHITE_SPACE(char *str, int str_len)
{
    char *ret = NULL;
    int reti;
    int i;
    const int len = str_len;
    int size = len + sizeof(char);
    ret = malloc(size);
    if(!ret)
    {   return NULL;
    }
    reti = 0;
    for(i = 0; i < len; ++i)
    {   
        if(str[i] != ' ')
        {   
            ret[reti] = str[i];
            ++reti;
        }
    }
    if(reti < size)
    {   
        size = reti + sizeof(char);
        void *tmp = realloc(ret, size);
        if(!tmp)
        {   
            free(ret);
            return NULL;
        }
        ret = tmp;
    }
    ret[size - 1] = '\0';
    return ret;
}

/* Allocated mememory must be freed by caller.
 * Can return NULL
 */
static char *
__CFG_PARSE_NAME(char *buff)
{
    const char *delimeter = "=";
    char *token = NULL;
    char *ret = NULL;
    const int maxlen = 1024;
    
    token = strtok(buff, delimeter);
    if(token)
    {   ret = __REMOVE_WHITE_SPACE(token, strnlen(token, maxlen));
    }
    return ret;
}

static char *
__CFG_PARSE_VALUE_STR(char *buff)
{
    const char *delimeter = "=";
    char *token = NULL;
    char *ret = NULL;
    const int maxlen = 1024;
    
    token = strtok(buff, delimeter);
    if(token)
    {   ret = __REMOVE_WHITE_SPACE(token, strnlen(token, maxlen));
    }
    return ret;
}


static CFGItem *
__CFG_GET_VAR_FROM_STRING(CFG *cfg, const char *VarName)
{
    CFGItem *item = NULL;
    CFGItem *ret = NULL;
    CFGID i;
    for(i = 0; i < cfg->items_len; ++i)
    {
        item = cfg->items + i;
        if(!strcmp(item->name, VarName))
        {   
            ret = item;
            break;
        }
    }
    return ret;
}

static CFGItem *
__CFG_GET_VAR_FROM_ID(CFG *cfg, CFGID id)
{
    if(cfg->items_len > id && id >= 0)
    {   return cfg->items + id;
    }
    return NULL;
}

static char *
__CFG_GET_FORMAT_SPECIFIER_FROM_TYPE(size_t size)
{
    switch(size)
    {
        case sizeof(uint8_t):
            return "%uc";
        case sizeof(uint16_t):
            return "%d";
        case sizeof(uint32_t):
            return "%u";
        case sizeof(uint64_t):
            return "%lu";
    }
    return "%lu";
}

CFG *
CFGCreate(
        char *FILE_NAME
        )
{
    CFG *ret = malloc(sizeof(CFG));

    if(ret)
    {   
        ret->file = FILE_NAME;
        ret->items = NULL;
    }

    return ret;
}

void
CFGDestroy(
    CFG *cfg
    )
{
    CFGItem *item;
    CFGID i;
    for(i = 0; i < cfg->items_len; ++i)
    {
        item = cfg->items + i;
        free(item->data);
    }
    free(cfg->items);
    free(cfg);
}

CFGID
CFGCreateVar(
        CFG *cfg, 
        char *VarName, 
        size_t MinSize,
        uint8_t *err_return
        )
{
    CFGItem item;

    item.data = calloc(1, sizeof(__Generic));
    if(!item.data)
    {   
        free(item.data);
        if(err_return)
        {   *err_return = 1;
        }
        return 0;
    }
    item.name = VarName;
    item.size = MinSize;
    if(__attach(cfg, &item))
    {   
        free(item.data);
        if(err_return)
        {   *err_return = 1;
        }
        return 0;
    }
    return cfg->items_len - 1;
}

int
CFGGetVarValue(
        CFG *cfg, 
        char *VarName,
        void *fill_data,
        size_t max_size
        )
{
    CFGItem *item;
    if((item = __CFG_GET_VAR_FROM_STRING(cfg, VarName)))
    {   
        if(item->size < max_size)
        {   memcpy(fill_data, item->data, item->size);
        }
        else
        {   memcpy(fill_data, item->data, max_size);
        }
        return 0;
    }
    return 1;
}

int
CFGGetVarValueF(
        CFG *cfg,
        CFGID id,
        void *fill_data,
        size_t max_size
        )
{
    CFGItem *item;
    if((item = __CFG_GET_VAR_FROM_ID(cfg, id)))
    {   
        if(item->size < max_size)
        {   memcpy(fill_data, item->data, item->size);
        }
        else
        {   memcpy(fill_data, item->data, max_size);
        }
        return 0;
    }
    return 1;
}

/* Saves data specified by the variable name If it exists. 
 * One must pass in the address of the data wanting to be used, this includes strings interpreted as char *.
 * Do note that the data type must be correct as no bounds checks are made when copying memoery.
 *
 * EX: int x = 10; 
 *     CFGSaveVar(MyCfg, "MyVar", &x);
 * EX: char *str = "my cool string";
 *     char str2[] = "my cool string";
 *     CFGSaveVar(MyCfg, "MyVarString", &str);
 *     CFGSaveVar(MyCfg, "MyVarStringArray", &str2);
 * 
 *
 * RETURN: 0 On Success.
 * RETURN: 1 On Failure.
 */
int
CFGSaveVar(
        CFG *cfg, 
        char *VarName, 
        void *data,
        size_t size
        )
{
    CFGItem *item;
    __Generic *gen;
    if((item = __CFG_GET_VAR_FROM_STRING(cfg, VarName)))
    {   
        gen = item->data;
        if(size < item->size)
        {   memset(((uint8_t *)gen) + (item->size - size), 0, sizeof(uint8_t) * (item->size - size));
        }
        else
        {   size = item->size;
        }
        memcpy(gen, data, size);
    }
    return !item;
}

int
CFGSaveVarF(
        CFG *cfg,
        CFGID id,
        void *data,
        size_t size
        )
{
    CFGItem *item;
    __Generic *gen;
    if((item = __CFG_GET_VAR_FROM_ID(cfg, id)))
    {   
        gen = item->data;
        memcpy(gen, data, item->size);
    }
    return !item;
}

int
CFGWrite(
        CFG *cfg
        )
{
    FILE *fw = fopen(cfg->file, "w");

    if(!fw)
    {   return ParseError;
    }
    CFGItem *item;
    char *format;
    uint8_t error = ParseSuccess;
    CFGID i;
    for(i = 0; i < cfg->items_len; ++i)
    {
        item = cfg->items + i;
        format = __CFG_GET_FORMAT_SPECIFIER_FROM_TYPE(item->size);
        fprintf(fw, "%s=", item->name);
        fprintf(fw, format, item->data);
        fprintf(fw, "\n");
    }
    fclose(fw);
    return error;
}

int
CFGLoad(
        CFG *OldCfg
        )
{
    const int standardbufflimit = 1024 << 2;
    int running = 1;
    char buff[standardbufflimit];
    FILE *fr = fopen(OldCfg->file, "r");

    char *name = NULL;
    char *typename = NULL;
    if(!fr)
    {   return ParseError;
    }

    uint8_t error = ParseSuccess;

    CFGID i = 0;
    
    while(running)
    {
        switch(__FILE_GET_NEW_LINE(fr, buff, standardbufflimit))
        {
            case ParseSuccess: 
                break;
            case ParseEOF:
                running = 0;
                /* FALLTHROUGH */
            case ParseOverflow: case ParseError: default:
                continue;
        }
        name = __CFG_PARSE_NAME(buff);
        typename = __CFG_PARSE_VALUE_STR(NULL);
        if(!name || !typename)
        {   
            free(name);
            free(typename);
            continue;
        }
        CFGItem *item;
        char *format = NULL;
        if((item = __CFG_GET_VAR_FROM_STRING(OldCfg, name)))
        {
            if((format = __CFG_GET_FORMAT_SPECIFIER_FROM_TYPE(item->size)))
            { 
                __Generic ge;
                const uint8_t SSCANF_SUCCESS = 1;
                const uint8_t sscanfstatus = sscanf(typename, format, &ge);
                if(SSCANF_SUCCESS == sscanfstatus)
                {   
                    CFGSaveVar(OldCfg, name, &ge, sizeof(__Generic));
                    ++i;
                }
            }
        }
        free(name);
        free(typename);
    }
    fclose(fr);
    if(i != OldCfg->items_len)
    {   error = ParseError;
    }
    return error;
}
