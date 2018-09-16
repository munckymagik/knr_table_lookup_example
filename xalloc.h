#if !defined(XALLOC_H)
#define XALLOC_H

#include <stdlib.h>
#include <stdint.h>

struct xalloc_stats {
    int count;
    int total;
};

void * xmalloc(size_t size);
char * xstrdup(const char *);
void xfree_str(char *);
void xfree(void *, size_t size);
struct xalloc_stats xalloc_get_stats();

#endif // XALLOC_H
