# ed2223
Code for the 2022-23 edition of Data Structures (group I) at Facultad de Informática UCM

## Contents

Documentation is in **doc**.

Source-code for official adts is in **adts**, where

- all depend on [Exceptions.h](https://github.com/manuel-freire/ed2223/blob/main/adts/Exceptions.h) being available, as sole dependency (so far).

- Sequential ADTs include some base containers (single and doubly-linked lists), and derived **Stack** and **Queue** ADTs built on such containers. 

    - [List.h](https://github.com/manuel-freire/ed2223/blob/main/adts/List.h) is a doubly-linked list, with iterators.
    - [ListSingle.h](https://github.com/manuel-freire/ed2223/blob/main/adts/ListSingle.h) is a singly-linked list, with no iterators, also usable as a stack.

    - [Stack.h](https://github.com/manuel-freire/ed2223/blob/main/adts/Stack.h) is backed by a dynamic array
    - [LinkedListStack.h](https://github.com/manuel-freire/ed2223/blob/main/adts/LinkedListStack.h) is backed by a singly-linked list w/phantom node

    - [Queue.h](https://github.com/manuel-freire/ed2223/blob/main/adts/Queue.h) is backed by a doubly-linked list

- Recursive ADTs include two versions of a binary tree using reference-counting to allow multiple trees to share internal nodes. 

    - [BinTree.h](https://github.com/manuel-freire/ed2223/blob/main/adts/BinTree.h) uses manual reference counting
    - [BinTreeSmart.h](https://github.com/manuel-freire/ed2223/blob/main/adts/BinTreeSmart.h) is identical, but relies instead on C++ smart shared pointers (which wrap pointer with some reference-counting logic).
  
