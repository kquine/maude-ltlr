/*
 * SEProductAutomaton.hh
 *
 *  Created on: Dec 2, 2010
 *      Author: kquine
 */

#ifndef SEPRODUCTAUTOMATON_HH_
#define SEPRODUCTAUTOMATON_HH_
#include "SystemEventGraph.hh"
#include "ProductAutomaton.hh"

namespace ltlrModelChecker {

// forward declarations
namespace product
{
	template <typename _PropertyAutomaton> struct SETransitionIterator;
}

template <typename _PropertyAutomaton>
class SEProductAutomaton: public ProductAutomaton<_PropertyAutomaton>
{
	friend class product::SETransitionIterator<_PropertyAutomaton>;
public:
	typedef product::State										State;
	typedef product::Transition<_PropertyAutomaton>				Transition;
	typedef product::SETransitionIterator<_PropertyAutomaton>	SETransitionIterator;

	SEProductAutomaton(SystemEventGraph& system, const _PropertyAutomaton& property);
	SETransitionIterator* makeTransitionIterator(const State& state);
};

namespace product
{

	//
	//	Product transition iterator (state/event)
	//
	template <typename _PropertyAutomaton>
	class SETransitionIterator: public TransitionIterator<_PropertyAutomaton>
	{
		SETransitionIterator(SEProductAutomaton<_PropertyAutomaton>* parent, const State& state);
		void computeNextTransition(FastBool first);
		Bdd label;
		friend class SEProductAutomaton<_PropertyAutomaton>;
	};
}

}

#include "SEProductAutomaton.cc"

#endif /* SEPRODUCTAUTOMATON_HH_ */
