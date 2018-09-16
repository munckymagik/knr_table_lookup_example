#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#define HASH_SIZE 1

struct nlist {
    char *name;
    char *defn;
};

static struct nlist *hash_table[HASH_SIZE];

struct nlist *hash_lookup(char *name) {
    struct nlist *np = hash_table[0];

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

        if ((np = malloc(sizeof(*np))) == NULL) {
            goto error;
        }

        if ((np->name = strdup(name)) == NULL) {
            goto error;
        }

        assert(hash_table[0] == NULL && "Table already had an entry");
        hash_table[0] = np;
    } else {
        // Found. Free the value because we are going to overwrite it.
        free(np->defn);
    }

    if ((np->defn = strdup(defn)) == NULL) {
        goto error;
    }

    return np;
error:
    if (np) {
        if (np->name) free(np->name);
        free(np);
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

    puts("OK.");

    return 0;
}


