/*
 * SystemWeakFairnessMap.cc
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"

//		forward declarations
#include "strategyLanguage.hh"
#include "mixfix.hh"

#include "natSet.hh"
#include "SystemWeakFairnessMap.hh"


namespace ltlrModelChecker {

SystemWeakFairnessMap::SystemWeakFairnessMap(SystemGraph& sysGraph, const Vector<Bdd>& fTable):
		sysGraph(sysGraph), fairConditions(fTable)
{
	for (int i = fairConditions.size() - 1; i >= 0; --i)
		allWeakFair.insert(i);
}

int
SystemWeakFairnessMap::getNrFairness() const
{
	return fairConditions.size();
}

FastBool
SystemWeakFairnessMap::satisfiedFairSet(const FairnessMap::FairSet* f) const
{
	const FairSet* nf = safeCast(const FairSet*,f);
	return nf->isSatisfied(allWeakFair);
}

FairnessMap::FairSet*
SystemWeakFairnessMap::makeFairSet(const Transition& t)
{
	NatSet satisfied;
	for (int i = fairConditions.size() - 1; i >= 0 ; --i)
	{
		if (sysGraph.satisfiesFormula(fairConditions[i], t.source.system, t.systemIndex))
			satisfied.insert(i);
	}

	return new FairSet(satisfied);
}

void
SystemWeakFairnessMap::mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g)
{
	FairSet* nf = safeCast(FairSet*,f);
	const FairSet* ng = safeCast(const FairSet*,g);
	nf->merge(ng);
}

FastBool
SystemWeakFairnessMap::updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f)
{
	return safeCast(FairSet::Goal*,goal)->update(f);
}

}
