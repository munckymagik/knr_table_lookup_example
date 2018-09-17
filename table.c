#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "xalloc.h"

#define HASH_SIZE 101
static struct table_entry *hash_table[HASH_SIZE] = { 0 };

static hash_value_t default_hash_function(const char *s) {
    hash_value_t hash_value = 0;

    while (*s != '\0') {
        hash_value = *s + 31 * hash_value;
        s++;
    }

    return hash_value % HASH_SIZE;
}

static hash_function_t hash_function = default_hash_function;

hash_value_t table_hash_function(const char *s) {
    return hash_function(s);
}

hash_function_t table_set_hash_function(hash_function_t f) {
    hash_function_t prev = hash_function;
    hash_function = f;
    return prev;
}

struct table_entry *table_entry_free(struct table_entry *np) {
    struct table_entry *prev = NULL;

    if (np) {
        prev = np->prev;
        if (np->name) xfree_str(np->name);
        if (np->defn) xfree_str(np->defn);
        xfree(np, sizeof(*np));
    }

    return prev;
}

void table_clear() {
    for (size_t i = 0; i < HASH_SIZE; ++i) {
        if (hash_table[i] == NULL) continue;

        struct table_entry *p = hash_table[i];
        while (p) {
            p = table_entry_free(p);
        }

        hash_table[i] = NULL;
    }
}

struct table_entry *table_lookup(char *name) {
    struct table_entry *np = hash_table[hash_function(name)];

    for (; np != NULL; np = np->prev) {
        if (strcmp(name, np->name) == 0) {
            return np;
        }
    }

    return np;
}

struct table_entry *table_insert(char *name, char *defn) {
    struct table_entry *np = NULL;

    if ((np = table_lookup(name)) == NULL) {
        // Not found. Add a new entry.

        if ((np = xmalloc(sizeof(*np))) == NULL) {
            goto error;
        }

        if ((np->name = xstrdup(name)) == NULL) {
            goto error;
        }

        hash_value_t hash_value = hash_function(name);
        np->prev = hash_table[hash_value];
        hash_table[hash_value] = np;
    } else {
        // Found. Free the value because we are going to overwrite it.
        xfree_str(np->defn);
    }

    if ((np->defn = xstrdup(defn)) == NULL) {
        goto error;
    }

    return np;
error:
    table_entry_free(np);
    return NULL;
}
