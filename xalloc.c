#include "xalloc.h"

#include <string.h>

static struct xalloc_stats stats = { 0, 0 };

void * xmalloc(size_t size) {
    void *p = malloc(size);

    if (p == NULL) return NULL;

    stats.count += size;
    stats.total += size;
    return p;
}

char * xstrdup(const char *s) {
    size_t size = strlen(s);
    stats.count += size;
    stats.total += size;
    return strdup(s);
}

void xfree(void *p, size_t size) {
    stats.count -= size;
    free(p);
}

void xfree_str(char *s) {
    xfree(s, strlen(s));
}

struct xalloc_stats xalloc_get_stats() {
    return stats;
}

