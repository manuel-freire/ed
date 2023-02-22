/**
 * Binary tree with C++ smart shared pointers
 * (c) Ignacio Fábregas, 2022
 * Based on code by Marco Antonio Gómez Martín and Enrique Martín Martín
 * Modified & translated by Manuel Freire, 2023
 */
#ifndef __BINTREE_SMART_H
#define __BINTREE_SMART_H

#include "Exceptions.h"
#include "List.h"    // Returned by traversals
#include "Queue.h"   // Used for traversal-by-levels
#include <iomanip>   // for setw (precise control of alignment when printing)
#include <iostream>  // endl 
#include <memory>    //shared_ptr

/**
 * Dynamic implementation of a binary tree with pointers
 * for left & right children.
 * The structure can be shared, using C++ smart shared pointers
 * to only clean up when necessary.
 *
 * Operations are:
 * - BinaryTreeSmart constructor: generator
 * - left, right: observers, return left or right children of a tree (also trees!)
 * - elem: observer, returns element at root
 * - empty: observer, returning true IFF a tree is empty
 */

template <typename T>
class BinTreeSmart {
public:

    /** Constructor; returns an empty tree */
    BinTreeSmart() : _root(nullptr) {
    }

    /** Constructor; returns a tree from left + element + right */
    BinTreeSmart(const BinTreeSmart &left, const T &elem, const BinTreeSmart &right) :
            _root(std::make_shared<Node>(Node(left._root, elem, right._root))) {}

    /** Constructor; returns a tree where the root is a leaf node containing elem  */
    BinTreeSmart(const T &elem) :
            _root(std::make_shared<Node>(Node(nullptr, elem, nullptr))) {}

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
    BinTreeSmart left() const {
        if (empty()) {
            throw EmptyTreeException();
        }
        return BinTreeSmart(_root->_left);
    }

    /**
     * Returns right subtree. Fails if tree is empty.
     * Partial observer, O(1)
    */
    BinTreeSmart right() const {
        if (empty()) {
            throw EmptyTreeException();
        }
        return BinTreeSmart(_root->_right);
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
            Queue<Link> pending;
            pending.push_back(_root);

            while (!pending.empty()) {
                Link current = pending.front();
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
    // NOTE: no need for a copy ctor or assignment operator, as there is no destructor!
    // //

    /** Comparison operators. */
    bool operator==(const BinTreeSmart<T> &rhs) const {
        return compareAux(_root, rhs._root);
    }

    bool operator!=(const BinTreeSmart<T> &rhs) const {
        return !(*this == rhs);
    }

    /** 
     *  Output, adapted from "ADTs, DataStructures, and Problem Solving with C++", 
     *  Larry Nyhoff, Person, 2015
     */
    friend std::ostream& operator<<(std::ostream& o, const BinTreeSmart<T>& t){
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
    static BinTreeSmart<T> fromPreOrderInput(const T& emptyRep) {
        T elem;
        std::cin >> elem;
        if (elem == emptyRep)
            return BinTreeSmart<T>();
        else {
            BinTreeSmart<T> hi = fromPreOrderInput(emptyRep);
            BinTreeSmart<T> hd = fromPreOrderInput(emptyRep);
            return BinTreeSmart<T>(hi, elem, hd);
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
    static BinTreeSmart<T> fromInOrderInput() {
        char c;
        std::cin >> c;
        if (c == '.')
            return BinTreeSmart<T>(); 
        else {
            assert (c == '(');
            BinTreeSmart<T> left = fromInOrderInput();
            T elem;
            std::cin >> elem;
            BinTreeSmart<T> right = fromInOrderInput();
            std::cin >> c;
            assert (c == ')');
            BinTreeSmart<T> result(left, elem, right);
            return result;
        }
    }



protected:
    /** used to generate output */
    static const int TREE_INDENTATION = 4;

    /**
     * Internal node class
     */
    class Node; // Forward declaration so that next line compiles
    using Link = std::shared_ptr<Node>; // Type alias; avoids lots of typing

    class Node {
    public:
        Node() : _left(nullptr), _right(nullptr) {}
        Node(Link left, const T &elem, Link right) : 
            _elem(elem), _left(left), _right(right) {}

        T _elem;
        Link _left;
        Link _right;
    };

    /**
     * Protected constructor, which builds a tree from an existing root node.
     * Using std::shared_ptr takes care of references for us
     */
    BinTreeSmart(Link raiz) : _root(raiz) {}

    // //
    // AUX METHODS FOR TRAVERSAL
    // //
    
    static void preOrderAux(Link root, List<T> &acc) {
        if (root != nullptr){
            acc.push_back(root->_elem);
            preOrderAux(root->_left, acc);
            preOrderAux(root->_right, acc);
        }
    }

    static void inOrderAux(Link root, List<T> &acc) {
        if (root != nullptr) {
            inOrderAux(root->_left, acc);
            acc.push_back(root->_elem);
            inOrderAux(root->_right, acc);
        }
    }

    static void postOrderAux(Link root, List<T> &acc) {
        if (root != nullptr) {
            postOrderAux(root->_left, acc);
            postOrderAux(root->_right, acc);
            acc.push_back(root->_elem);
        }
    }

    static void outputIndented(std::ostream & out, int indent, Link root){
        if (root != nullptr) {
            outputIndented(out, indent + TREE_INDENTATION, root->_right);
            out << std::setw(indent) << " " << root->_elem << std::endl;
            outputIndented(out, indent + TREE_INDENTATION, root->_left);
        }
    }

    // //
    // OTHER AUX METHODS
    // //

    static unsigned int nodeCountAux(Link root) {
        if (root == nullptr) {
            return 0;
        }
        return 1 + nodeCountAux(root->_left) + nodeCountAux(root->_right);
    }

    static unsigned int depthAux(Link root) {
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

    static unsigned int leafCountAux(Link root) {
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
     * Compares two nodes and their children for equality
     */
    static bool compareAux(Link r1, Link r2) {
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
    Link _root;
};

#endif // __BINTREE_SMART_H
