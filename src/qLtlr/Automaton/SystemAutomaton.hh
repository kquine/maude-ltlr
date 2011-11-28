/*
 * StateSystemAutomaton.h
 *
 *  Created on: Nov 26, 2010
 *      Author: kquine
 */

#ifndef STATESYSTEMAUTOMATON_H_
#define STATESYSTEMAUTOMATON_H_
#include "Util/PtrVector.hh"
#include "Util/PropDagNodeSet.hh"
#include "Interface/PropLabelChecker.hh"
#include "SearchGraph/stateTransitionGraph.hh"
#include "StateTransitionDagMap.hh"
#include "SystemGraph.hh"

namespace ltlrModelChecker {

class SystemAutomaton: public SystemGraph, public StateTransitionDagMap
{
	NO_COPYING(SystemAutomaton);
public:
	SystemAutomaton(RewritingContext* sysContext, PropDagNodeSet& propositions,
			PropLabelChecker* propChecker, ProofTermGenerator* proofTerm);

	int getNrStates() const;
	int getNrTransitions(int stateNr) const;
	int getNextState(int stateNr, int transitionNr);

	DagNode* getStateDag(int stateNr) const;
	DagNode* getTransitionDag(int stateNr, int transitionNr) const;

protected:
	FastBool checkStateLabel(int propId, int stateNr);

private:
	struct Label
	{
		NatSet testedItems;
		NatSet trueItems;
	};
	PtrVector<Label> stateLabels;
	PropDagNodeSet& propositions;
    PropLabelChecker* propChecker;

    auto_ptr<ltlrModelChecker::StateTransitionGraph> graph;
    RewritingContext* sysContext;
};

}

#endif /* STATESYSTEMAUTOMATON_H_ */
