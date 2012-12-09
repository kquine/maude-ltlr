/*
 * SESystemAutomaton.cc
 *
 *  Created on: Nov 27, 2010
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

#include "Interface/ProofTermGenerator.hh"
#include "SESystemAutomaton.hh"

namespace ltlrModelChecker {

SESystemAutomaton::SESystemAutomaton(RewritingContext* sysContext,
		PropDagNodeSet& propositions, PropLabelChecker* propChecker, ProofTermGenerator* proofTerm)
	: sysContext(sysContext), propositions(propositions), propChecker(propChecker)
{
	graph.reset(new SEStateTransitionGraph(sysContext, proofTerm));
}


local_inline int
SESystemAutomaton::getNrStates() const
{
	return graph->getNrStates();
}

local_inline int
SESystemAutomaton::getNrTransitions(int stateNr) const
{
	return graph->getNrTransitions(stateNr);
}

local_inline DagNode*
SESystemAutomaton::getStateDag(int stateNr) const
{
	return graph->getStateDag(stateNr);
}

local_inline DagNode*
SESystemAutomaton::getTransitionDag(int stateNr, int transitionNr) const
{
	return graph->getTransitionDag(stateNr, transitionNr);
}


local_inline int
SESystemAutomaton::getNextState(int stateNr, int index)
{
    int n = graph->getNextState(stateNr, index);
    if (n == NONE && index == 0)
        return stateNr; // fake a self loop for deadlocked state
    return n;
}

FastBool
SESystemAutomaton::isEventProp(int propId) const
{
	return propositions.isEvent(propId);
}

local_inline FastBool
SESystemAutomaton::checkStateLabel(int propId, int stateNr)
{
	if (stateNr < labels.size())
	{
		if (labels[stateNr]->state.testedItems.contains(propId))
			return labels[stateNr]->state.trueItems.contains(propId);
	}
	else
		labels.expandTo(stateNr + 1);

	FastBool result = propChecker->computeStateLabel(
			graph->getStateDag(stateNr), propositions.index2DagNode(propId), sysContext);
	labels[stateNr]->state.testedItems.insert(propId);
	if (result)
		labels[stateNr]->state.trueItems.insert(propId);
	return result;
}

local_inline FastBool
SESystemAutomaton::checkEventLabel(int propId, int stateNr, int transitionNr)
{
	if (stateNr < labels.size())
	{
		if (transitionNr < labels[stateNr]->transitionLabels.size())
		{
			const Label* l = labels[stateNr]->transitionLabels[transitionNr];
			if (l->testedItems.contains(propId))
				return l->trueItems.contains(propId);
		}
		else
			labels[stateNr]->transitionLabels.expandTo(transitionNr + 1);
	}
	else
	{
		labels.expandTo(stateNr + 1);
		labels[stateNr]->transitionLabels.expandTo(transitionNr + 1);
	}

	FastBool result = propChecker->computeEventLabel(
			graph->getTransitionDag(stateNr,transitionNr), propositions.index2DagNode(propId), sysContext);
	Label* l = labels[stateNr]->transitionLabels[transitionNr];
	l->testedItems.insert(propId);
	if (result)
		l->trueItems.insert(propId);
	return result;
}


}
