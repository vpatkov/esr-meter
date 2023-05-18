/*
 * Stuff for working with shared and volatile objects
 *
 * Shared object is an object that is shared across different threads of
 * execution. Such an object (if is not a constant) should be accessed in an
 * exclusive way. Note that 'volatile' keyword of C/C++ have nothing to do with
 * exclusive access.
 *
 * Volatile object is an object that is accessed with side effects (for example,
 * memory mapped I/O). Accesses to volatile objects should not be reordered or
 * optimized out. 'volatile' keyword is the standard C/C++ tool to guarantee
 * that, but it works by suppressing optimization, which is almost never we
 * actually want. So usually it's better to not use 'volatile' keyword at all,
 * but use explicit memory barriers instead.
 */

#ifndef VOLATILE_HPP_
#define VOLATILE_HPP_

#ifdef __AVR_ARCH__
#  include <util/atomic.h>
#  define ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
#  error "Not supported architecture"
#endif

inline void memory_barrier() {
	asm volatile ("" ::: "memory");
}

template<typename T>
inline T atomic_read(const T &x) {
	if (sizeof(x) == 1)
		return x;
	else {
		T t;
		ATOMIC { t = x; }
		return t;
	}
}

template<typename T>
inline void atomic_write(T &x, decltype(T{}) val) {
	if (sizeof(x) == 1)
		x = val;
	else {
		ATOMIC { x = val; }
	}
}

#endif
