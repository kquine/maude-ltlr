/*
 * PtrVector.hh
 *
 *  Created on: Dec 10, 2010
 *      Author: kquine
 */

#ifndef PTRVECTOR_HH_
#define PTRVECTOR_HH_
#include "macros.hh"
#include "vector.hh"

namespace ltlrModelChecker {

template <typename T>
class PtrVector
{
	NO_COPYING(PtrVector);	// copying is not allowed.

public:
	typedef typename Vector<T*>::iterator			iterator;
	typedef typename Vector<T*>::const_iterator		const_iterator;
	typedef typename Vector<T*>::size_type			size_type;

	PtrVector()								{ }
	virtual ~PtrVector()					{ clear(); }

	iterator begin()						{ return vec.begin(); }
	iterator end()							{ return vec.end(); }
	const_iterator begin() const			{ return vec.begin(); }
	const_iterator end() const				{ return vec.end(); }

	const T* operator[](size_type i) const	{ return vec[i]; }
	T* operator[](size_type i)				{ return vec[i]; }
	void expandTo(size_type newLength);
	void contractTo(size_type newLength);
	void append(T* item)					{ vec.append(item); }
	size_type size() const					{ return vec.size(); }
	bool empty() const						{ return vec.empty(); }
	void clear();

private:
	Vector<T*> vec;
};

template <typename T> void
PtrVector<T>::expandTo(size_type newLength)
{
	int old = vec.size();
	vec.expandTo(newLength);
	for (int i = old; i < newLength; ++i)
		vec[i] = new T;
}

template <typename T> void
PtrVector<T>::contractTo(size_type newLength)
{
	for (int i = newLength; i < vec.size(); ++i )
		delete vec[i];
	vec.contractTo(newLength);
}

template <typename T> void
PtrVector<T>::clear()
{
	FOR_EACH_CONST(i, typename Vector<T*>, vec)
		delete (*i);
}



}

#endif /* PTRVECTOR_HH_ */
