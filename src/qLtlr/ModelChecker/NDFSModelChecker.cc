/*
 * NDFSModelChecker.cc
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"

//      utility class definitions
#include "natSet.hh"
#include "BFSGraph.hh"
#include "Util/PtrStack.hh"

#include "NDFSModelChecker.hh"

namespace ltlrModelChecker {


class NDFSModelChecker::PrefixBFSGraph : public BFSGraph<BuchiAutomaton2>
{
public:
	PrefixBFSGraph(Automaton& prod, const PtrVector<StateSet>& stateMap, const State& cycleState):
		BFSGraph<BuchiAutomaton2>(prod, prod.getInitialStates()), stateMap(stateMap), cycleState(cycleState) {}

	FastBool isTarget(const State& s) const
	{
		return s == cycleState;
	}
	FastBool isTarget(const Transition& t)
	{
		return isTarget(t.target);
	}
	FastBool inDomain(const State& s) const
	{
		// We do not further search states which have not been visited during DFS...
		if (s.system < stateMap.size())
			return stateMap[s.system]->dfs1Seen.contains(s.property);
		else
			return false;
	}

private:
	const State cycleState;
	const PtrVector<StateSet>& stateMap;
};


NDFSModelChecker::NDFSModelChecker(Automaton& prod): prod(prod) {}

bool
NDFSModelChecker::findCounterExample()
{
	intersectionStates.expandTo(1);
	bool result = false;

	FOR_EACH_CONST(i, Vector<State>, prod.getInitialStates())
	{
		if ( dfs1(*i) )
		{
			result = true;
			break;
		}
	}

	// counter example found
	if (result)
	{
		//
		//	Shorten prefix-path using BFS if possible
		//
		leadIn.clear();
		PrefixBFSGraph bfs(prod, intersectionStates, cycleState);
		bfs.doBFS(leadIn);
	}
	return result;
}



bool
NDFSModelChecker::dfs1(const State& initial)
{
	PtrStack<TransitionIterator> dfs1;
	intersectionStates[initial.system]->dfs1Seen.insert(initial.property);
	intersectionStates[initial.system]->onDfs1Stack.insert(initial.property);
	leadIn.push_back(make_pair(NONE,NONE));	// dummy node to make "leadIn" have the same size with dfs1 stack.
	dfs1.push(prod.makeTransitionIterator(initial));

	while (!dfs1.empty())
	{
		if (dfs1.top()->hasNext())
		{
			State ns = dfs1.top()->pick().target;
			int sysIndex = dfs1.top()->pick().systemIndex;
			dfs1.top()->next();

			int stateSize = intersectionStates.size();
			if ( ns.system < stateSize )
			{
				if (intersectionStates[ns.system]->dfs1Seen.contains(ns.property))
					continue;	// skip already visited
			}
			else
				intersectionStates.expandTo(ns.system + 1);

			// if not visited yet
			intersectionStates[ns.system]->dfs1Seen.insert(ns.property);
			intersectionStates[ns.system]->onDfs1Stack.insert(ns.property);
			leadIn.push_back(make_pair(dfs1.top()->getSource().system, sysIndex));
			dfs1.push(prod.makeTransitionIterator(ns));
		}
		else	// pop
		{
			const State& cur = dfs1.top()->getSource();
			if (prod.getPropertyAutomaton().isAccepting(cur.property))	// if accepting state
			{
				if (dfs2(cur))
				{
					//
					// complete the list "cycle" for the counterexample
					//
					while (dfs1.top()->getSource() != cycleState)
					{
						cycle.push_front(leadIn.back());
						leadIn.pop_back();
						dfs1.pop();
					}
					leadIn.pop_front();		// remove dummy
					return true;
				}
			}
			intersectionStates[cur.system]->onDfs1Stack.subtract(cur.property);
			dfs1.pop();
			leadIn.pop_back();
		}
	}
	return false;
}

local_inline bool
NDFSModelChecker::dfs2(const State& initial)
{
	PtrStack<TransitionIterator> dfs2;
	intersectionStates[initial.system]->dfs2Seen.insert(initial.property);
	cycle.push_back(make_pair(NONE,NONE));	// dummy node to make "cycle" have the same size with dfs2 stack.
	dfs2.push(prod.makeTransitionIterator(initial));

	while (!dfs2.empty())
	{
		if (dfs2.top()->hasNext())
		{
			State ns = dfs2.top()->pick().target;
			int sysIndex = dfs2.top()->pick().systemIndex;
			dfs2.top()->next();

			Assert(ns.system < intersectionStates.size(), "visited system state for the first time on dfs2");
			StateSet* sset = intersectionStates[ns.system];
			if ( sset->onDfs1Stack.contains(ns.property))	// found an accepted cycle
			{
				cycleState = ns;
				cycle.push_back(make_pair(dfs2.top()->getSource().system, sysIndex));
				cycle.pop_front();		// remove dummy
				return true;
			}
			if ( !sset->dfs2Seen.contains(ns.property))	// not visited yet during DFS2
			{
				sset->dfs2Seen.insert(ns.property);
				cycle.push_back(make_pair(dfs2.top()->getSource().system, sysIndex));
				dfs2.push(prod.makeTransitionIterator(ns));
			}
			// skip
		}
		else
		{
			dfs2.pop();
			cycle.pop_back();
		}
	}
	return false;
}

}
