/*
 * Exceptions used in ADTs
 * (c) Marco Antonio Gómez Martín, 2012
 * English & formatting by Manuel Freire, 2023
 */

#ifndef __EXCEPTIONS_H
#define __EXCEPTIONS_H

#include <string>
#include <iosfwd>

/** 
 * Exceptions all inherit from this. Provides a _msg field for error messages.
 */
class ADTException {
public:
    ADTException() {}
    ADTException(const std::string &msg) : _msg(msg) {}

    const std::string msg() const { return _msg; }

    friend std::ostream &operator<<(std::ostream &out, const ADTException &e);

protected:
    std::string _msg;
};

inline std::ostream &operator<<(std::ostream &out, const ADTException &e) {
    out << e._msg;
    return out;
}

/*
 * Exception-declaring macro. 
 * Creates a new exception class inheriting from ExceptionTAD
 * Avoids writing boilerplate over and over again...
 */


#define DECLARE_EXCEPTION(Exception) \
class Exception : public ADTException { \
public: \
Exception() {}; \
Exception(const std::string &msg) : ADTException(msg) {} \
};

/** Tried to operate on an empty stack. */
DECLARE_EXCEPTION(EmptyStackException);

/** Tried to fill a stack beyond its capacity. Only thrown in fixed-size stacks. */
DECLARE_EXCEPTION(FullStackException);

/** Tried to operate on an empty queue. */
DECLARE_EXCEPTION(EmptyQueueException);

/** Tried to operate on an empty double-ended queue. */
DECLARE_EXCEPTION(EmptyDequeException);

/** Tried to operate on an empty list. */
DECLARE_EXCEPTION(EmptyListException);

/** Tried to access an invalid element, or bad iterator use. */
DECLARE_EXCEPTION(InvalidAccessException);

/** Unexpectedly empty tree encountered in binary tree. */
DECLARE_EXCEPTION(EmptyTreeException);

/** Invalid key passed to certain tree and hashmap operations. */
DECLARE_EXCEPTION(BadKeyException);

#endif // __EXCEPTIONS_H
