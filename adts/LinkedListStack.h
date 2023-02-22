/**
 * Implementation of the Stack ADT using a singly-linked list
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * English & more by Manuel Freire, 2023
 */
#ifndef __LL_STACK_H
#define __LL_STACK_H

#include "Exceptions.h"
#include <iostream>
#include <iomanip>

/**
 * Implementation of a Stack ADT using a linked list of nodes.
 * Operations are:
 *   - EmptyStack: -> Stack. Generator (empty constructor)
 *   - push: Stack, Elem -> Stack. Generator
 *   - pop: Stack -> Stack. Partial modifier
 *   - top: Stack -> Elem. Partial observer
 *   - empty: Stack -> Bool. Observer
 *   - size: Stack -> Int. Observer
 */
template <class T>
class LinkedListStack {
public:

    /** Constructor; EmptyStack operation. O(1) */
    LinkedListStack() : _top(nullptr), _size(0) {
    }

    /** Destructor; frees the list. O(n) */
    ~LinkedListStack() {
        free();
        _top = nullptr; // mark as empty
    }

    /** Pushes an element. Generator. O(1) */
    void push(const T &_elem) {
        _top = new Node(_elem, _top);
        _size++;
    }
    
    /**
     * Pops an element (whichever was pushed last). 
     * Partial modifier (fails if empty). O(1)
     */
    void pop() {
        if (empty()) {
            throw EmptyStackException("Cannot pop. The stack is empty");
        }
        Node *toRemove = _top;
        _top = _top->_next;
        delete toRemove;
        --_size;
    }

    /**
     * Returns top-most element (whichever would be popped by pop()).
     * Partial observer (fails if empty). O(1)
     */
    const T &top() const {
        if (empty()) {
            throw EmptyStackException("Cannot get top. The stack is empty");
        }
        return _top->_elem;
    }

    /** True IFF stack is empty. Observer. O(1) */
    bool empty() const {
        return _top == nullptr;
    }

    /** Returns number of elements. Observer. O(1) */
    int size() const {
        return _size;
    }

    // //
    // C++ Boilerplate code to make class more useful
    // //

    /** Copy constructor. O(n) */
    LinkedListStack(const LinkedListStack<T> &other) : _top(nullptr) {
        copy(other);
    }

    /** Assignment constructor. O(n) */
    LinkedListStack<T> &operator=(const LinkedListStack<T> &other) {
        if (this != &other) {
            free();
            copy(other);
        }
        return *this;
    }

    /** Equality operator. O(n) */
    bool operator==(const LinkedListStack<T> &rhs) const {
        if (_size != rhs._size)
            return false;
        bool same = true;
        Node *top1 = _top;
        Node *top2 = rhs._top;
        while ((top1 != nullptr) && (top2 != nullptr) && same) {
            if (top1->_elem != top2->_elem) {
                same = false;
            } else {
                top1 = top1->_next;
                top2 = top2->_next;
            }
        }
        return same;
    }

    /** Inequality operator. O(n) */
    bool operator!=(const LinkedListStack<T> &rhs) const {
        return !(*this == rhs);
    }

    /** Outputs to stream using operator <<, stacking elements from top to bottom. */
    void write(std::ostream& sOut) const {
        Node* aux = _top;
        while (aux != nullptr){
            sOut << "| " << std::setw(2) << std::left << aux->_elem << "|" << std::endl;
            aux = aux->_next;
        }
        sOut << "|---|" << std::endl;
    }

protected:

    void free() {
        free(_top);
    }

    void copy(const LinkedListStack &other) {
        if (other.empty()) {
            _top = nullptr;
            _size = 0;
        } else {
            Node *_current = other._top;
            Node *_prev;
            _top = new Node(_current->_elem);
            _prev = _top;
            while (_current->_next != nullptr) {
                _current = _current->_next;
                _prev->_next = new Node(_current->_elem);
                _prev = _prev->_next;
            }
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
    Node *_top;

    /** Element count */
    int _size;
};

/** Output operator, for use with streams */
template<class T>
std::ostream& operator<<(std::ostream& sOut, LinkedListStack<T>& s) {
    s.write(sOut);
    return sOut;
}

#endif // __LL_STACK_H
