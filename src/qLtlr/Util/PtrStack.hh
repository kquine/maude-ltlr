/*
 * PtrStack.hh
 *
 *  Created on: Dec 5, 2010
 *      Author: kquine
 */

#ifndef PTRSTACK_HH_
#define PTRSTACK_HH_
#include <stack>

namespace ltlrModelChecker {

template<typename T>
class PtrStack
{
	NO_COPYING(PtrStack);	// copying is not allowed
public:
	PtrStack()				{ }
	~PtrStack()				{ while (!stk.empty()) pop(); }
	bool empty() const		{ return stk.empty(); }
	void pop()				{ delete stk.top(); stk.pop(); }
	void push(T* x)			{ stk.push(x); }
	size_t size() const		{ return stk.size(); }
	T* top()				{ return stk.top(); }
	const T* top() const	{ return stk.top(); }
private:
	stack<T*> stk;
};

}

#endif /* PTRSTACK_HH_ */
