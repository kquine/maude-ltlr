/*
 * DagTransitionGraph.hh
 *
 *  Created on: Nov 30, 2010
 *      Author: kquine
 */

#ifndef DAGTRANSITIONGRAPH_HH_
#define DAGTRANSITIONGRAPH_HH_
#include <memory>
#include "Util/PtrVector.hh"
#include "rewriteSearchState.hh"
#include "Util/TwoWayHashConsSet.hh"
#include "Interface/ProofTermGenerator.hh"

namespace ltlrModelChecker {

//
//	Base class for transition graphs
//
class DagTransitionGraph
{
	NO_COPYING(DagTransitionGraph);

public:
	DagTransitionGraph(RewritingContext* initial, ProofTermGenerator* proofTerm);
	virtual ~DagTransitionGraph() {}

	int getNrStates() const;
	int getNrTransitions(int stateNr) const;

	DagNode* getStateDag(int stateNr) const;

protected:
	struct State
	{
		NO_COPYING(State);
		State(const DagTransitionGraph* graph, int hashConsIndex);

		const int hashConsIndex;
		auto_ptr<RewriteSearchState> rewriteState;	// do rewriting for next states. 0 for "fully explored"
		Vector<int> nextStates;
	};

	ProofTermGenerator* proofTerm;

	TwoWayHashConsSet hashConsSet;
	PtrVector<State> seen;
	RewritingContext* initial;
};

}

#endif /* DAGTRANSITIONGRAPH_HH_ */
