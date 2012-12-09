/*
 * DagTransitionGraph.cc
 *
 *  Created on: Nov 30, 2010
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
#include "strategyLanguage.hh"
#include "mixfix.hh"

//	interface class definitions
#include "dagNode.hh"

#include "DagTransitionGraph.hh"

namespace ltlrModelChecker {

DagTransitionGraph::DagTransitionGraph(RewritingContext* initial, ProofTermGenerator* proofTerm):
		initial(initial), proofTerm(proofTerm)
{
	initial->reduce();
}

int
DagTransitionGraph::getNrStates() const
{
	return seen.size();
}

int
DagTransitionGraph::getNrTransitions(int stateNr) const
{
	Assert(seen[stateNr] != NULL, "Invalid state lookup (transition sizes)");
	return seen[stateNr]->nextStates.size();
}

DagNode*
DagTransitionGraph::getStateDag(int stateNr) const
{
	Assert(stateNr < getNrStates(), "Invalid state lookup (state dag)");
	return hashConsSet.getCanonical(seen[stateNr]->hashConsIndex);
}

DagTransitionGraph::State::State(const DagTransitionGraph* graph, int hashConsIndex): hashConsIndex(hashConsIndex)
{
	//
	//	During DFS, a next state will be immediately explored.
	//  Hence, creating rewriteState HERE do not produce additional spaces.
	//
	DagNode* canonicalStateDag = graph->hashConsSet.getCanonical(hashConsIndex);
	RewritingContext* newContext = graph->initial->makeSubcontext(canonicalStateDag);
	rewriteState.reset(new RewriteSearchState(newContext,
					       NONE,
					       RewriteSearchState::GC_CONTEXT |
					       RewriteSearchState::SET_UNREWRITABLE |
					       PositionState::SET_UNSTACKABLE,
					       0,
					       UNBOUNDED));
}

}
