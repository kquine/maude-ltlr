/*
 * BFSGraph.cc
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

#include "BFSGraph.hh"

namespace ltlrModelChecker {

template <class PA>
struct BFSGraph<PA>::Step		// used for counter-example generation
{
	Step() : systemIndex(NONE) {}
	Step(const State& p, int index) : parent(p), systemIndex(index) {}
	State parent;
	int systemIndex;
};

template <class PA>
BFSGraph<PA>::BFSGraph(ProductAutomaton<PA>& graph, const Vector<State>& initials):
	graph(graph), initials(initials) {}

template <class PA> typename BFSGraph<PA>::State
BFSGraph<PA>::doBFS(list<pair<int,int> >& path)
{
	list<pair<int,int> > temp_path;

	FOR_EACH_CONST(i, typename Vector<State>, initials)
	{
		if (isTarget(*i))	// if initial state is a target state
			return *i;
		else if (inDomain(*i))
		{
			toVisit.push(*i);
			parent.expand(*i);
			parent.set(*i,Step());
		}
	}
	Assert(!(toVisit.empty()), "no initial states in domain.");

	for (;;)
	{
		Assert(!(toVisit.empty()), "toVisit empty");
		State s = toVisit.front();
		toVisit.pop();

		const auto_ptr<TransitionIterator> ts(graph.makeTransitionIterator(s));
		while (ts->hasNext())
		{
			Transition t = ts->pick();
			ts->next();

			if (inDomain(t.target))		// try to visit a state only if it is in some domain..
			{
				if (isTarget(t))	// if we found a path to the target, generate a path and return the target state.
				{
					temp_path.push_front(make_pair(s.system, t.systemIndex));
					for(;;)
					{
						const Step &c = parent.get(s);		// always find such c in parent since we use queue.
						if (c.systemIndex == NONE)	// if initial states
							break;
						temp_path.push_front(make_pair(c.parent.system, c.systemIndex));
						s = c.parent;
					}
					path.splice(path.end(), temp_path);	// append to the end of path..
					return t.target;
				}
				else
				{
					if (parent.expand(t.target) || !parent.contains(t.target))	// if not already visited
					{
						parent.set(t.target, Step(s, t.systemIndex));
						toVisit.push(t.target);
					}
				}
			}
		}
	}
}

}
