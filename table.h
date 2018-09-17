#if !defined(TABLE_H)
#define TABLE_H

#include <stdint.h>

struct table_entry {
    struct table_entry *prev;
    char *name;
    char *defn;
};

void table_free();
struct table_entry *table_lookup(char *name);
struct table_entry *table_insert(char *name, char *defn);

typedef uint32_t hash_value_t;
typedef hash_value_t (*hash_function_t)(const char *s);

hash_value_t table_hash_function(const char *s);
hash_function_t table_set_hash_function(hash_function_t f);

#endif // TABLE_H
