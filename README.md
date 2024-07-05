# SCParser
A revised version of [ConfigParser](https://github.com/DerjenigeUberMensch/ConfigParser)

## Example
```C
#include <stdio.h>
#include <string.h>

#include "parser.h"

int
main()
{
    const char *const VAR_NAME = "VAR_INT";
    int var_int = 10;
    const int VAR_NUM_SHOWCASE = 1;

    SCParser *parser = SCPParserCreate(VAR_NUM_SHOWCASE);
    /* Failed to aloc memory for parser */
    if(!parser)
    {     return 1;
    }
    /* normally this would be a constant string, ie not memory allocated, preferably static, as I havent tested for stack allocated variables. */
    const int READONLY = 1;
    int failed = SCParserNewVar(parser, VAR_NAME, strlen(VAR_NAME) + 1, READONLY, sizeof(int), SCTypeINT);
    if(failed)
    {   /* We dont handle this in this example so just free data and exit. */
        SCParserDestroy(parser);
        return 1;
    }
    
    /* This can fail due to malloc() */
    failed = SCParserSaveVar(parser, "VAR_INT", &var_int);

    if(failed)
    {   /* We dont handle this in this example so just free data and exit. */
        SCParserDestroy(parser);
        return 1;
    }

    /* write data to file */
    const char *const FILE_NAME = "file_name";
    /* This should only fail if you input bad data/directory doesn't exist.
     * Overrites any previous data by the way, beware.
     */
    failed = SCParserWrite(parser, FILE_NAME);
    if(failed)
    {   /* See above comment. */
        SCParserDestroy(parser);
        return 1;
    }

    /* Load data to variables. */

    /* This should only fail if you input bad data/directory doesn't exist. or the file doesnt exist. */
    failed = SCParserReadFile(parser, FILE_NAME);

    if(failed)
    {   /* See above comment. */
        SCParserDestroy(parser);
        return 1;
    }

    int example_data = 0;
    SCItem *item = SCParserSearch(parser, VAR_NAME);
    /* khash must have failed a malloc, search linearly */
    if(!item)
    {   
        /* Assuming we didnt get a error from SCParserNewVar then item should always return here. */
        item = SCParserSearchSlow(parser, VAR_NAME);
    }
    /* This can fail for alot of reason, if its a string, malloc, however if it isnt, then sscanf(), must have failed, which again is bad data. */
    failed = SCParserLoad(item, &example_data, sizeof(int), SCTypeINT);
    if(failed)
    {   /* note untested for unknown types */
        SCParserDestroy(parser);
        return 1;
    }
    printf("OLD_DATA:   [%d]\n", var_int);
    printf("SAVED_DATA: [%d]\n", example_data);
    
    /* example, we dont have to when destroying parser: delete our var */
    failed = SCParserDelVar(parser, VAR_NAME);
    /* only fails if doesnt exist */
    if(failed)
    {   /* which isnt possible in this example */
    }

    /* cleanup */
    SCParserDestroy(parser);
    return 0;
}

```
