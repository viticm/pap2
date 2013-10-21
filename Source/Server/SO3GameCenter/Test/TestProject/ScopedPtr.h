#pragma  once

#include <cstddef>            // for std::ptrdiff_t
#include <assert.h>           // for assert
#include <stdlib.h>           // for free() decl

#ifdef _WIN32
namespace std { using ::ptrdiff_t; };
#endif // _WIN32



template <typename T>
class ScopedPtr {
private:

	T* ptr;

	ScopedPtr(ScopedPtr const &);
	ScopedPtr & operator=(ScopedPtr const &);

public:

	typedef T element_type;

	explicit ScopedPtr(T* p = 0): ptr(p) {}

	~ScopedPtr() {
		typedef char type_must_be_complete[sizeof(T)];
		delete ptr;
	}

	void reset(T* p = 0) {
		typedef char type_must_be_complete[sizeof(T)];

		if (ptr != p) {
			delete ptr;
			ptr = p;
		}
	}

	T& operator*() const {
		assert(ptr != 0);
		return *ptr;
	}

	T* operator->() const  {
		assert(ptr != 0);
		return ptr;
	}

	T* get() const  {
		return ptr;
	}

	void swap(ScopedPtr & b) {
		T* tmp = b.ptr;
		b.ptr = ptr;
		ptr = tmp;
	}

	T* release() {
		T* tmp = ptr;
		ptr = 0;
		return tmp;
	}

	T** accept() {
		if (ptr) {
			delete ptr;
			ptr = 0;
		}
		return &ptr;
	}

	T** use() {
		return &ptr;
	}
};

template<typename T> inline
void swap(ScopedPtr<T>& a, ScopedPtr<T>& b) {
	a.swap(b);
}




//  scoped_array extends ScopedPtr to arrays. Deletion of the array pointed to
//  is guaranteed, either on destruction of the scoped_array or via an explicit
//  reset(). Use shared_array or std::vector if your needs are more complex.

template<typename T>
class scoped_array {
private:

	T* ptr;

	scoped_array(scoped_array const &);
	scoped_array & operator=(scoped_array const &);

public:

	typedef T element_type;

	explicit scoped_array(T* p = 0) : ptr(p) {}

	~scoped_array() {
		typedef char type_must_be_complete[sizeof(T)];
		delete[] ptr;
	}

	void reset(T* p = 0) {
		typedef char type_must_be_complete[sizeof(T)];

		if (ptr != p) {
			delete [] ptr;
			ptr = p;
		}
	}

	T& operator[](std::ptrdiff_t i) const {
		assert(ptr != 0);
		assert(i >= 0);
		return ptr[i];
	}

	T* get() const {
		return ptr;
	}

	void swap(scoped_array & b) {
		T* tmp = b.ptr;
		b.ptr = ptr;
		ptr = tmp;
	}

	T* release() {
		T* tmp = ptr;
		ptr = 0;
		return tmp;
	}

	T** accept() {
		if (ptr) {
			delete [] ptr;
			ptr = 0;
		}
		return &ptr;
	}
};

template<class T> inline
void swap(scoped_array<T>& a, scoped_array<T>& b) {
	a.swap(b);
}

// ScopedPtrMalloc<> is similar to ScopedPtr<>, but it accepts a
// second template argument, the function used to free the object.

template<typename T, void (*FF)(void*) = free> class ScopedPtrMalloc {
private:

	T* ptr;

	ScopedPtrMalloc(ScopedPtrMalloc const &);
	ScopedPtrMalloc & operator=(ScopedPtrMalloc const &);

public:

	typedef T element_type;

	explicit ScopedPtrMalloc(T* p = 0): ptr(p) {}

	~ScopedPtrMalloc() {
		typedef char type_must_be_complete[sizeof(T)];
		FF(static_cast<void*>(ptr));
	}

	void reset(T* p = 0) {
		typedef char type_must_be_complete[sizeof(T)];

		if (ptr != p) {
			FF(static_cast<void*>(ptr));
			ptr = p;
		}
	}

	T& operator*() const {
		assert(ptr != 0);
		return *ptr;
	}

	T* operator->() const {
		assert(ptr != 0);
		return ptr;
	}

	T* get() const {
		return ptr;
	}

	void swap(ScopedPtrMalloc & b) {
		T* tmp = b.ptr;
		b.ptr = ptr;
		ptr = tmp;
	}

	T* release() {
		T* tmp = ptr;
		ptr = 0;
		return tmp;
	}

	T** accept() {
		if (ptr) {
			FF(static_cast<void*>(ptr));
			ptr = 0;
		}
		return &ptr;
	}
};

template<typename T, void (*FF)(void*)> inline
void swap(ScopedPtrMalloc<T,FF>& a, ScopedPtrMalloc<T,FF>& b) {
	a.swap(b);
}
