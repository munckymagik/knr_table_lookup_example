#ifndef TEST_H
#define TEST_H

#include <assert.h>

// A convenience wrapper to define a test block
#define TEST(description, body)        \
    do {                               \
        printf("%s: ", (description)); \
        { body }                       \
        puts("PASS");                  \
    } while(0)

// A convenience macro to disable a test block
#define XTEST(_description, _body)

#endif /* TEST_H */
