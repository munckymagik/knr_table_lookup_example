#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "xalloc.h"

#define HASH_SIZE 101

struct nlist {
    char *name;
    char *defn;
};

static struct nlist *hash_table[HASH_SIZE] = { 0 };

typedef uint32_t hash_value_t;

hash_value_t hash_function(const char *s) {
    hash_value_t hash_value = 0;

    while (*s != '\0') {
        hash_value = *s + 31 * hash_value;
        s++;
    }

    return hash_value % HASH_SIZE;
}

struct nlist *hash_lookup(char *name) {
    struct nlist *np = hash_table[hash_function(name)];

    if (np == NULL) {
        return NULL;
    }

    if (strcmp(name, np->name) == 0) {
        return np;
    }

    return NULL;
}

struct nlist *hash_insert(char *name, char *defn) {
    struct nlist *np = NULL;

    if ((np = hash_lookup(name)) == NULL) {
        // Not found. Add a new entry.

        if ((np = xmalloc(sizeof(*np))) == NULL) {
            goto error;
        }

        if ((np->name = xstrdup(name)) == NULL) {
            goto error;
        }

        hash_value_t hash_value = hash_function(name);
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
    if (np) {
        if (np->name) xfree_str(np->name);
        xfree(np, sizeof(*np));
    }
    return NULL;
}

int main() {
    TEST("Add an entry then retrieve it", {
        struct nlist *entry = hash_insert("X", "Y");

        assert(entry != NULL && "entry was NULL");
        assert(strcmp(entry->name, "X") == 0 && "entry->name was not X");
        assert(strcmp(entry->defn, "Y") == 0 && "entry->defn was not Y");

        struct nlist *found = hash_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");
    });

    TEST("Overwrite the entry: ", {
        hash_insert("X", "Y");
        struct nlist *found = hash_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");

        hash_insert("X", "Z");
        found = hash_lookup("X");

        assert(found != NULL && "found was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Z") == 0 && "found->defn was not Z");
    });

    TEST("The hash function: ", {
        assert(hash_function("") == 0 && "Hash isn't 0 for the empty string");
        assert(hash_function("\1") == 1 && "Hash isn't 1 for '\1'");
        assert(hash_function("\1\1") == 32 && "Hash isn't 32 for '\1\1'");
        assert(hash_function("\1\1\1") == 84 && "Hash isn't 84 for '\1\1\1'");
        assert(hash_function("\1\1\1") == 84 && "Hash isn't deterministic for '\1\1\1'");
    });

    TEST("Add more than one entry: ", {
        struct nlist *found = NULL;

        hash_insert("X", "Y");
        hash_insert("Y", "Z");

        found = hash_lookup("X");
        assert(found != NULL && "found for X was NULL");
        assert(strcmp(found->name, "X") == 0 && "found->name was not X");
        assert(strcmp(found->defn, "Y") == 0 && "found->defn was not Y");

        found = hash_lookup("Y");
        assert(found != NULL && "found for Y was NULL");
        assert(strcmp(found->name, "Y") == 0 && "found->name was not Y");
        assert(strcmp(found->defn, "Z") == 0 && "found->defn was not Z");
    });

    puts("OK.");

    struct xalloc_stats stats = xalloc_get_stats();
    printf("Leaked: %d bytes of total %d allocated\n", stats.count, stats.total);

    return 0;
}


