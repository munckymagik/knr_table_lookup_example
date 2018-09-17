#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "xalloc.h"

#define HASH_SIZE 101

/*
 * This is the global singleton hash table.
 */
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

struct table_entry *table_entry_free(struct table_entry *p_entry) {
    struct table_entry *p_prev = NULL;

    if (p_entry) {
        p_prev = p_entry->prev;
        if (p_entry->name) xfree_str(p_entry->name);
        if (p_entry->defn) xfree_str(p_entry->defn);
        xfree(p_entry, sizeof(*p_entry));
    }

    return p_prev;
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
    struct table_entry *p_entry = hash_table[hash_function(name)];

    for (; p_entry != NULL; p_entry = p_entry->prev) {
        if (strcmp(name, p_entry->name) == 0) {
            return p_entry;
        }
    }

    return p_entry;
}

struct table_entry *table_insert(char *name, char *defn) {
    struct table_entry *p_entry = NULL;

    if ((p_entry = table_lookup(name)) == NULL) {
        // Not found. Add a new entry.

        if ((p_entry = xmalloc(sizeof(*p_entry))) == NULL) {
            goto error;
        }

        if ((p_entry->name = xstrdup(name)) == NULL) {
            goto error;
        }

        hash_value_t hash_value = hash_function(name);
        p_entry->prev = hash_table[hash_value];
        hash_table[hash_value] = p_entry;
    } else {
        // Found. Free the value because we are going to overwrite it.
        xfree_str(p_entry->defn);
    }

    if ((p_entry->defn = xstrdup(defn)) == NULL) {
        goto error;
    }

    return p_entry;
error:
    table_entry_free(p_entry);
    return NULL;
}
