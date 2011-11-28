/*
 * BFSGraph.hh
 *
 *  Created on: Nov 29, 2010
 *      Author: kquine
 */

#ifndef BFSGRAPH_HH_
#define BFSGRAPH_HH_
#include <queue>
#include <tr1/unordered_map>
#include "macros.hh"
#include "Automaton/ProductAutomaton.hh"
#include "StateMap.hh"

namespace ltlrModelChecker {

//
//	Used for short counter example generation (by breadth first search).
//
template <typename _PropertyAutomaton>
class BFSGraph
{
public:
	typedef typename ProductAutomaton<_PropertyAutomaton>::State				State;
	typedef typename ProductAutomaton<_PropertyAutomaton>::Transition			Transition;
	typedef typename ProductAutomaton<_PropertyAutomaton>::TransitionIterator	TransitionIterator;

	BFSGraph(ProductAutomaton<_PropertyAutomaton>& graph, const Vector<State>& initials);
	virtual ~BFSGraph() {}

	virtual FastBool inDomain(const State& s) const = 0;
	virtual FastBool isTarget(const State& t) const = 0;
	virtual FastBool isTarget(const Transition& t) = 0;
	State doBFS(list<pair<int,int> >& path);

protected:
	ProductAutomaton<_PropertyAutomaton>& graph;

private:
	struct Step;		// used for counter-example generation

	queue<State> toVisit;
	StateMap<Step> parent;	// parent state + transition index
	const Vector<State>& initials;
};


}

#include "BFSGraph.cc"

#endif /* BFSGRAPH_HH_ */
