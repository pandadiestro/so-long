#include <netutils.h>
#include <log.h>



/*
 * Instead of using the per-file-descriptor raw entries
 * inside /proc/{pid}/fd, I could parse those from
 * /proc/{pid}/fdinfo into hashmaps, this because they
 * contain more "structured" information about the
 * processes, e.g.:
 *
 * $ cat fdinfo/4
     pos:    0
     flags:  02
     mnt_id: 9
     ino:    52706
     scm_fds: 0
 *
 * However, this requires Linux 2.6.22+, so I will stick
 * to the "traditional approach" of reading all links
 * at /proc/{pid}/fd in search of one of the form:
 * *socket:[inode]*, e.g.:
 *
 * $ readlink /proc/13098/fd/4
     socket:[2743]
 *
 * Read: (https://www.man7.org/linux/man-pages/man5/proc.5.html)
*/

typedef struct {
    char* str;
    size_t len;
} LineStr;

typedef union {
    LineStr* FSDStr;
    intmax_t FSDInt;
} FSDInfoValue;

typedef struct {
    LineStr* keys;
    size_t keysCapacity;
    size_t keysSize;
    FSDInfoValue* bucket;
    size_t bucketCapacity;
    size_t bucketSize;
} HashMap;

Err
newMap(HashMap* map)
{
    *map = (HashMap) {
        .bucket = calloc(4, sizeof(FSDInfoValue)),
        .bucketCapacity = 4,
        .bucketSize = 0,
        .keys = calloc(4, sizeof(LineStr)),
        .keysCapacity = 4,
        .keysSize = 0,
    };

    if (map->bucket == NULL || map->keys == NULL)
        return HEAP_MALFUNC;

    return NO_ERR;
}

void
freeMap(HashMap* map)
{
    free(map->keys);
    free(map->bucket);
    return;
}

char
isKeyInMap(const HashMap* map, LineStr* key)
{
    if (map == NULL || key == NULL)
        return -1;

    for (size_t i = 0; i < map->keysSize; i++) {
        if (key->len != map->keys[i].len)
            continue;

        if (memcmp(key->str, map->keys[i].str, key->len) == 0)
            return 1;
    }

    return 0;
}

size_t
getIndexOf(const HashMap* map, const LineStr* key)
{
    size_t carry = 0;
    for (size_t i = 0; i < key->len; i++) {
        if (i == 0) {
            carry = key->str[i];
            continue;
        }

        carry ^= key->str[i];
    }

    return (carry)%(map->bucketCapacity);
}

Err
get(const HashMap* map, const LineStr* key, FSDInfoValue* buffer)
{
    if (map == NULL || key == NULL)
        return INVALID_PARAM;

    if (map->bucketSize == 0 || map->keysSize == 0)
        return NO_ERR;

    size_t index = getIndexOf(map, key);
    *buffer = map->bucket[index];
    return NO_ERR;
}

Err
set(const HashMap* map, const LineStr* key, const FSDInfoValue* buffer)
{
    if (map == NULL || key == NULL || buffer == NULL)
        return INVALID_PARAM;

    if (map->bucketSize == 0 || map->keysSize == 0)
        return NO_ERR;

    if (map->bucketCapacity < map->bucketSize + 1)
        realloc(map->bucket, map->bucketCapacity * 2);

    return NO_ERR;
}






