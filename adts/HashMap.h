/**
 * Map ADT using an open hash table
 * (c) Antonio Sánchez Ruiz-Granados, 2012
 * Modified by Ignacio Fábregas, 2022
 * Modified & translated by Manuel Freire, 2023
 */

#ifndef __HASHMAP_H
#define __HASHMAP_H

#include <iostream>
#include "Exceptions.h"

/**
 * Map using an open HashTable
 * 
 * Requires keys to support a hash function: a function that accepts a single
 * 	   argument of the key type, and which consistently returns the same unsigned int outputs
 *     for its inputs, and ideally spreads the output values to that collisions are rare.
 * Operations are:
 *    - HashMap constructor: generator
 *    - insert(key, value): generator, adds a new (key, value) pair to the tree.
 *          If the key was already present, replaces its value with the new one.
 *    - erase(key): mutator. Removes the key from the tree. No effect if key absent.
 *    - at(key): observer. Returns value that corresponds to a key. 
 *          Partial: key must exist; use contains() first if unsure.
 *    - contains(key): observer. Returnes true iff key exists in map
 *    - empty(): observer. Returns true if no keys present.
 *    - size(): observer. Returns count of currently-contained keys.
 */
template <typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
private:
	/**
	 * Internal node class used in bins.
	 */
	class Node {
	public:
		/* Ctors. */
		Node(const K &key, const V &value) :
                _key(key), _value(value), _next(nullptr) {};
		
		Node(const K &key, const V &value, Node *next) :
                _key(key), _value(value), _next(next) {};

		K _key;
		V _value;
		Node *_next;
	};  
	
public:
	
	/** Initial table size (number of bins). */
	static const int INITIAL_BIN_COUNT = 8;    
	
	/** Constructor; returns an empty HashMap. O(1) */
	HashMap() : _bins(new Node*[INITIAL_BIN_COUNT]), _binCount(INITIAL_BIN_COUNT), _entryCount(0) {
		for (unsigned int i=0; i < _binCount; ++i)
            _bins[i] = nullptr;
	}
	
    /** Destructor; frees nodes in O(n) */
	~HashMap() {
		free();
	}
	
    /** 
     * Adds a new key, value pair to the set. If key
     * already present, replaces old value with new one.  
     * No effect if element already present.
     * generator, O(1) amortized cost. 
     */
	void insert(const K &key, const V &value) {        
		// If occupation very high, grow table
		float occupation = 100 * ((float) _entryCount) / _binCount;
		if (occupation > MAX_OCCUPATION) {			
			grow();
		}
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		if (n != nullptr) { // key exists in bin: overwrite value
			n->_value = value;
		} else { // new key: insert new node
			_bins[idx] = new Node(key, value, _bins[idx]);
			_entryCount++;
		}
	}
	
    /**
     * Removes a key, value pair from the map. 
     * No effect if key not there in the first place. 
     * mutator, O(1)
     */
	void erase(const K &key) {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Find node with key; and if present, also previous node
		Node *n = _bins[idx];
		Node *prev = nullptr;
        findWithPrevious(key, n, prev);
		if (n != nullptr) { // found!
			// remove from list
			if (prev != nullptr) {
				prev->_next = n->_next;
			} else {
                _bins[idx] = n->_next;
			}
			// And now, delete it
			delete n;
			_entryCount--;
		}        
	}
	
	/**
	 * Returns value associated to a key. 
     * Partial - if key not present, throws exception. Use contains() if unsure
     * observer, O(log n)
	 */
	const V &at(const K &key) const {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		if (n == nullptr) {
			throw BadKeyException();			
		}
		return n->_value;
	}
	
    /** 
     * Returns true IFF element in set
     * observer, O(log n)
     */
	bool contains(const K &key) const {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		return n != nullptr;
	}
	
    /** 
     * Returns true IFF no elements in set
     * observer, O(1)
     */
	bool empty() const {
		return _entryCount == 0;
	}

    /** 
     * Returns number of keys in map. 
     * observer, O(1) 
     */
	int size() const {
        return _entryCount;
    }
	
	/**
	 * Overloads the [] operator, to access (and possibly modify) a value given its key.
	 * If the key is not present, inserts a new (default) value for that key.
	 */
	V &operator[](const K &key) {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		if (n == nullptr) { // Not there, must add
			insert(key, V());
			// NOTE: idx may have changed due to growth!
			idx = _hash(key) % _binCount;
			n = findNode(key, _bins[idx]);
		}        
		return n->_value;
	}

    // //
    // NON-CONSTANT ITERATOR
    // //

	/**
     * An iterator that allows walking through the whole map. 
     * Allows changes.
	 */
    class Iterator {
    public:
        Iterator() : _table(nullptr), _current(nullptr), _idx(0) {}

        /** O(1) amortized. */
		void next() {
			if (_current == nullptr) {
				// reached end; complain
                throw InvalidAccessException();
			}
			// return next node in current bin
			_current = _current->_next;
			// if bin exhausted, return 1st node in next non-empty bin
			while ((_current == nullptr) && (_idx < _table->_binCount - 1)) {
				++_idx;
                _current = _table->_bins[_idx];
			}
		}

        /** O(1) */
        const K &key() const {
            if (_current == nullptr) {
                throw InvalidAccessException();
			}
            return _current->_key;
        }

        /** O(1) */
        V &value() const {
            if (_current == nullptr) {
                throw InvalidAccessException();
			}
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

        /** O(1) */
        Iterator &operator++() {
            next();
            return *this;
        }

        /** O(1) */
        Iterator operator++(int) {
            Iterator ret(*this);
            operator++();
            return ret;
        }

    protected:
        friend class HashMap;

		Iterator(const HashMap* table, Node* current, unsigned int idx) 
			: _table(table), _current(current), _idx(idx) { }

        /** Pointer to hash table being iterated */
		const HashMap *_table;

        /** Pointer to current node in current bin */
        Node* _current;

        /** Current bin index */
		unsigned int _idx;
    };

    /**
     * Returns a non-constant map iterator starting at the "first" element.
	 * Note that has tables are not really ordered.
     * O(1)
     */
	Iterator begin() const {
		unsigned int idx = 0;
		Node *n = _bins[0];
		while (idx < _binCount - 1 && n == nullptr) {
			idx++;
			n = _bins[idx];
		}
		return Iterator(this, n, idx);
	}
	
    /** 
     * Returns a non-constant map iterator just outside the map, reachable by an iterator 
     * that starts at begin()
     * O(1) 
     */
	Iterator end() const {
		return Iterator(this, nullptr, 0);
	}
	
	/**
	 * Returns an iterator to a the location of a key.
	 * If key not found, returns end().
	 */
	Iterator find(const K &key) {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		return Iterator(this, n, idx); // if nullptr, returns end()
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
		ConstIterator() : _table(nullptr), _current(nullptr), _idx(0) {}

        /** Converts an Iterator to a ConstIterator */
        ConstIterator(const Iterator& it){
            _table = it._table;
            _current = it._current;
            _idx = it._idx;
        }

        /** O(1) amortized. */
		void next() {
			if (_current == nullptr) {
				// reached cend; complain
                throw InvalidAccessException();
			}
			// return next node in current bin
			_current = _current->_next;
			// if bin exhausted, return 1st node in next non-empty bin
			while ((_current == nullptr) && (_idx < _table->_binCount - 1)) {
				++_idx;
                _current = _table->_bins[_idx];
			}
		}
		
        /** O(1) */
		const K &key() const {
			if (_current == nullptr) {
                throw InvalidAccessException();
			}
			return _current->_key;
		}
		
        /** O(1) */
		const V &value() const {
			if (_current == nullptr) {
                throw InvalidAccessException();
			}
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
		
        /** O(1) */
		ConstIterator &operator++() {
			next();
			return *this;
		}
		
        /** O(1) */
		ConstIterator operator++(int) {
			ConstIterator ret(*this);
			operator++();
			return ret;
		}
		
	protected:
		friend class HashMap;
		
		ConstIterator(const HashMap* table, Node* current, unsigned int idx) 
			: _table(table), _current(current), _idx(idx) { }

        /** Pointer to hash table being iterated */
		const HashMap *_table;

        /** Pointer to current node in current bin */
        Node* _current;

        /** Current bin index */
		unsigned int _idx;
	};
	
    /**
     * Returns a constant map iterator starting at the "first" element.
	 * Note that has tables are not really ordered.
     * O(1)
     */
	ConstIterator cbegin() const {
		unsigned int idx = 0;
		Node *n = _bins[0];
		while (idx < _binCount - 1 && n == nullptr) {
			idx++;
			n = _bins[idx];
		}
		return ConstIterator(this, n, idx);
	}
	
    /** 
     * Returns a constant map iterator just outside the map, reachable by an iterator 
     * that starts at cbegin()
     * O(1) 
     */
	ConstIterator cend() const {
		return ConstIterator(this, nullptr, 0);
	}
	
	/**
	 * Returns an iterator to a the location of a key.
	 * If key not found, returns cend().
	 */
	ConstIterator find(const K &key) const {
		// Locate bin index
		unsigned int idx = _hash(key) % _binCount;
		// Look for key in bin
		Node *n = findNode(key, _bins[idx]);
		return ConstIterator(this, n, idx); // if nullptr, returns cend()
	}

	
	// //
    // C++ Boilerplate code to make class more useful
	// //

    /** 
     * Pretty-printing of map. Only for debugging. 
     * observer, O(n)
     */    
    friend std::ostream& operator<<(std::ostream& o, const HashMap& t){
        o<<"{";
        t.show(o);
        o<<"}";
        return o;
    }
	
	/** Copy ctor. O(n) */
	HashMap(const HashMap<K, V, Hash> &other) {
		copy(other);
	}
	
	/** Assignment operator. O(n) */
	HashMap<K, V, Hash> &operator=(const HashMap<K, V, Hash> &other) {
		if (this != &other) {
			free();
			copy(other);
		}
		return *this;
	}
	
private:
	
	/** Frees all memory allocated in this table. */
	void free() {
		// Frees all node lists
		for (unsigned int i=0; i < _binCount; i++) {
			freeNodes(_bins[i]);
		}
		// Frees the array of pointers to nodes.
		if (_bins != nullptr) {
			delete[] _bins;
            _bins = nullptr;
		}
	}
	
	/** Frees nodes in a linked list. O(n), where n is length of list */
	static void freeNodes(Node *n) {
		while (n != nullptr) {
			Node *aux = n;
			n = n->_next;
			delete aux;
		}       
	}
	
	/**
	 * Copies a table received as a parameter.
	 * Before calling this, you should have freed any memory from this table
	 */
	void copy(const HashMap<K, V, Hash> &other) {
        _binCount = other._binCount;
        _entryCount = other._entryCount;
		// Allocate bin array
		_bins = new Node*[_binCount];
		for (unsigned int i=0; i < _binCount; ++i) {
            _bins[i] = nullptr;
			// Copy node bin; reverses bin order, but this is generally not visible
			Node *n = other._bins[i];
			while (n != nullptr) {
                _bins[i] = new Node(n->_key, n->_value, _bins[i]);
				n = n->_next;
			}
		}
	}
	
	/** 
	 * Grows the table: doubles the number of bins. O(n) 
	 */
	void grow() {
		// Keep a pointer to the old bins, and also keep a copy of their size.
		Node **oldBins = _bins;
		unsigned int oldBinCount = _binCount;
		// Allocate a new bin array, of double the size
		_binCount *= 2;
        _bins = new Node*[_binCount];
		for (unsigned int i=0; i < _binCount; ++i) {
            _bins[i] = nullptr;
		}
		
		// Iterate the original array
		for (unsigned int i=0; i<oldBinCount; ++i) {
            /* NOTE: changing the size will geneally also change the bin index.
             * For efficiency reasons, we do not copy nodes, we only move them.
             */
			Node *n = oldBins[i];
			while (n != nullptr) {
				Node *aux = n;
				n = n->_next;
				unsigned int idx = _hash(aux->_key) % _binCount; // new index
				aux->_next = _bins[idx];
                _bins[idx] = aux;
			}
		}
		// Borramos el array antiguo (ya no contiene ningún nodo).
		delete[] oldBins;
	}
	
	/**
	 * Finds a node in a linked list. If found, will store in current, and parent in 
	 * prev. If not, current will be set to nullptr.
	 * O(k), where k is the length of the linked list
	 */
	static void findWithPrevious(const K &key, Node* &current, Node* &prev) {
		prev = nullptr;
		bool found = false;
		while ((current != nullptr) && !found) {
			// check current key against searched-for key 
			if (current->_key == key) {
				found = true;
			} else {
				prev = current;
				current = current->_next;
			}
		}
	}
	
	/**
	 * Finds a node in a linked list. If found, returns it. Otherwise, returns nullptr.
	 * O(k), where k is the length of the linked list
	 */
	static Node* findNode(const K &key, Node* n) {
		Node *current = n;
		Node *next = nullptr;
        findWithPrevious(key, current, next);
		return current;
	}

    /** 
     * Pretty-printing of map. Only for debugging. 
     * observer, O(n)
     */    
    void muestra(std::ostream &out) const {
        bool first = true;
        for (int i = 0; i < _binCount; i++) {
            Node *n = _bins[i];
            while(n != nullptr){ // if bin not empty, iterates contents
                out << (first ? "" : ", ");
                first = false;
                out << n->_key << " -> " << n->_value;
                n = n->_next;
            }
        }
    }

	
	/**
	 * Max percentage full before triggering growth.
	 */
	static const unsigned int MAX_OCCUPATION = 80;

    /** Array of node pointers / bins */
	Node **_bins;

    /** Chosen hash function */
    Hash _hash;

    /** Number of bins in _bins */
	unsigned int _binCount;

    /** Number of entries in the table */
	unsigned int _entryCount;
};

#endif // __HASHMAP_H
