#include <stdio.h>
#include <stdbool.h>

#define VERBOSE 0

/**
 * Prints a verbose message to the console.
 *
 * @param str The string to be printed.
 */
void printVerbose(const char *str);


/**
 * Returns the minimum of two integers.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The minimum of the two integers.
 */
int min(int a, int b);


/**
 * Returns the minimum value among three integers.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @param c The third integer.
 * @return The minimum value among the three integers.
 */
int min3(int a, int b, int c);
