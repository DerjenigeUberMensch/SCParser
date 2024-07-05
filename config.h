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
#ifndef _SIMPLE_CONFIG_H 
#define _SIMPLE_CONFIG_H


#include <stdint.h>
#include <stdlib.h>



typedef struct CFG CFG;
typedef struct CFGItem CFGItem;

typedef uint32_t CFGID;


struct CFG
{
    char *file;
    CFGItem *items;
    uint32_t items_len;
};

struct CFGItem
{
    size_t size;
    void *data;
    char *name;
};

enum ParseCode
{
    ParseSuccess,
    ParseError,
    ParsePartialError,
    ParseOverflow,
    ParseEOF,
};

CFG *
CFGCreate(
        char *FILE_NAME
        );

void
CFGDestroy(
        CFG *cfg
        );

CFGItem *
CFGCreateItem(
        void
        );

/* fills err_return with non 0, on failure.
 */
CFGID
CFGCreateVar(
        CFG *cfg, 
        char *VarName, 
        size_t min_size,
        uint8_t *err_return
        );

/*
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
CFGGetVarValue(
        CFG *cfg, 
        char *VarName,
        void *filldata,
        size_t maxsize
        );
/*
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
CFGGetVarValueF(
        CFG *cfg,
        CFGID id,
        void *filldata,
        size_t maxsize
        );

/* 
 * RETURN: 0 On Success.
 * RETURN: 1 On Failure.
 */
int
CFGSaveVar(
        CFG *cfg, 
        char *VarName, 
        void *data,
        size_t size
        );

int
CFGSaveVarF(
        CFG *cfg, 
        CFGID id,
        void *data,
        size_t size
        );

int
CFGWrite(
        CFG *cfg
        );

int
CFGLoad(
        CFG *OldCfg
        );











#endif







