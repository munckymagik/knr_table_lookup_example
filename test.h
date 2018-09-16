#ifndef TEST_H
#define TEST_H

#include <assert.h>

#define TEST(description, body)    \
    printf("%s: ", (description)); \
    { body }                       \
    puts("PASS");                  \

#endif /* TEST_H */
