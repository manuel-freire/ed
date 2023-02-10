/**
 * Implementation of the Stack ADT using a dynamic array
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * English & more by Manuel Freire, 2023
 */
#ifndef __STACK_H
#define __STACK_H

#include "Exceptions.h"
#include <iostream>
#include <iomanip>

/**
 * Implementation of a Stack ADT using a dynamic array.
 * Operations are:
 *   - EmptyStack: -> Stack. Generator (empty constructor)
 *   - push: Stack, Elem -> Stack. Generator
 *   - pop: Stack -> Stack. Partial modifier
 *   - top: Stack -> Elem. Partial observer
 *   - empty: Stack -> Bool. Observer
 *   - size: Stack -> Int. Observer
 */
template <class T>
class Stack {
public:

	/** Initial size of dynamic array. */
	static const int INITIAL_CAPACITY = 10;

	/** Ctor; EmptyStack operation */
	Stack() {
		init();
	}

	/** Dtor; frees vector. */
	~Stack() {
		free();
	}

	/** Pushes an element. Generator. O(1) (amortized) */
	void push(const T &_elem) {
        _data[_size] = _elem;
		_size++;
		if (_size == _max) {
			grow(); // full, must grow
		}
	}
	
	/**
	 * Pops an element (whichever was pushed last). 
	 * Partial modifier (fails if empty). O(1)
	 */
	void pop() {
		if (empty()) {
			throw EmptyStackException("Cannot pop. The stack is empty");
		}
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
		return _data[_size - 1];
	}

	/** True IFF stack is empty. Observer. O(1) */
	bool empty() const {
		return _size == 0;
	}

	/** Returns number of elements. Observer. O(1) */
	int size() const {
		return _size;
	}

	// //
	// C++ Boilerplate code to make class more useful
	// //

	/** Copy constructor. O(n) */
	Stack(const Stack<T> &other) {
		copy(other);
	}

	/** Assignment constructor. O(n) */
	Stack<T> &operator=(const Stack<T> &other) {
		if (this != &other) {
			free();
			copy(other);
		}
		return *this;
	}

	/** Equality operator. O(n) */
	bool operator==(const Stack<T> &rhs) const {
		if (_size != rhs._size) {
			return false;
		}
		bool same = true;
		for (unsigned int i = 0; i < _size && same; ++i) {
			if (_data[i] != rhs._data[i]) {
				same = false;
			}
		}
		return same;
	}

    /** Inequality operator. O(n) */
	bool operator!=(const Stack<T> &rhs) const {
		return !(*this == rhs);
	}

	/** Outputs to stream using operator <<, stacking elements from top to bottom. */
	void write(std::ostream& sOut) {
		for (int i = _size - 1; i >= 0; --i)
			sOut << "| " << std::setw(2) << std::left << _data[i] << "|" << std::endl;
		sOut << "|---|" << std::endl;
	}

protected:

    /** creates a dynamic array with the initial capacity and no elements */
	void init() {
        _data = new T[INITIAL_CAPACITY];
        _max = INITIAL_CAPACITY;
        _size = 0;
	}

	void free() {
		delete []_data;
        _data = NULL;
	}

	void copy(const Stack &other) {
        _max = other._size + INITIAL_CAPACITY;
        _size = other._size;
        _data = new T[_max];
		for (unsigned int i = 0; i < _size; ++i) {
            _data[i] = other._data[i];
		}
	}

    /** creates 2x bigger backing array, copying over data and freeing old array */
	void grow() {
		T *old = _data;
        _max *= 2;
        _data = new T[_max];
		for (unsigned int i = 0; i < _size; ++i)
            _data[i] = old[i];

		delete []old;
	}

private:

	/** Pointer to data. */
	T * _data;

	/** Capacity of array (as reserved via new[]). */
	unsigned int _max;

	/** Actual number of stored elements. */
	unsigned int _size;
};

/** Output operator, for use with streams */
template<class T>
std::ostream& operator<<(std::ostream& sOut, Stack<T>& s) {
	s.write(sOut);
	return sOut;
}


#endif // __STACK_H
