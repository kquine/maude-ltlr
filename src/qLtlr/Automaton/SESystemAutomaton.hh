/*
 * SESystemAutomaton.hh
 *
 *  Created on: Nov 27, 2010
 *      Author: kquine
 */

#ifndef SESYSTEMAUTOMATON_HH_
#define SESYSTEMAUTOMATON_HH_
#include "Util/PtrVector.hh"
#include "Util/PropDagNodeSet.hh"
#include "Interface/PropLabelChecker.hh"
#include "SearchGraph/seStateTransitionGraph.hh"
#include "StateTransitionDagMap.hh"
#include "SystemEventGraph.hh"

namespace ltlrModelChecker {

class SESystemAutomaton:  public SystemEventGraph, public StateTransitionDagMap
{
	NO_COPYING(SESystemAutomaton);
public:
	SESystemAutomaton(RewritingContext* sysContext, PropDagNodeSet& propositions,
			PropLabelChecker* propChecker, ProofTermGenerator* proofTerm);

	int getNrStates() const;
	int getNrTransitions(int stateNr) const;
	int getNextState(int stateNr, int transitionNr);

	DagNode* getStateDag(int stateNr) const;
	DagNode* getTransitionDag(int stateNr, int transitionNr) const;

protected:
	FastBool checkStateLabel(int propId, int stateNr);
	FastBool checkEventLabel(int propId, int stateNr, int transitionNr);

private:
	struct Label
	{
		NatSet testedItems;
		NatSet trueItems;
	};
	struct StateEventLabel
	{
		Label state;
		PtrVector<Label> transitionLabels;
	};
	FastBool isEventProp(int propId) const;

    PtrVector<StateEventLabel> labels;

	PropDagNodeSet& propositions;
    PropLabelChecker* propChecker;

    auto_ptr<SEStateTransitionGraph> graph;
    RewritingContext* sysContext;
};



}

#endif /* SESYSTEMAUTOMATON_HH_ */
