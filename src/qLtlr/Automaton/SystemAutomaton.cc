/*
 * StateSystemAutomaton.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"
#include "higher.hh"
#include "freeTheory.hh"
#include "strategyLanguage.hh"
#include "mixfix.hh"

#include "SystemAutomaton.hh"

namespace ltlrModelChecker {

SystemAutomaton::SystemAutomaton(RewritingContext* sysContext, PropDagNodeSet& propositions,
		PropLabelChecker* propChecker, ProofTermGenerator* proofTerm)
	: sysContext(sysContext), propositions(propositions), propChecker(propChecker)
{
	Assert(!propositions.hasEvent(), "State/Event automaton required");
	graph.reset(new ltlrModelChecker::StateTransitionGraph(sysContext, proofTerm));
}

local_inline int
SystemAutomaton::getNrStates() const
{
	return graph->getNrStates();
}

local_inline int
SystemAutomaton::getNrTransitions(int stateNr) const
{
	return graph->getNrTransitions(stateNr);
}

local_inline DagNode*
SystemAutomaton::getStateDag(int stateNr) const
{
	return graph->getStateDag(stateNr);
}

local_inline DagNode*
SystemAutomaton::getTransitionDag(int stateNr, int transitionNr) const
{
	return graph->getTransitionDag(stateNr, transitionNr);
}

local_inline int
SystemAutomaton::getNextState(int stateNr, int index)
{
    int n = graph->getNextState(stateNr, index);
    if (n == NONE && index == 0)
        return stateNr; // fake a self loop for deadlocked state
    return n;
}

local_inline FastBool
SystemAutomaton::checkStateLabel(int propId, int stateNr)
{
	if (stateNr < stateLabels.size())
	{
		if (stateLabels[stateNr]->testedItems.contains(propId))
			return stateLabels[stateNr]->trueItems.contains(propId);
	}
	else
		stateLabels.expandTo(stateNr + 1);

	FastBool result = propChecker->computeStateLabel(graph->getStateDag(stateNr),
			propositions.index2DagNode(propId), sysContext);
	stateLabels[stateNr]->testedItems.insert(propId);
	if (result)
		stateLabels[stateNr]->trueItems.insert(propId);
	return result;
}

}
