#include "xalloc.h"

#include <string.h>

static struct xalloc_stats stats = { 0, 0 };

static void increment_stats(size_t size) {
    stats.count += size;
    stats.total += size;
}

void * xmalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) return NULL;
    increment_stats(size);
    return p;
}

char * xstrdup(const char *s) {
    size_t str_len = strlen(s);
    increment_stats(str_len + 1);
    return strndup(s, str_len);
}

void xfree(void *p, size_t size) {
    stats.count -= size;
    free(p);
}

void xfree_str(char *s) {
    xfree(s, strlen(s) + 1);
}

struct xalloc_stats xalloc_get_stats() {
    return stats;
}

