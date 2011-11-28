/*
 * SCCModelChecker.cc
 *
 *  Created on: Dec 7, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"

//      utility class definitions
#include "natSet.hh"
#include "Util/PtrStack.hh"

#include "SCCModelChecker.hh"

namespace ltlrModelChecker {


struct SCCModelChecker::PrefixBFSGraph: public SCCBFSGraph
{
	PrefixBFSGraph(SCCModelChecker* mc, const StateMap<Uint>& H, int root):
		SCCBFSGraph(mc,H,root,mc->prod.getInitialStates()) {}
	FastBool inDomain(const State& s) const	{ return !map.invalid(s) && map.contains(s); }
	FastBool isTarget(const State& s) const	{ return inDomain(s) && map.get(s) >= root; }
	FastBool isTarget(const Transition& t)	{ return isTarget(t.target); }
};

class SCCModelChecker::CycleCompBFSGraph: public SCCBFSGraph
{
	const State des;
	Vector<State> initial;
public:
	CycleCompBFSGraph(SCCModelChecker* mc, const StateMap<Uint>& H, int root, const State& start, const State& des):
		SCCBFSGraph(mc,H,root,initial), des(des) { initial.append(start); }
	FastBool inDomain(const State& s) const	{ return !map.invalid(s) && map.contains(s) && map.get(s) >= root; }
	FastBool isTarget(const State& s) const	{ return s == des; }
	FastBool isTarget(const Transition& t)	{ return isTarget(t.target); }
};

class SCCModelChecker::CycleBFSGraph: public SCCBFSGraph
{
	FairSet::Goal* goal;
	Vector<State> initial;
public:
	CycleBFSGraph(SCCModelChecker* mc, const StateMap<Uint>& H, int root, const State& start, FairSet::Goal* goal):
		SCCBFSGraph(mc,H,root,initial), goal(goal) { initial.append(start); }
	FastBool inDomain(const State& s) const	{ return !map.invalid(s) && map.contains(s) && map.get(s) >= root; }
	FastBool isTarget(const State& s) const	{ return false; }
	FastBool isTarget(const Transition& t)
	{
		auto_ptr<FairSet> fs(mc->fairMap.makeFairSet(t));
		return goal->empty() ? true : mc->fairMap.updateFairGoal(goal, fs.get());
	}
};


SCCModelChecker::SCCModelChecker(Automaton& prod, FairnessMap& fm): prod(prod), fairMap(fm), max(0) {}

bool
SCCModelChecker::findCounterExample()
{
	auto_ptr<SCC> res(findAcceptedSCC(prod.getInitialStates()));;

	if (res.get() != NULL)
	{
		auto_ptr<FairSet::Goal> goal(res->acc_fair->makeFairGoal());
		//
		// prefix
		//
		PrefixBFSGraph prefix_bfs(this, H, res->root);
		const State& root = prefix_bfs.doBFS(leadIn);
		//
		// cycle
		//
		State s = root;
		do
		{
			CycleBFSGraph cycle_bfs(this, H, res->root, s, goal.get());
			s = cycle_bfs.doBFS(cycle);
		} while( !goal->empty());
		//
		// cycle completion
		//
		if (s != root)
		{
			CycleCompBFSGraph comp_bfs(this, H, res->root, s, root);
			comp_bfs.doBFS(cycle);
		}
		return true;
	}
	return false;
}

SCCModelChecker::SCCStack::SCCStack(SCCModelChecker* mc): mc(mc) {}

FastBool
SCCModelChecker::SCCStack::empty() const
{
	return sccStack.empty();
}

FastBool
SCCModelChecker::SCCStack::hasNextSucc()
{
	while ( !dfsStack.empty() &&
			sccStack.top()->root <= mc->H.get(dfsStack.top()->getSource()) &&
			!dfsStack.top()->hasNext())
		dfsPop();

	return  !dfsStack.empty() && sccStack.top()->root <= mc->H.get(dfsStack.top()->getSource());
}

const SCCModelChecker::Transition&
SCCModelChecker::SCCStack::pickSucc() const
{
	return dfsStack.top()->pick();
}

void
SCCModelChecker::SCCStack::nextSucc()
{
	dfsStack.top()->next();
}

SCCModelChecker::SCC*
SCCModelChecker::SCCStack::topSCC()
{
	return sccStack.top();
}

void
SCCModelChecker::SCCStack::dfsPush(const State& s, auto_ptr<FairSet>& a)
{
	mc->H.set(s, ++mc->max);
	sccStack.push(new SCC(mc->max, a));
	dfsStack.push(mc->prod.makeTransitionIterator(s));
}

void
SCCModelChecker::SCCStack::dfsPop()
{
	stateStack.push(dfsStack.top()->getSource());	// keep states of the top SCC for later use
	dfsStack.pop();
}

SCCModelChecker::State
SCCModelChecker::SCCStack::sccPop(bool unvisit)
{
	Assert(mc->H.get(stateStack.top()) == sccStack.top()->root, "SCC Error (root index)");

	int top_root = sccStack.top()->root;
	State root = stateStack.top();
	if (unvisit)
	{
		while (!stateStack.empty() && mc->H.get(stateStack.top()) >= top_root )
		{
			mc->H.remove(stateStack.top());	// set unvisited.
			stateStack.pop();
		}
	}
	else
	{
		while (!stateStack.empty() && mc->H.get(stateStack.top()) >= top_root )
		{
			mc->H.set(stateStack.top(), 0);	// set removed. (H[state] = 0 iff the state is removed from the dfs stack)
			stateStack.pop();
		}
	}
	mc->max = top_root - 1;
	sccStack.pop();
	return root;
}

void
SCCModelChecker::SCCStack::merge(int threshold, auto_ptr<FairSet>& back)
{
	while ( threshold <  sccStack.top()->root )
	{
		if (sccStack.top()->acc_fair.get() != NULL)
			mc->fairMap.mergeFairSet(back.get(), sccStack.top()->acc_fair.get());
		if (sccStack.top()->incoming_fair.get() != NULL)
			mc->fairMap.mergeFairSet(back.get(), sccStack.top()->incoming_fair.get());
		sccStack.pop();
	}
	auto_ptr<FairSet>& top_acc = sccStack.top()->acc_fair;
	if (top_acc.get() != NULL)
		mc->fairMap.mergeFairSet(top_acc.get(), back.get());
	else
		top_acc.reset(back.release());
}

}
