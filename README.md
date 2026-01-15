# ed

Code for the 2025-26 edition of Data Structures (group I) at Facultad de Informática UCM.

These collections are similar (in API) to those of the standard C++ library, but their implementation is meant to be (much) easier to follow. 

## Contents

Documentation is in **doc**.

Source-code for official adts is in **adts**, where

- all depend on [Exceptions.h](https://github.com/manuel-freire/ed/blob/main/adts/Exceptions.h) being available, as sole dependency (so far).

- **Sequential** ADTs include some base containers (single and doubly-linked lists), and derived **Stack** and **Queue** ADTs built on such containers. 

    - [Stack.h](https://github.com/manuel-freire/ed/blob/main/adts/Stack.h) is a **stack** backed by a dynamic array
    - [LinkedListStack.h](https://github.com/manuel-freire/ed/blob/main/adts/LinkedListStack.h) is another **stack** backed by a singly-linked list with a phantom node
    - [Queue.h](https://github.com/manuel-freire/ed/blob/main/adts/Queue.h) is a **queue** backed by a doubly-linked list  
    - [ListSingle.h](https://github.com/manuel-freire/ed/blob/main/adts/ListSingle.h) is a singly-linked list, also usable as a stack
    - [List.h](https://github.com/manuel-freire/ed/blob/main/adts/List.h) is a doubly-linked list, and is the only sequential ADT (in this library) with iterators.

- **Branching** ADTs include two versions of an (unbalanced) binary tree using reference-counting to allow multiple trees to share internal nodes. All have iterators.

    - [BinTree.h](https://github.com/manuel-freire/ed/blob/main/adts/BinTree.h) uses manual reference counting
    - [BinTreeSmart.h](https://github.com/manuel-freire/ed/blob/main/adts/BinTreeSmart.h) is identical, but relies instead on C++ smart shared pointers (which wrap pointers with some reference-counting logic).

- **Associative** ADTs include both a (non-balanced) TreeMap and a HashMap; and a TreeSet that is very similar to the TreeMap in its implementation

    - [TreeSet.h](https://github.com/manuel-freire/ed/blob/main/adts/TreeSet.h) is nice to de-duplicate and sort collections.
    - [TreeMap.h](https://github.com/manuel-freire/ed/blob/main/adts/TreeMap.h) provides an efficient key-value store that only requires keys to implement a less-than operator.
    - [HashMap.h](https://github.com/manuel-freire/ed/blob/main/adts/HashMap.h) uses the hash functions implemented in [Hash.h](https://github.com/manuel-freire/ed2223/blob/main/adts/Hash.h) to provide O(1) lookups.
