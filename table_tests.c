#include <stdio.h>
#include <string.h>

#include "table.h"
#include "test.h"
#include "xalloc.h"

static hash_value_t always_0_hash_function(const char *_s) {
    (void) _s; // unused

    return 0;
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
        assert(table_hash_function("") == 0 && "Hash isn't 0 for the empty string");
        assert(table_hash_function("\1") == 1 && "Hash isn't 1 for '\1'");
        assert(table_hash_function("\1\1") == 32 && "Hash isn't 32 for '\1\1'");
        assert(table_hash_function("\1\1\1") == 84 && "Hash isn't 84 for '\1\1\1'");
        assert(table_hash_function("\1\1\1") == 84 && "Hash isn't deterministic for '\1\1\1'");
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
        hash_function_t old_hash_function = table_set_hash_function(always_0_hash_function);

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
        table_set_hash_function(old_hash_function);
    });

    table_free();

    struct xalloc_stats stats = xalloc_get_stats();
    printf("\nLeaked: %d bytes of total %d allocated\n", stats.count, stats.total);
    assert(stats.count == 0 && "memory leak detected");

    puts("\nOK.");

    return 0;
}
