// You can use these methods and add new ones not related to the simulation if
// you want.

#include "utils.h"

void printVerbose(const char *str)
{
    if (VERBOSE)
    {
        printf("%s", str);
    }
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int min3(int a, int b, int c)
{
    return min(min(a, b), c);
}
