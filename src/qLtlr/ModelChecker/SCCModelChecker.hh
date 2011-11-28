/*
 * SCCModelChecker.hh
 *
 *  Created on: Nov 29, 2010
 *      Author: kquine
 */

#ifndef SCCMODELCHECKER_HH_
#define SCCMODELCHECKER_HH_
#include "StateMap.hh"
#include "ModelChecker.hh"
#include "BFSGraph.hh"
#include "Fairness/FairnessMap.hh"
#include "Automaton/ProductAutomaton.hh"


namespace ltlrModelChecker
{

class SCCModelChecker: public ltlrModelChecker::ModelChecker
{
	NO_COPYING(SCCModelChecker);
public:
	typedef ProductAutomaton<GenBuchiAutomaton>	Automaton;
	typedef Automaton::State					State;
	typedef Automaton::Transition				Transition;
	typedef Automaton::TransitionIterator		TransitionIterator;

	SCCModelChecker(Automaton& prod, FairnessMap& fm);
	bool findCounterExample();

protected:
	//
	//	scc = (root index, incoming fair set, scc acceptance fair set)
	//
	struct SCC
	{
		Uint root;
		auto_ptr<FairSet> incoming_fair;
		auto_ptr<FairSet> acc_fair;

		SCC(int root, auto_ptr<FairSet>& incoming): root(root), incoming_fair(incoming) {}
	};
	class SCCStack
	{
	public:
		SCCStack(SCCModelChecker* mc);

		FastBool empty() const;
		SCC* topSCC();

		FastBool hasNextSucc();
		void nextSucc();
		const Transition& pickSucc() const;

		State sccPop(bool unvisit = false);		// pop and return a root
		void dfsPush(const State& s, auto_ptr<FairSet>& a);
		void merge(int threshold, auto_ptr<FairSet>& back);

	private:
		void dfsPop();

		PtrStack<SCC> sccStack;
		PtrStack<TransitionIterator> dfsStack;
		stack<State> stateStack;
		SCCModelChecker* mc;
	};

	virtual SCC* findAcceptedSCC(const Vector<State>& initials) = 0;

	int max;			// dfs index
	StateMap<Uint> H;
	FairnessMap& fairMap;

private:
	class SCCBFSGraph;
	class PrefixBFSGraph;
	class CycleBFSGraph;
	class CycleCompBFSGraph;

	Automaton& prod;
};

//
//	to generate counter example..
//
class SCCModelChecker::SCCBFSGraph: public BFSGraph<GenBuchiAutomaton>
{
public:
	SCCBFSGraph(SCCModelChecker* mc, const StateMap<Uint>& H, int root, const Vector<State>& initials):
		BFSGraph<GenBuchiAutomaton>(mc->prod, initials), root(root), map(H), mc(mc) {}
protected:
	int root;
	const StateMap<Uint>& map;
	SCCModelChecker* mc;
};


}

#endif /* SCCMODELCHECKER_HH_ */
