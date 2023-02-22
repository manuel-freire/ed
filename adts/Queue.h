/**
 * Implementation of a Queue ADT using a singly-linked list
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * English & more by Manuel Freire, 2023
 */
#ifndef __LL_QUEUE_H
#define __LL_QUEUE_H

#include "Exceptions.h"
#include <iostream>

/**
 * Implementation of a Queue ADT using a linked list of nodes.
 * Operations are:
 *   - EmptyQueue: -> Queue. Queue (empty constructor)
 *   - push_back: Queue, Elem -> Stack. Generator
 *   - pop_front: Queue -> Queue. Partial modifier
 *   - front: Queue -> Elem. Partial observer
 *   - empty: Queue -> Bool. Observer
 *   - size: Queue -> Int. Observer
 */
template <class T>
class Queue {
public:

    /** Constructor; EmptyQueue operation. O(1) */
    Queue() : _first(nullptr), _last(nullptr), _size(0) {
    }

    /** Destructor; frees the list. O(n) */
    ~Queue() {
        free();
        _first = _last = nullptr;
    }

    /** Pushes an element at back. Generator. O(1) */
    void push_back(const T &_elem) {
        Node *newNode = new Node(_elem, nullptr);
        if (_last != nullptr) {
            _last->_next = newNode;
        }
        _last = newNode;
        // If queue was empty, newNode is also _first
        if (_first == nullptr) {
            _first = newNode;
        }
        _size++;
    }

    /**
     * Pops an element (whichever was pushed first). 
     * Partial modifier (fails if empty). O(1)
     */
    void pop_front() {
        if (empty()) {
            throw EmptyQueueException("Cannot pop: Queue is empty");
        }
        Node *toRemove = _first;
        _first = _first->_next;
        delete toRemove;
        --_size;
        if (_first == nullptr) //si la cola queda vacía, no hay último
            _last = nullptr;
    }

    /**
     * Returns top-most element (whichever would be popped by pop_front()).
     * Partial observer (fails if empty). O(1)
     */
    const T &front() const {
        if (empty()) {
            throw EmptyQueueException("Cannot get front: Queue is empty");
        }
        return _first->_elem;
    }

    /** True IFF queue is empty. Observer. O(1) */
    bool empty() const {
        return _first == nullptr;
    }

    /** Returns number of elements. Observer. O(1) */
    int size() const {
        return _size;
    }

    // //
    // C++ Boilerplate code to make class more useful
    // //

    /** Copy ctor. O(n) */
    Queue(const Queue<T> &other) : _first(nullptr), _last(nullptr) {
        copy(other);
    }

    /** Assignment constructor. O(n) */
    Queue<T> &operator=(const Queue<T> &other) {
        if (this != &other) {
            free();
            copy(other);
        }
        return *this;
    }

    /** Equality operator. O(n) */
    bool operator==(const Queue<T> &rhs) const {
        if (_size != rhs._size)
            return false;
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

    /** Inequality operator. O(n) */
    bool operator!=(const Queue<T> &rhs) const {
        return !(*this == rhs);
    }

    /** Outputs to stream using operator <<, from first to last. */
    void write(std::ostream& sOut) {
        Node* aux = _first;
        while (aux != nullptr){
            sOut << aux->_elem;
            aux = aux->_next;
            if (aux != nullptr) sOut << " ";
        }
    }

protected:

    void free() {
        free(_first);
    }

    void copy(const Queue &other) {
        if (other.empty()) {
            _first = _last = nullptr;
            _size = 0;
        } else {
            Node *_current = other._first;
            Node *_prev;
            _first = new Node(_current->_elem);
            _prev = _first;
            while (_current->_next != nullptr) {
                _current = _current->_next;
                _prev->_next = new Node(_current->_elem);
                _prev = _prev->_next;
            }
            _last = _prev;
            _size = other._size;
        }
    }

private:

    /**
     * Node class. Stores the element (of type T), and pointers to next node
     * Pointer to next may be nullptr for last node in list
     */
    class Node {
    public:
        Node() : _next(nullptr) {}
        Node(const T &_elem) : _elem(_elem), _next(nullptr) {}
        Node(const T &_elem, Node *_next) : _elem(_elem), _next(_next) {}

        T _elem;
        Node *_next;
    };

    /**
     * Removes all nodes from list. 
     * Passing nullptr is ok - nothing to free then. 
     * If argument is non-nullptr, must be 1st element in list.
     */
    static void free(Node *n) {
        while (n != nullptr) {
            Node *aux = n;
            n = n->_next;
            delete aux;
        }
    }

    /** Pointer to 1st element */
    Node *_first;

    /** Pointer to last element */
    Node *_last;

    /** Element count */
    int _size;
};

/** Output operator, for use with streams */
template<class T>
std::ostream& operator<<(std::ostream& sOut, Queue<T>& q) {
    q.write(sOut);
    return sOut;
}

#endif // __LL_QUEUE_H
