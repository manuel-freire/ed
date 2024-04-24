/**
 * List ADT implemented with a singly-linked list
 * Original by Manuel Montenegro
 * Modified by Ignacio Fábregas
 * English & more by Manuel Freire
 *
 * Generic version, with a phantom node, head & tail
 */
  
#ifndef __LIST_LINKED_SINGLE_PLUS_H
#define __LIST_LINKED_SINGLE_PLUS_H

#include <cassert>
#include <iostream>

template<typename T>
class ListLinkedSingle {
protected:

    /** Node as structure */
    struct Node {
        T _value;
        Node *_next;
    };

public:
    /** O(1) */
    ListLinkedSingle() {
        _head = new Node;
        _head->_next = nullptr;
        _tail = _head;
    }

    /** O(n) */
    ~ListLinkedSingle() {
        delete_list(_head);
    }

    /** O(n) */
    ListLinkedSingle(const ListLinkedSingle &other): _head(copy_nodes(other._head)) {
        _tail = nth_node(other.size()-1);
    }

    /** Assignment constructor. O(n) */
    ListLinkedSingle<T> &operator=(const ListLinkedSingle<T> &other) {
        if (this != &other) {
            delete_list(_head->_next);
            _head->_next = copy_nodes(other._head->_next);
            if ( ! other.empty()) {
              _tail = nth_node(other.size()-1);
            }
        }
        return *this;
    }

    /** O(1)*/
    void push_front(const T &_elem) {
        Node *new_node = new Node { _elem, _head->_next };
        _head->_next = new_node;
        if(_tail == _head) //si la lista era unitaria
             _tail = new_node;
    }

    /** O(1)*/
    void push_back(const T &_elem);

    /** O(1) */
    void pop_front() {
        assert (_head->_next != nullptr);
        Node *old_head = _head->_next;
        _head->_next = _head->_next->_next;
        delete old_head;
        if(_head->_next == nullptr)
            _tail = _head;
    }

    /** O(n) */
    void pop_back();

    /** O(n) */
    int size() const;

    /** O(1)*/
    bool empty() const {
        return _head->_next == nullptr;
    };

    /** O(1)*/
    const T & front() const {
        assert (_head->_next != nullptr);
        return _head->_next->_value;
    }

    /** non-const front  O(1)*/
    T & front() {
        assert (_head->_next != nullptr);
        return _head->_next->_value;
    }

    /** O(1)*/
    const T & back() const {
        assert (_tail != nullptr);
        return _tail->_value;
    }

    /** non-const back O(1)*/
    T & back() {
        assert (_tail != nullptr);
        return _tail->_value;
    }

    /** O(n)*/
    const T & at(int index) const {
        Node *result_node = nth_node(index);
        assert (result_node != nullptr);
        return result_node->_value;
    }

    /** non-const at O(1)*/
    T & at(int index) {
        Node *result_node = nth_node(index);
        assert (result_node != nullptr);
        return result_node->_value;

    }

    /** output to a stream O(n)*/
    void display(std::ostream &out) const;

    /** O(n)*/
    void display() const {
        display(std::cout);
     }

private:
    /** pointer to 1st node */
    Node *_head;
    /** pointer to last node */
    Node *_tail;

    void delete_list(Node *start_node);
    Node *nth_node(int n) const;
    Node *copy_nodes(Node *start_node) const;

};


template <typename T>
typename ListLinkedSingle<T>::Node * ListLinkedSingle<T>::copy_nodes(Node *start_node) const {
  if (start_node != nullptr) {
    Node *result = new Node { start_node->_value, copy_nodes(start_node->_next) };
    return result;
  } else {
    return nullptr;
  }
}

template <typename T>
void ListLinkedSingle<T>::delete_list(Node *start_node) {
  if (start_node != nullptr) {
    delete_list(start_node->_next);
    delete start_node;
  }
}

template <typename T>
void ListLinkedSingle<T>::push_back(const T &_elem) {
  Node *new_node = new Node { _elem, nullptr };
  _tail->_next = new_node;
  _tail = _tail->_next;
}

template <typename T>
void ListLinkedSingle<T>::pop_back() {
  assert (_head->_next != nullptr);
  Node *previous = _head;
  Node *current = _head->_next;

  while (current->_next != nullptr) {
    previous = current;
    current = current->_next;
  }

  delete current;
  previous->_next = nullptr;
  _tail = previous;
}

template <typename T>
int ListLinkedSingle<T>::size() const {
  int num_nodes = 0;

  Node *current = _head->_next;
  while (current != nullptr) {
    num_nodes++;
    current = current->_next;
  }

  return num_nodes;
}

template <typename T>
typename ListLinkedSingle<T>::Node * ListLinkedSingle<T>::nth_node(int n) const {
  assert (0 <= n);
  int current_index = 0;
  Node *current = _head->_next;

  while (current_index < n && current != nullptr) {
    current_index++;
    current = current->_next;
  }

  return current;
}

template <typename T>
void ListLinkedSingle<T>::display(std::ostream &out) const {
    if (_head->_next != nullptr) {
        out << _head->_next->_value;
        Node *current = _head->_next->_next;
        while (current != nullptr) {
            out << " " << current->_value;
            current = current->_next;
        }
    }
}


#endif
