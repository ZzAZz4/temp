#ifndef FILES_HASH_FUNCTIONS_H
#define FILES_HASH_FUNCTIONS_H

#include "class_names.h"
#include <unordered_set>
#include <unordered_map>

template<class T>
constexpr inline
void
hash_combine(std::size_t &seed, const T &v);

namespace std
{
    template<typename T>
    struct hash<Point<T>>;
    template<typename T>
    struct hash<Shot_marker<T>>;
}

template<typename C>
struct hash_container;

//#include "../Definitions/Hash_functions.cpp"


#endif //FILES_HASH_FUNCTIONS_H
