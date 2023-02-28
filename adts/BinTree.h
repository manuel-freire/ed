/**
 * Binary tree with manual reference counting
 * (c) Marco Antonio Gómez Martín, 2012
 * Modified by Ignacio Fábregas, 2022
 * Modified & translated by Manuel Freire, 2023
 */
#ifndef __BINTREE_H
#define __BINTREE_H

#include "Exceptions.h"
#include "List.h"    // Returned by traversals
#include "Queue.h"   // Used for traversal-by-levels
#include <iomanip>   // for setw (precise control of alignment when printing)
#include <iostream>  // endl 

/**
 * Dynamic implementation of a binary tree with pointers
 * for left & right children.
 * The structure can be shared, using reference counting to keep
 * track of uses. 
 * A better implementation would use C++ "smart pointers" for
 * simpler accounting.
 *
 * Operations are:
 * - BinaryTree constructor: generator
 * - left, right: observers, return left or right children of a tree (also trees!)
 * - elem: observer, returns element at root
 * - empty: observer, returning true IFF a tree is empty
 */

template <typename T>
class BinTree {
public:

    /** Constructor; returns an empty tree */
    BinTree() : _root(nullptr) {
    }

    /** Constructor; returns a tree from left + element + right */
    BinTree(const BinTree &iz, const T &elem, const BinTree &dr) :
            _root(new Node(iz._root, elem, dr._root)) {
        _root->addRef();
    }

    /** Constructor; returns a tree where the root is a leaf node containing elem  */
    BinTree(const T &elem) :
            _root(new Node(nullptr, elem, nullptr)) {
        _root->addRef();
    }

    /** Destructor; frees nodes (unless used elsewhere) & clears root. */
    ~BinTree() {
        free();
        _root = nullptr;
    }

    /**
     * Returns element at root.
     * Partial observer, O(1)
     */
    const T &elem() const {
        if (empty()) {
            throw EmptyTreeException();
        }
        return _root->_elem;
    }

    /**
     * Returns left subtree. Fails if tree is empty.
     * Partial observer, O(1)
     */
    BinTree left() const {
        if (empty()) {
            throw EmptyTreeException();
        }
        // note that this is using a copy ctor
        return BinTree(_root->_left);
    }

    /**
     * Returns right subtree. Fails if tree is empty.
     * Partial observer, O(1)
     */
    BinTree right() const {
        if (empty()) {
            throw EmptyTreeException();
        }
        // note that this is using a copy ctor
        return BinTree(_root->_right);
    }

    /** 
     * Returns true IFF tree is empty. 
     * Observer, O(1) 
     */
    bool empty() const {
        return _root == nullptr;
    }

    // //
    // TREE TRAVERSALS; all return pointers-to-list
    // //

    List<T>* preOrder() const {
        List<T>* ret = new List<T>();
        preOrderAux(_root, *ret);
        return ret;
    }

    List<T>* inOrder() const {
        List<T>* ret = new List<T>();
        inOrderAux(_root, *ret);
        return ret;
    }

    List<T>* postOrder() const {
        List<T>* ret = new List<T>();
        postOrderAux(_root, *ret);
        return ret;
    }

    List<T>* levels() const {
        List<T>* ret = new List<T>();
        if (!empty()){
            Queue<Node*> pending;
            pending.push_back(_root);

            while (!pending.empty()) {
                Node *current = pending.front();
                pending.pop_front();
                ret->push_back(current->_elem);
                if (current->_left != nullptr)
                    pending.push_back(current->_left);
                if (current->_right != nullptr)
                    pending.push_back(current->_right);
            }
        }
        return ret;
    }

    // //
    // OTHER OBSERVERS
    // //

    /** Returns the number of nodes in a tree. */
    unsigned int nodeCount() const {
        return nodeCountAux(_root);
    }

    /** Returns the depth of the tree. */
    unsigned int depth() const {
        return depthAux(_root);
    }

    /** Returns the number of leaves in a tree. */
    unsigned int leafCount() const {
        return leafCountAux(_root);
    }

    // //
    // BOILERPLATE C++ CODE 
    // //

    /** Copy ctor. */
    BinTree(const BinTree<T> &other) : _root(NULL) {
        copy(other);
    }

    /** Copy assignment operator. */
    BinTree<T> &operator=(const BinTree<T> &other) {
        if (this != &other) {
            free();
            copy(other);
        }
        return *this;
    }

    /** Comparison operators. */
    bool operator==(const BinTree<T> &rhs) const {
        return compareAux(_root, rhs._root);
    }

    bool operator!=(const BinTree<T> &rhs) const {
        return !(*this == rhs);
    }

    /** 
     *  Output, adapted from "ADTs, DataStructures, and Problem Solving with C++", 
     *  Larry Nyhoff, Person, 2015
     */
    friend std::ostream& operator<<(std::ostream& o, const BinTree<T>& t){
        o  << "==== Tree =====" << std::endl;
        outputIndented(o, 0, t._root);
        o << "===============" << std::endl;
        return o;
    }

    /** 
     * Pre-order input.
     * emptyRep is the element used to represent an empty node
     * with emptyRep X, example input could be 1 2 X X 3 X X for
     *     1
     *   2   3
     *  X X X X
     */
    static BinTree<T> fromPreOrderInput(const T& emptyRep) {
        T elem;
        std::cin >> elem;
        if (elem == emptyRep)
            return BinTree<T>();
        else {
            BinTree<T> hi = fromPreOrderInput(emptyRep);
            BinTree<T> hd = fromPreOrderInput(emptyRep);
            return BinTree<T>(hi, elem, hd);
        }
    }

     /** 
      * In-order input 
      * Expects '.' for empty, and ( and ) to delimit left and right
      * example input of ( ( . 2 . ) 1 ( . 3 . ) would result in
     *     1
     *   2   3
     *  X X X X
      */
    static BinTree<T> fromInOrderInput() {
        char c;
        std::cin >> c;
        if (c == '.')
            return BinTree<T>(); 
        else {
            assert (c == '(');
            BinTree<T> left = fromInOrderInput();
            T elem;
            std::cin >> elem;
            BinTree<T> right = fromInOrderInput();
            std::cin >> c;
            assert (c == ')');
            BinTree<T> result(left, elem, right);
            return result;
        }
    }



protected:
    /** used to generate output */
    static const int TREE_INDENTATION = 4;

    /**
     * Internal node class
     */
    class Node {
    public:
        Node() : _left(nullptr), _right(nullptr), _refs(0) {}
        Node(Node *left, const T &elem, Node *right) :
                _elem(elem), _left(left), _right(right), _refs(0) {
            if (left != nullptr)
                left->addRef();
            if (right != nullptr)
                right->addRef();
        }

        void addRef() { assert(_refs >= 0); _refs++; }
        void rmRef() { assert(_refs > 0); _refs--; }

        T _elem;
        Node *_left;
        Node *_right;

        int _refs;
    };

    /**
     * Protected constructor, which builds a tree from an existing root node.
     * Since that node is being SHARED, also increments its reference count.
     */
    BinTree(Node *raiz) : _root(raiz) {
        if (_root != nullptr) {
            _root->addRef();
        }
    }

    /** for cleanup */
    void free() {
        free(_root);
    }

    /** copy */
    void copy(const BinTree &other) {
        assert(this != &other);
        _root = other._root;
        if (_root != nullptr) {
            _root->addRef();
        }
    }

    // //
    // AUX METHODS FOR TRAVERSAL
    // //
    
    static void preOrderAux(Node *root, List<T> &acc) {
        if (root != nullptr){
            acc.push_back(root->_elem);
            preOrderAux(root->_left, acc);
            preOrderAux(root->_right, acc);
        }
    }

    static void inOrderAux(Node *root, List<T> &acc) {
        if (root != nullptr) {
            inOrderAux(root->_left, acc);
            acc.push_back(root->_elem);
            inOrderAux(root->_right, acc);
        }
    }

    static void postOrderAux(Node *root, List<T> &acc) {
        if (root != nullptr) {
            postOrderAux(root->_left, acc);
            postOrderAux(root->_right, acc);
            acc.push_back(root->_elem);
        }
    }

    static void outputIndented(std::ostream & out, int indent, Node* root){
        if (root != nullptr) {
            outputIndented(out, indent + TREE_INDENTATION, root->_right);
            out << std::setw(indent) << " " << root->_elem << std::endl;
            outputIndented(out, indent + TREE_INDENTATION, root->_left);
        }
    }

    // //
    // OTHER AUX METHODS
    // //

    static unsigned int nodeCountAux(Node *root) {
        if (root == nullptr) {
            return 0;
        }
        return 1 + nodeCountAux(root->_left) + nodeCountAux(root->_right);
    }

    static unsigned int depthAux(Node *root) {
        if (root == nullptr) {
            return 0;
        }
        int leftDepth = depthAux(root->_left);
        int rightDepth = depthAux(root->_right);
        if (leftDepth > rightDepth) {
            return 1 + leftDepth;
        } else {
            return 1 + rightDepth;
        }
    }

    static unsigned int leafCountAux(Node *root) {
        if (root == nullptr) {
            return 0;
        }

        if ((root->_left == nullptr) && (root->_right == nullptr)) {
            return 1;
        }

        return leafCountAux(root->_left) + leafCountAux(root->_right);
    }

private:

    /**
     * Removes all nodes at or below a given root.
     * The root may be null (in which case it does nothing)
     * Other references may still be in use (in which case it only
     * decreases the reference counter)
     */
    static void free(Node *root) {
        if (root != nullptr) {
            root->rmRef();
            if (root->_refs == 0) {
                free(root->_left);
                free(root->_right);
                delete root;
            }
        }
    }

    /**
     * Compares two nodes and their children for equality
     */
    static bool compareAux(Node *r1, Node *r2) {
        if (r1 == r2)
            // Note that this covers "both are null"
            return true;
        else if ((r1 == nullptr) || (r2 == nullptr))
            // This only covers "one is null and not the other"
            // if both are null, the previous check would have returned true
            return false;
        else {
            return (r1->_elem == r2->_elem) &&
                   compareAux(r1->_left, r2->_left) &&
                   compareAux(r1->_right, r2->_right);
        }
    }

protected:
    /**
     * Root node
     */
    Node *_root;
};

#endif // __BINTREE_H
