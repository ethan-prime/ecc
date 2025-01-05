#include "semantics.h"
#include <stdio.h>
#include <stdlib.h>

void semantic_error(char *message)
{
    printf("***semantic error:***\n");
    printf("%s\n", message);
    exit(1);
}

void semantic_error_declared_twice(char *identifier, identifiermap *im)
{
    printf("***semantic error:***\n");
    printf("variable %s redeclared\n", identifier);
    printf("variable map at exit:\n");
    identifiermap_print(im);
    exit(1);
}

void semantic_error_undefined(char *identifier, identifiermap *im)
{
    printf("***semantic error:***\n");
    printf("variable %s undefined\n", identifier);
    printf("variable map at exit:\n");
    identifiermap_print(im);
    exit(1);
}