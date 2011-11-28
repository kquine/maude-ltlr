/*
 * ProductAutomaton.cc
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

//	utility stuff
#include "macros.hh"
#include "vector.hh"

//	forward declarations
#include "interface.hh"
#include "core.hh"
#include "higher.hh"
#include "freeTheory.hh"
#include "temporal.hh"

//	interface class definitions
#include "buchiAutomaton2.hh"
#include "genBuchiAutomaton.hh"

//	automaton definitions
#include "Util/PtrStack.hh"
#include "SystemAutomaton.hh"
#include "SESystemAutomaton.hh"
#include "ProductAutomaton.hh"

namespace ltlrModelChecker {

template <typename PA>
ProductAutomaton<PA>::ProductAutomaton(SystemGraph& system, const PA& property):
	system(system), property(property)
{
	FOR_EACH_CONST(i, NatSet, property.getInitialStates())
		initialStates.append(State(0,*i));
}

template <typename PA> typename ProductAutomaton<PA>::TransitionIterator*
ProductAutomaton<PA>::makeTransitionIterator(const State& state)
{
	TransitionIterator* itor = new TransitionIterator(this, state);
	itor->init();
	return itor;
}

template <typename PA> const Vector<typename ProductAutomaton<PA>::State>&
ProductAutomaton<PA>::getInitialStates() const
{
	return initialStates;
}


template <typename PA> inline SystemGraph&
ProductAutomaton<PA>::getSystemAutomaton() const
{
	return system;
}

template <typename PA> inline const PA&
ProductAutomaton<PA>::getPropertyAutomaton() const
{
	return property;
}


namespace product
{
	//
	// specialization for transitions of BuchiAutomaton
	//
	template <>
	struct PropertyTransitionAdaptor<BuchiAutomaton2>
	{
		typedef BuchiAutomaton2::Transition		Transition;
		typedef BuchiAutomaton2::TransitionMap	TransitionSet;
		static int getNextState(const Transition& t)		{ return t.first; }
		static const Bdd& getFormula(const Transition&t)	{ return t.second; }
	};

	//
	// specialization for transitions of GenBuchiAutomaton
	//
	template <>
	struct PropertyTransitionAdaptor<GenBuchiAutomaton>
	{
		typedef GenBuchiAutomaton::FairTransition		Transition;
		typedef GenBuchiAutomaton::FairTransitionSet	TransitionSet;
		static int getNextState(const Transition& t)		{ return t.first.first; }
		static const Bdd& getFormula(const Transition&t)	{ return t.second; }
	};


	template <typename PA>
	TransitionIterator<PA>::TransitionIterator(ProductAutomaton<PA>* parent, const State& state)
	{
		tr.source = state;
		sysGraph = &parent->getSystemAutomaton();
		ts = &parent->getPropertyAutomaton().getTransitions(state.property);
		// NOTE: computeNextTransition cannot be called here since it is virtual.
	}

	template <typename PA> inline FastBool
	TransitionIterator<PA>::hasNext() const
	{
		return tr.systemIndex != NONE;
	}

	template <typename PA> inline const Transition<PA>&
	TransitionIterator<PA>::pick() const
	{
		return tr;
	}

	template <typename PA> inline void
	TransitionIterator<PA>::init()
	{
		computeNextTransition(true);
	}

	template <typename PA> inline void
	TransitionIterator<PA>::next()
	{
		computeNextTransition(false);
	}

	template <typename PA> inline const State&
	TransitionIterator<PA>::getSource() const
	{
		return tr.source;
	}

	template <typename PA> void
	TransitionIterator<PA>::computeNextTransition(FastBool first)
	{
		switch(first)
		{
		case 1:
			for (tr.propertyIndex = ts->begin(); tr.propertyIndex != ts->end(); ++tr.propertyIndex)
			{
				if (sysGraph->satisfiesFormula(PropertyTransitionAdaptor<PA>::getFormula(*tr.propertyIndex), tr.source.system))
				{
					for (tr.systemIndex = 0; ; ++tr.systemIndex)
					{
						tr.target.system = sysGraph->getNextState(tr.source.system, tr.systemIndex);
						if (tr.target.system == NONE)
							break;
						tr.target.property = PropertyTransitionAdaptor<PA>::getNextState(*tr.propertyIndex);
						return;
		default:;
					}
				}
			}
		}
		tr.systemIndex = NONE;
		tr.target.system = NONE;
		tr.target.property = NONE;
	}
}

//
// test code
//

template <typename PA> void
ProductAutomaton<PA>::dump(ostream& o)
{
	int nrStates = 0;
	int nrTransitions = 0;

	PtrStack<TransitionIterator> stk;
	Vector<NatSet> visited;
	visited.expandBy(1);

	const Vector<State>& initials = getInitialStates();
	for (int i = initials.size() - 1; i >= 0; i--)
	{
		++nrStates;
		stk.push(makeTransitionIterator(initials[i]));

		visited[0].insert(initials[i].property);
		o << "initial visit: " << initials[i] << " with the first next state: " << stk.top()->pick().target << endl;
	}

	while(!stk.empty())
	{
		if(stk.top()->hasNext())
		{
			Transition t = stk.top()->pick();
			cout << "pick a transition: " << stk.top()->getSource() << " -> " << t.target << endl;

			stk.top()->next();
			++nrTransitions;
			if (t.target.system >= visited.length())
				visited.expandTo(t.target.system + 1);

			if (visited[t.target.system].contains(t.target.property))
			{
				// skip
			}
			else
			{
				visited[t.target.system].insert(t.target.property);
				stk.push(makeTransitionIterator(t.target));

				++nrStates;
				o << "visit: " << t.target << "  with the first next state: " <<  stk.top()->pick().target << endl;
			}
		}
		else
		{
			o << "-------------------pop" << endl;
			stk.pop();
		}
	}
	o << "visited state : " << nrStates << ", visited transition: " << nrTransitions << endl;

	int totalTr = 0;
	for (int i = 0; i < system.getNrStates(); ++i)
		totalTr += system.getNrTransitions(i);

	o << "total system state: " << system.getNrStates() << ", total system transition: " << totalTr << endl;
}


}
