/**
 * Example hash functions.
 * (c) Antonio Sánchez Ruiz-Granados, 2012
 * Modified by Ignacio Fábregas, 2022
 * Modified & translated by Manuel Freire, 2023
 */

#ifndef __HASH_H
#define __HASH_H

#include <string>

// ----------------------------------------------------
//
// Hash functions for basic types
//
// ----------------------------------------------------


inline unsigned int myhash(unsigned int key) {
    return key;
}

inline unsigned int myhash(int key) {
    return (unsigned int)key;
}

inline unsigned int hash(char key) {
    return key;
}

// Fowler/Noll/Vo (FNV) -- adapted from http://bretmulvey.com/hash/6.html 
inline unsigned int myhash(std::string key) {
    const unsigned int p = 16777619; // large prime
    unsigned int hash = 2166136261;  // initial value
    for (unsigned int i=0; i<key.size(); i++)
        hash = (hash ^ key[i]) * p; // ^ is a bit-wise xor
    // final mix
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

/**
 * Object-function to hash a given key type
 */
template<class C>
class Hash{
public:
    int operator()(const C& c)  const{
        return myhash(c);
    }

};


#endif // __HASH_H
