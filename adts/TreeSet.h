/**
 * Set ADT using an unbalanced binary tree 
 *    NOTE that operations are only O(log(n)) if inserts/removals are random
 *         a better implementation would rebalance the tree, making them always O(log(n))
 * (c) Marco Antonio Gómez Martín & Miguel Gómez-Zamalloa, 2018
 * Adapted by Ignacio Fábregas, 2022
 * Modified & transaltad by Manuel Freire, 2023
 */
#ifndef __TREESET_H
#define __TREESET_H

#include "Exceptions.h"
#include "Stack.h" // Used for iteration

/**
 * Operations are:
 *    - TreeSet: constructor
 *    - insert(elem): mutator, adds an element. Does nothing if element was already present.
 *    - erase(elem): mutator, removes an element
 *    - contains(elem): observer, true IFF element already in set
 *    - empty(): observer, true IFF no elements in set
 */
template <class T>
class TreeSet {
private:
    /**
     Clase nodo que almacena internamente el dato
     y los punteros al hijo izquierdo y al hijo derecho.
     */
    class Node {
    public:
        Node() : _left(nullptr), _right(nullptr) {}
        Node(const T &elem)
            : _elem(elem), _left(nullptr), _right(nullptr) {}
        Node(Node *left, const T &elem, Node *right)
            : _elem(elem), _left(left), _right(right) {}

        T _elem;
        Node *_left;
        Node *_right;
    };

public:

    /** Constructor; returns an empty TreeSet */
    TreeSet() : _root(nullptr) {}

    /** Destructor; frees nodes in O(n) */
    ~TreeSet() {
        free();
        _root = nullptr;
    }

    /** 
     * Adds a new element to the set.  
     * No effect if element already present.
     * generator, O(log n) 
     */
    void insert(const T &elem) {
        _root = insertAux(elem, _root);
    }

    /**
     * Removes an element from the set. 
     * No effect if element not there in the first place. 
     * mutator, O(log n)
     */
    void erase(const T &elem) {
        _root = eraseAux(_root, elem);
    }

    /** 
     * Returns true IFF element in set
     * observer, O(log n)
     */
    bool contains(const T &elem) const {
        return (findAux(_root, elem) != nullptr) ? true : false;
    }

    /** 
     * Returns true IFF no elements in set
     * observer, O(1)
     */
    bool empty() const {
        return _root == nullptr;
    }

    /** 
     * Pretty-printing of tree. Only for debugging. 
     * observer, O(n)
     */
    friend std::ostream& operator<<(std::ostream& o, const TreeSet<T>& t){
        o  << "==== Tree =====" << std::endl;
        graph_rec(o, 0, t._root);
        o << "===============" << std::endl;
        return o;
    }

    // //
    // NON-CONST ITERATOR
    // //

    /**
     * An iterator that allows walking through the whole tree. 
     * Does not allow changing structure
     */
    class Iterator {
    public:
        Iterator() : _current(nullptr) {}

        /** O(log n) */
        void next() {
            if (_current == nullptr)
                throw InvalidAccessException();
            // If _Si hay hijo derecho, saltamos al primero en inorden del hijo derecho
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

        const T &elem() const {
            if (_current == nullptr)
                throw InvalidAccessException();
            return _current->_elem;
        }

        const T& operator*() const {
            return elem();
        }

        bool operator==(const Iterator &other) const {
            return _current == other._current;
        }

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
        friend class TreeSet;

        Iterator(Node *act) {
            this->_current = firstInOrder(act);
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
     * Returns an iterator just outside the set, reachable by an iterator 
     * that starts at begin()
     * O(1) 
     */
    Iterator end() const {
        return Iterator(nullptr);
    }

    /**
    * Returns an iterator to the node with element e,
    * or end() if not found
    * O(log n)
    */
    Iterator find(const T &e) {
        Stack<Node*> ancestors;
        Node *p = _root;
        while ((p != nullptr) && (p->_elem != e)) {
            if (p->_elem > e) {
                ancestors.push(p);
                p = p->_left;
            } else
                p = p->_right;
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
     * An iterator that allows walking through the whole tree. 
     * Does not allow changing structure
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
        const T &elem() const {
            if (_current == nullptr)
                throw InvalidAccessException();
            return _current->_elem;
        }

        /** O(1) */
        const T& operator*() const {
            return elem();
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
        friend class TreeSet;

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
     * Returns a constant iterator starting from the smallest element.
     * O(log n)
     */
    ConstIterator cbegin() const {
        return ConstIterator(_root);
    }

    /** 
     * Returns a constant iterator just outside the set, reachable by an iterator 
     * that starts at cbegin()
     * O(1) 
     */    
    ConstIterator cend() const {
        return ConstIterator(nullptr);
    }

    /**
     * Returns a constant iterator to the node with element e,
     * or cend() if not found
     * O(log n)
     */
    ConstIterator find(const T &e) const {
        Stack<Node*> ancestors;
        Node *p = _root;
        while ((p != nullptr) && (p->_elem != e)) {
            if (p->_elem > e) {
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
    // BOILERPLATE C++ CODE
    // //

    /** Copy ctor. O(n) */
    TreeSet(const TreeSet<T> &other) : _root(nullptr) {
        copy(other);
    }

    /** Copy assignment operator. O(n) */
    TreeSet<T> &operator=(const TreeSet<T> &other) {
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

    void copy(const TreeSet &other) {
        _root = copyAux(other._root);
    }

private:
    /** used to generate output */
    static const int TREE_INDENTATION = 4;

    /**
     * Deletes the structure recursively.
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
     * O(n)
     */
    static Node* copyAux(Node *root) {
        if (root == nullptr)
            return nullptr;
        return new Node(copyAux(root->_left), root->_elem, copyAux(root->_right));
    }

    /**
     * Inserts an element into the structure, either at or below p.
     * p may be nullptr, in which case a root node will be created & returned.
     * otherwise, returns p.
     * O(log n)
     */
    static Node* insertAux(const T &elem, Node *p) {
        if (p == nullptr) {
            return new Node(elem);
        } else if (p->_elem == elem) {
            return p;
        } else if (elem < p->_elem) {
            p->_left = insertAux(elem, p->_left);
            return p;
        } else { // (elem > p->elem)
            p->_right = insertAux(elem, p->_right);
            return p;
        }
    }

    /**
     * Finds an element in the structure
     * Returns a pointer to the element, or nullptr if not found
     * O(log n)
     */
    static Node* findAux(Node *p, const T &elem) {
        if (p == nullptr) {
            return nullptr;
        } else if (p->_elem == elem) {
            return p;
        } else if (elem < p->_elem) {
            return findAux(p->_left, elem);
        } else {
            return findAux(p->_right, elem);
        }
    }

    /**
     * Removes (if found) an element at or below p
     * If element at p, a modified (but valid) pointer will be returned 
     * otherwise returns p.
     * O(log n)
    */
    static Node* eraseAux(Node *p, const T &elem) {
        if (p == nullptr)
            return nullptr;
        if (elem == p->_elem) {
            return eraseRoot(p);
        } else if (elem < p->_elem) {
            p->_left = eraseAux(p->_left, elem);
            return p;
        } else { // elem > p->elem
            p->_right = eraseAux(p->_right, elem);
            return p;
        }
    }

    /**
     * Removes the root of a structure, and returns a pointer to the new root.
     * O(log n) (only if a smallest element needs to be found)
     */
    static Node* eraseRoot(Node *p) {
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
    static Node *moveSmallestAndErase(Node *p) {
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


    static void outputIndented(std::ostream & out, int indent, Node* root){
        if (root != nullptr) {
            outputIndented(out, indent + TREE_INDENTATION, root->_right);
            out << std::setw(indent) << " " << root->_elem << std::endl;
            outputIndented(out, indent + TREE_INDENTATION, root->_left);
        }
    }

    /**
     * Root node
     */
    Node *_root;
};

#endif // __TREESET_H

