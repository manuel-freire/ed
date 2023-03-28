/**
 * Map ADT using binary trees
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * Modified & translated by Manuel Freire, 2023
 */

#ifndef __TREEMAP_H
#define __TREEMAP_H

#include <iostream>
#include "Exceptions.h"
#include "Stack.h"      // Used by iterators

/**
 * Map using an (unbalanced) Binary Tree 
 * 
 * Requires keys to support a comparison operator: a function object that accepts
 *    two values of the key type, and which can answer whether 1st < 2nd.
 *    The < operator is used if present for the key type T. 
 * Operations are:
 *    - TreeMap constructor: generator
 *    - insert(key, value): generator, adds a new (key, value) pair to the tree.
 *          If the key was already present, replaces its value with the new one.
 *    - erase(key): mutator. Removes the key from the tree. No effect if key absent.
 *    - at(key): observer. Returns value that corresponds to a key. 
 *          Partial: key must exist; use contains() first if unsure.
 *    - contains(key): observer. Returnes true iff key exists in map
 *    - empty(): observer. Returns true if no keys present.
 *    - size(): observer. Returns count of currently-contained keys.
 */

template <typename K, typename V, typename Comparator = std::less<K>>
class TreeMap {
private:
    /**
     * Internal node class
     */
    class Node {
    public:
        Node() : _left(nullptr), _right(nullptr) {}
        Node(const K &key, const V &value) 
            : _key(key), _value(value), _left(nullptr), _right(nullptr) {}
        Node(Node *left, const K &key, const V &value, Node *right)
            : _key(key), _value(value), _left(left), _right(right) {}

        K _key;
        V _value;
        Node* _left;
        Node* _right;
    };

public:

    /** Constructor; returns an empty TreeMap. O(1) */
    TreeMap() : _root(nullptr) { _size =0; }

    /** Destructor; frees nodes in O(n) */
    ~TreeMap() {
        free();
        _root = nullptr;
        _size = 0;
    }

    /** 
     * Adds a new key, value pair to the set. If key
     * already present, replaces old value with new one.  
     * No effect if element already present.
     * generator, O(log n) 
     */
    void insert(const K &key, const V &value) {
        _root = insertAux(key, value, _root);
        // note that insertAux increases _size accordingly
    }

    /**
     * Removes a key, value pair from the map. 
     * No effect if key not there in the first place. 
     * mutator, O(log n)
     */
    void erase(const K &key) {
        _root = eraseAux(_root, key);
        // note that eraseAux decreases _size accordingly
    }

    /**
     * Returns value associated to a key. 
     * Partial - if key not present, throws exception. Use contains() if unsure
     * observer, O(log n)
     */
    const V &at(const K &key) const {
        Node *p = findAux(_root, key);
        if (p == nullptr) {
            throw BadKeyException();
        }
        return p->_value;
    }

    /** 
     * Returns true IFF element in set
     * observer, O(log n)
     */
    bool contains(const K &key) const {
        return findAux(_root, key) != nullptr;
    }

    /** 
     * Returns true IFF no elements in set
     * observer, O(1)
     */
    bool empty() const {
        return _root == nullptr;
    }

    /** 
     * Returns number of keys in map. 
     * observer, O(1) 
     */
    int size() const{
        return _size;
    }

    /**
     * Overloads the [] operator, to access (and possibly modify) a value given its key.
     * If the key is not present, inserts a new (default) value for that key.
     */
    V &operator[](const K &key) {
        bool inserted;
        Node* ret = findOrInsert(_root, key, inserted); 
        if (inserted) { 
            ++_size;
        }
        return ret->_value;
    }

    /** 
     * Pretty-printing of map. Only for debugging. 
     * observer, O(n)
     */    
    friend std::ostream& operator<<(std::ostream& o, const TreeMap<K, V, Comparator>& t){
        o<<"{";
        show(t._root, o);
        o<<"}";
        return o;
    }

    // //
    // NON-CONST ITERATOR
    // //

    /**
     * An iterator that allows walking through the whole map. 
     * Allows changing values (but not keys)
     */
    class Iterator {
    public:
        Iterator() : _current(nullptr) {}

        /** O(log n) */
        void next() {
            if (_current == nullptr)
                throw InvalidAccessException();
            // If right child, jump its smallest child (first in order)
            if (_current->_right != nullptr)
                _current = firstInOrder(_current->_right);
            else {
                // Otherwise, we backtrack to the first unvisited ancestor
                if (_ancestors.empty()) // Reached root!
                    _current = nullptr;
                else {
                    _current = _ancestors.top();
                    _ancestors.pop();
                }
            }
        }

        /** O(1) */
        const K &key() const {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_key;
        }

        /** O(1) */
        V &value() {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_value;
        }

        /** O(1) */
        bool operator==(const Iterator &other) const {
            return _current == other._current;
        }

        /** O(1) */
        bool operator!=(const Iterator &other) const {
            return !(this->operator==(other));
        }

        /** O(log n) */
        Iterator &operator++() {
            next();
            return *this;
        }

        /** O(log n) */
        Iterator operator++(int) {
            Iterator ret(*this);
            operator++();
            return ret;
        }

    protected:
        friend class TreeMap;

        Iterator(Node *current) {
            this->_current = firstInOrder(current);
        }

        /**
         * Returns the 1st element in an in-order search of the node structure.
         * Keeps a stack of ancestors to allow backtracking when needed
         * O(log n)
         */
        Node *firstInOrder(Node *p) {
            if (p == nullptr)
                return nullptr;

            while (p->_left != nullptr) {
                _ancestors.push(p);
                p = p->_left;
            }
            return p;
        }

        /** Pointer to current node in traversal */
        Node *_current;

        /** Non-visited ascendants, for use when backtracking up the tree */
        Stack<Node*> _ancestors;
    };

    /**
     * Returns an iterator starting from the smallest element.
     * O(log n)
     */
    Iterator begin() {
        return Iterator(_root);
    }

    /** 
     * Returns an iterator just outside the map, reachable by an iterator 
     * that starts at begin()
     * O(1) 
     */
    Iterator end() const {
        return Iterator(nullptr);
    }

    /**
     * Returns an iterator to the node with a given key,
     * or end() if not found
     * O(log n)
     */
    Iterator find(const K &key) {
        Stack<Node*> ancestors;
        Node *p = _root;
        while (p != nullptr && (_cless(p->_key, key) || _cless(key, p->_key))) {
            if (_cless(key, p->_key)) {
                ancestors.push(p);
                p = p->_left;
            } else {
                p = p->_right;
            }
        }
        Iterator ret;
        ret._current = p;
        if (p != nullptr)
            ret._ancestors = ancestors;
        return ret;
    }


    // //
    // CONSTANT ITERATOR
    // //

    /**
     * An iterator that allows walking through the whole map. 
     * Does not allow any changes
     */
    class ConstIterator {
    public:
        ConstIterator() : _current(nullptr) {}

        /** Converts an Iterator to a ConstIterator  */
        ConstIterator(const Iterator& it){
            this->_current = it._current;
            this->_ancestors = it._ancestors;
        }

        /** O(log n) */
        void next() {
            if (_current == nullptr)
                throw InvalidAccessException();
            // If right child, jump its smallest child (first in order)
            if (_current->_right != nullptr)
                _current = firstInOrder(_current->_right);
            else {
                // Otherwise, we backtrack to the first unvisited ancestor
                if (_ancestors.empty()) // Reached root!
                    _current = nullptr;
                else {
                    _current = _ancestors.top();
                    _ancestors.pop();
                }
            }
        }

        /** O(1) */
        const K &key() const {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_key;
        }

        /** O(1) */
        const V &value() const {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_value;
        }

        /** O(1) */
        bool operator==(const ConstIterator &other) const {
            return _current == other._current;
        }

        /** O(1) */
        bool operator!=(const ConstIterator &other) const {
            return !(this->operator==(other));
        }

        /** O(log n) */
        ConstIterator &operator++() {
            next();
            return *this;
        }

        /** O(log n) */
        ConstIterator operator++(int) {
            ConstIterator ret(*this);
            operator++();
            return ret;
        }

    protected:
        friend class TreeMap;

        ConstIterator(Node *current) {
            this->_current = firstInOrder(current);
        }

        /**
         * Returns the 1st element in an in-order search of the node structure.
         * Keeps a stack of ancestors to allow backtracking when needed
         * O(log n)
         */
        Node *firstInOrder(Node *p) {
            if (p == nullptr)
                return nullptr;

            while (p->_left != nullptr) {
                _ancestors.push(p);
                p = p->_left;
            }
            return p;
        }

        /** Pointer to current node in traversal */
        Node *_current;

        /** Non-visited ascendants, for use when backtracking up the tree */
        Stack<Node*> _ancestors;
    };

    /**
     * Returns a constant iterator starting from the smallest key
     * O(log n)
     */
    ConstIterator cbegin() const {
        return ConstIterator(_root);
    }

    /** 
     * Returns a constant iterator just outside the map, reachable by an iterator 
     * that starts at cbegin()
     * O(1) 
     */    
    ConstIterator cend() const {
        return ConstIterator(nullptr);
    }

    /**
     * Returns a constant iterator to the node with a given key,
     * or cend() if not found
     * O(log n)
     */
    ConstIterator find(const K &key) const {
        Stack<Node*> ancestors;
        Node *p = _root;
        while (p != nullptr && (_cless(p->_key, key) || _cless(key, p->_key))) {
            if (_cless(key, p->_key)) {
                ancestors.push(p);
                p = p->_left;
            } else {
                p = p->_right;
            }
        }
        ConstIterator ret;
        ret._current = p;
        if (p != nullptr)
            ret._ancestors = ancestors;
        return ret;
    }



    // //
    // C++ Boilerplate code to make class more useful
    // //

    /** Copy ctor. O(n) */
    TreeMap(const TreeMap<K, V, Comparator> &other) : _root(nullptr) {
        copy(other);
    }


    /** Copy assignment operator. O(n) */
    TreeMap<K, V, Comparator> &operator=(const TreeMap<K, V, Comparator> &other) {
        if (this != &other) {
            free();
            copy(other);
        }
        return *this;
    }

protected:

    void free() {
        free(_root);
    }

    void copy(const TreeMap &other) {
        _root = copyAux(other._root);
        _size = other._size;
        _cless = other._cless;
    }

private:

    /**
     * Removes all nodes from a tree structure that starts at n.
     * O(n)
     */
    static void free(Node *n) {
        if (n != nullptr) {
            free(n->_left);
            free(n->_right);
            delete n;
        }
    }


    /**
     * Copies the structure recursively.
     */
    static Node* copyAux(Node *root) {
        if (root == nullptr)
            return nullptr;
        return new Node(copyAux(root->_left), root->_key, root->_value, 
                        copyAux(root->_right));
    }

    /**
     * Inserts a key-value pair into the structure, either at or below p.
     * p may be nullptr, in which case a root node will be created & returned.
     * otherwise, returns p.
     * O(log n)
     */
    Node *insertAux(const K &key, const V &value, Node *p) {
        if (p == nullptr) {
            _size ++;
            return new Node(key, value);
        } else if (_cless(key, p->_key)) { // key < root->_key
            p->_left = insertAux(key, value, p->_left);
            return p;
        } else if (_cless(p->_key, key)) { // key < root->_key
            p->_right = insertAux(key, value, p->_right);
            return p;
        } else { // key == p->key
            return p;
        }
    }

    /**
     * Finds an element in the structure
     * Returns a pointer to the element, or nullptr if not found
     * O(log n)
     */
    Node *findAux(Node *p, const K &key) const {
        if (p == nullptr) {
            return nullptr;
        } else if (_cless(key, p->_key)) { // key < root->_key
            return findAux(p->_left, key);
        } else if (_cless(p->_key, key)) { // key < root->_key
            return findAux(p->_right, key);
        } else { // key == p->key
            return p;
        }
    }


    /**
     * Finds or inserts a key in the map. 
     * If the key is new, inserts a default value.
     * If the key was already present, nothing is inserted.
     * Returns a node with the key (a new one if absent, the old one otherwise)
     * Used in operator[]. Sets inserted to true iff new node returned
     * O(log n)
     */
    Node *findOrInsert(Node* &root, const K &key, bool& inserted) {
        if (root == nullptr) { // Key not present: return new Node*
            inserted = true;
            root = new Node(key, V()); // adds new node to tree
            return root; 
        } else if (_cless(key, root->_key)) { // key < root->_key
            return findOrInsert(root->_left, key, inserted);
        } else if (_cless(root->_key, key)) { // key < root->_key
            return findOrInsert(root->_right, key, inserted);
        } else {              // Key found: do not modify it
            inserted = false;
            return root;      
        }
    }

    /**
     * Removes (if found) an element at or below p
     * If element at p, a modified (but valid) pointer will be returned 
     * otherwise returns p.
     * O(log n)
     */
    Node* eraseAux(Node *p, const K &key) {
        if (p == nullptr) {
            return nullptr;
        } else if (_cless(key, p->_key)) { // key < p->key
            p->_left = eraseAux(p->_left, key);
            return p;
        } else if(_cless(p->_key, key)) { // (key > p->key)
            p->_right = eraseAux(p->_right, key);
            return p;
        } else {  // p->key == key
            _size--;
            return eraseRoot(p);
        }
    }

    /**
     * Removes the root of a structure, and returns a pointer to the new root.
     * O(log n) (only if a smallest element needs to be found)
     */
    Node* eraseRoot(Node *p) {
        Node *aux;
        if (p->_left == nullptr) {// Si no hay hijo izquierdo, la raíz pasa a ser el hijo derecho
            aux = p->_right;
            delete p;
            return aux;
        } else
        if (p->_right == nullptr) {// Si no hay hijo derecho, la raíz pasa a ser el hijo izquierdo
            aux = p->_left;
            delete p;
            return aux;
        } else { // both right and left nodes
                 // smallest element will be moved to right subtree
            return moveSmallestAndErase(p);
        }
    }

    /**
     * Finds smallest element in right subtree, which will be used as new root.
     *   - Smallest will be new right child
     *   - Left child of old smallest's parent will be the new left child
     * O(log n)
     */
    Node *moveSmallestAndErase(Node *p) {
        /*
         * Find smallest & its immediate parent
         */
        Node *parent = nullptr;
        Node *aux = p->_right;
        while (aux->_left != nullptr) {
            parent = aux;
            aux = aux->_left;
        }
        if (parent != nullptr) { // parent found, switch
            parent->_left = aux->_right;
            aux->_left = p->_left;
            aux->_right = p->_right;
        } else { // no parent - was already root
            aux->_left = p->_left;
        }

        delete p;
        return aux;
    }

    /**
     * Output. Used only for debugging
     */
    static void show(Node *n, std::ostream &out) {
        if (n != nullptr) {
            if (n->_left != nullptr) {
                show(n->_left, out);
                out << ", ";
            }
            out << n->_key << " -> " << n->_value;
            if (n->_right != nullptr) {
                out << ", ";
                show(n->_right, out);
            }
        }
    }

    /**
     * Root node
     */
    Node *_root;

    /**
     * Comparator
     */
    Comparator _cless;

    /** 
     * Number of elements in the set
     */
    int _size;
};

#endif // __TREEMAP_H
