#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "xalloc.h"

#define HASH_SIZE 101

struct table_entry {
    struct table_entry *prev;
    char *name;
    char *defn;
};

typedef uint32_t hash_value_t;
typedef hash_value_t (*hash_function_t)(const char *s);

static hash_value_t default_hash_function(const char *s) {
    hash_value_t hash_value = 0;

    while (*s != '\0') {
        hash_value = *s + 31 * hash_value;
        s++;
    }

    return hash_value % HASH_SIZE;
}

static hash_value_t always_0_hash_function(const char *_s) {
    (void) _s; // unused

    return 0;
}

static hash_function_t hash_function = default_hash_function;
static struct table_entry *hash_table[HASH_SIZE] = { 0 };

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

void table_free() {
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

int main() {
    TEST("Add an entry then retrieve it", {
        struct table_entry *entry = table_insert("X", "Y");

        assert(entry != NULL && "entry was NULL");
        assert(strcmp(entry->name, "X") == 0 && "entry->name was not X");
        assert(strcmp(entry->defn, "Y") == 0 && "entry->defn was not Y");

        struct table_entry *found = table_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");
    });

    TEST("Overwrite the entry", {
        table_insert("X", "Y");
        struct table_entry *found = table_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");

        table_insert("X", "Z");
        found = table_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Z") == 0 && "found->defn was not Z");
    });

    TEST("The hash function", {
        assert(hash_function("") == 0 && "Hash isn't 0 for the empty string");
        assert(hash_function("\1") == 1 && "Hash isn't 1 for '\1'");
        assert(hash_function("\1\1") == 32 && "Hash isn't 32 for '\1\1'");
        assert(hash_function("\1\1\1") == 84 && "Hash isn't 84 for '\1\1\1'");
        assert(hash_function("\1\1\1") == 84 && "Hash isn't deterministic for '\1\1\1'");
    });

    TEST("Add more than one entry", {
        struct table_entry *found = NULL;

        table_insert("X", "Y");
        table_insert("Y", "Z");

        found = table_lookup("X");
        assert(found != NULL && "found for X was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");

        found = table_lookup("Y");
        assert(found != NULL && "found for Y was NULL");
        assert(strcmp(found->name, "Y") == 0 && "found->name was not Y");
        assert(strcmp(found->defn, "Z") == 0 && "found->defn was not Z");
    });

    TEST("Lookup non-existent", {
        assert(table_lookup("non-existent") == NULL && "non-existent, existed");
    });

    TEST("Stores entries in buckets", {
        // Override the default hash function to always return the same key
        hash_function = always_0_hash_function;

        struct table_entry *x = table_insert("X", "Y");
        struct table_entry *y = table_insert("Y", "Z");

        assert(x->prev == NULL && "x->prev was not NULL");
        assert(y->prev == x && "y->prev was not x");

        // Y will be the first in the bucket
        struct table_entry *yy = table_lookup("Y");
        assert(yy != NULL && "yy was NULL");

        // X will be a link in the chain
        struct table_entry *xx = table_lookup("X");
        assert(xx != NULL && "xx was NULL");

        // Make sure non-existent keys in the chain do not cause problems
        assert(table_lookup("non-existent") == NULL && "non-existent, existed in bucket");

        // Restore the default hash function
        hash_function = default_hash_function;
    });

    table_free();

    struct xalloc_stats stats = xalloc_get_stats();
    printf("\nLeaked: %d bytes of total %d allocated\n", stats.count, stats.total);
    assert(stats.count == 0 && "memory leak detected");

    puts("\nOK.");

    return 0;
}


