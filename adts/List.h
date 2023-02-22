/**
 * Implementation of the List ADT, with a doubly-linked list
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * English & more by Manuel Freire, 2023
 */
#ifndef __LIST_H
#define __LIST_H

#include "Exceptions.h"
#include <cassert>

/**
 * Implementation of the List ADT, with a doubly-linked list
 * Operations:
 *    - EmptyList: -> List. Generator (as empty constructor)
 *    - push_front: List, Elem -> List. Generator.
 *    - push_back: List, Elem -> List. Mutator.
 *    - front: List - -> Elem. Observer, partial
 *    - pop_front: List - -> List. Mutator, partial
 *    - back: List - -> Elem. Observer, partial
 *    - pop_back: List - -> List. Mutator, partial
 *    - empty: List -> Bool. Observer
 *    - size: List -> Int. Observer
 *    - at: List, Int - -> Elem. Observer, partial.
 */
template <class T>
class List {
private:
    /**
     * Node class. Stores the element (of type T), and pointers to previous and next nodes
     * Both pointers may be nullptr if this is 1st and/or last node of list.
     */
    class Node {
    public:
        Node() : _next(nullptr), _prev(nullptr) {}
        Node(const T &_elem) : _elem(_elem), _next(nullptr), _prev(nullptr) {}
        Node(Node *_prev, const T &_elem, Node *_next) : _elem(_elem), _next(_next), _prev(_prev) {}

        T _elem;
        Node *_next;
        Node *_prev;
    };

public:

    /** Constructor; EmptyList. O(1) */
    List() : _first(nullptr), _last(nullptr), _size(0) {}

    /** Destructor; frees list. O(n) */
    ~List() {
        free();
    }

    /** Adds an an element at the front. O(1) */
    void push_front(const T &_elem) {
        _first = insertElem(_elem, nullptr, _first);
        if (_last == nullptr) {
            _last = _first;    // list was empty; last is also 1st element
        }
    }

    /** Adds an element at the back. O(1) */
    void push_back(const T &_elem) {
        _last = insertElem(_elem, _last, nullptr);
        if (_first == nullptr) {
            _first = _last;    // list was empty; 1st is also last element
        }
    }

    /**
     * Returns 1st (front) element. It is an error to do this on an empty list.
     * O(1)
     */
    const T &front() const {
        if (empty())
            throw EmptyListException("Cannot get front. The list is empty.");
        return _first->_elem;
    }

    /**
     * Returns last (back) element. It is an error to do this on an empty list.
     * O(1)
     */
    const T &back() const {
        if (empty())
            throw EmptyListException("Cannot get back. The list is empty.");
        return _last->_elem;
    }

    /**
     * Removes 1st element. It is an error to do this on an empty list.
     * O(1)
     */
    void pop_front() {
        if (empty())
            throw EmptyListException("Cannot pop. The list is empty.");
        Node *aBorrar = _first;
        _first = _first->_next;
        deleteElem(aBorrar);
        if (_first == nullptr) // if empty, must also update last
            _last = nullptr;
    }

    /**
     * Removes last element. It is an error to do this on an empty list.
     * O(1)
     */
    void pop_back() {
        if (empty())
            throw EmptyListException("Cannot pop. The list is empty.");
        Node *aBorrar = _last;
        _last = _last->_prev;
        deleteElem(aBorrar);
        if (_last == nullptr) //si la lista queda vacía, modificamos también _first
            _first = nullptr;
    }

    /** Operación Observer para saber si una lista tiene o no elementos. O(1) */
    bool empty() const {
        return _first == nullptr;
    }

    /** Devuelve el número de elementos que hay en la lista. O(1) */
    unsigned int size() const {
        return _size;
    }

    /**
     * Devuelve el elemento i-ésimo de la lista, teniendo en cuenta que el primer elemento (first())
     * es el elemento 0 y el último es size()-1, es decir idx está en [0..size()-1].
     * Operación Observer partial que puede fallar si se da un índice incorrecto.
     * O(n)
    */
    const T &at(unsigned int idx) const {
        if (idx >= _size)
            throw InvalidAccessException("Cannot get specified element. Invalid index");
        Node *aux = _first;
        for (int i = 0; i < idx; ++i)
            aux = aux->_next;
        return aux->_elem;
    }

    // //
    // ITERATORS
    // //

    /**
     * Clase interna que implementa un iterador sobre la lista que permite recorrer la lista pero no
     * permite cambiarlos.
     */
    class ConstIterator {
    public:
        ConstIterator() : _current(nullptr) {}

        void next() {
            if (_current == nullptr) throw InvalidAccessException();
            _current = _current->_next;
        }

        const T &_elem() const {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_elem;
        }

        bool operator==(const ConstIterator &other) const {
            return _current == other._current;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(this->operator==(other));
        }

        const T& operator*() const {
            return _elem();
        }

        ConstIterator &operator++() {
            next();
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator ret(*this);
            operator++();
            return ret;
        }

    protected:
        /** Para que pueda construir objetos del tipo iterador */
        friend class List;

        ConstIterator(Node *_current) : _current(_current) {}

        /** Puntero al nodo actual del recorrido */
        Node *_current;
    };

    /**
     * Clase interna que implementa un iterador sobre la lista que permite recorrer la lista e incluso
     * alterar el valor de sus elementos.
     */
    class Iterator {
    public:
        Iterator() : _current(nullptr) {}

        void next() {
            if (_current == nullptr) throw InvalidAccessException();
            _current = _current->_next;
        }

        const T &_elem() const {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_elem;
        }

        void set(const T &_elem) const {
            if (_current == nullptr) throw InvalidAccessException();
            _current->_elem = _elem;
        }

        bool operator==(const Iterator &other) const {
            return _current == other._current;
        }

        bool operator!=(const Iterator &other) const {
            return !(this->operator==(other));
        }

        const T& operator*() const {
            return _elem();
        }

        /** same as _elem(), but non-const */
        T& operator*() {
            if (_current == nullptr) throw InvalidAccessException();
            return _current->_elem;
        }

        Iterator &operator++() {
            next();
            return *this;
        }

        Iterator operator++(int) {
            Iterator ret(*this);
            operator++();
            return ret;
        }

    protected:
        /** Allows lists to create iterators */
        friend class List;

        Iterator(Node *_current) : _current(_current) {}

        /** Current node being iterated */
        Node *_current;
    };

    // //
    // OPERATORS ON ITERATORS
    // //


    /** Const iterator, starting at 1st element. O(1) */
    ConstIterator cbegin() const {
        return ConstIterator(_first);
    }

    /** Const iterator at after-last element. O(1) */
    ConstIterator cend() const {
        return ConstIterator(nullptr);
    }

    /** Non-const iterator, starting at 1st element. O(1) */
    Iterator begin() {
        return Iterator(_first);
    }

    /** Non-const iterator at after-last element. O(1) */
    Iterator end() const {
        return Iterator(nullptr);
    }

    /**
     * Removes element at current position. 
     * The passed-in iterator is NO LONGER VALID. Use the returned iterator instead, 
     * which will point to the next element in the list (the one that was next to the removed one).
     * @return New iterator pointing to the element that was after the removed one
     * O(1).
     */
    Iterator erase(const Iterator &it) {
        if (it._current == nullptr)
            throw InvalidAccessException("Cannot erase specified element. Iterator pointing to nullptr");
        if (it._current == _first) {
            // Special case: 1st element
            pop_front();
            return Iterator(_first);
        } else if (it._current == _last) {
            // Special case: last element
            pop_back();
            return Iterator(nullptr);
        } else {
            // Not a special case
            Node *_next = it._current->_next;
            deleteElem(it._current);
            return Iterator(_next);
        }
    }

    /**
     * Inserts at current location.
     * To be precice, inserts _just before_ current element. For example, give it = l.begin(),
     * the inserted element will now be the new l.begin() (but iterator will point to now-2nd-element).
     * O(1).
     */
    void insert(const T &_elem, const Iterator &it) {
        if (_first == it._current) {
            // Special case: insert at start
            push_front(_elem);
        } else
        if (it._current == nullptr) {
            // Special case: insert at end
            push_back(_elem);
        } else {
            // Normal case
            insertElem(_elem, it._current->_prev, it._current);
        }
    }

    // //
    // C++ Boilerplate code to make class more useful
    // //

    /** Copy ctor. O(n) */
    List(const List<T> &other) : _first(nullptr), _last(nullptr) {
        copy(other);
    }

    /** Assignment op. O(n) */
    List<T> &operator=(const List<T> &other) {
        if (this != &other) {
            free();
            copy(other);
        }
        return *this;
    }

    /** Equality op. O(n) */
    bool operator==(const List<T> &rhs) const {
        if (_size != rhs._size) {
            return false;
        }
        bool same = true;
        Node *p1 = _first;
        Node *p2 = rhs._first;
        while ((p1 != nullptr) && (p2 != nullptr) && same) {
            if (p1->_elem != p2->_elem)
                same = false;
            else {
                p1 = p1->_next;
                p2 = p2->_next;
            }
        }
        return same;
    }

    bool operator!=(const List<T> &rhs) const {
        return !(*this == rhs);
    }

protected:

    void free() {
        free(_first);
        _first = nullptr;
        _last = nullptr;
    }

    /** Copies via push_back */
    void copy(const List<T> &other) {
        _first = 0;
        _size = 0;
        Node *curr = other._first;
        while (curr != nullptr) {
            push_back(curr->_elem);
            curr = curr->_next;
        }
    }

private:

    /** 
     * Inserts an element between node1 & node2. Returns a pointer to new node. O(1)
     * General case: both exist.
     *   node1->_next == node2
     *   node2->_prev == node1
     * Special cases: one or both is nullptr
    */
    Node *insertElem(const T &e, Node *node1, Node *node2) {
        Node *new_node = new Node(node1, e, node2);
        if (node1 != nullptr)
            node1->_next = new_node;
        if (node2 != nullptr)
            node2->_prev = new_node;
        _size ++;
        return new_node;
    }

    /**
     * Removes a node. If node has neighbors, updates their pointers.
     * Does not accept nullptr.
     * O(1)
     */
    void deleteElem(Node *n) {
        assert(n != nullptr);
        Node *pprev = n->_prev;
        Node *pnext = n->_next;
        if (pprev != nullptr) {
            pprev->_next = pnext; // update previous, if any
        }
        if (pnext != nullptr) {
            pnext->_prev = pprev; // update next, if any
        }
        _size --;
        delete n;
    }

    /**
     * Removes all nodes from list. 
     * Passing nullptr is ok - nothing to free then. 
     * If argument is non-nullptr, must be 1st element in list.
     */
    static void free(Node *n) {
        // test for nullptr or 1st-element
        assert(!n || !n->_prev);
        while (n != nullptr) {
            Node *aux = n;
            n = n->_next;
            delete aux;
        }
    }

    // Pointers to 1st and last elements. May be both nullptr (empty list), or the same one (_size == 1)!
    Node *_first, *_last;

    // Element count
    unsigned int _size;
};

#endif // __LIST_H
