/*
 * FormulaFairnessMap.cc
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */
//      utility stuff
#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "FormulaFairnessMap.hh"

namespace ltlrModelChecker {

FormulaFairnessMap::FormulaFairnessMap(const GenBuchiAutomaton& property): property(property)
{
	for (int i = property.getNrFairnessSets() - 1; i >= 0; --i)
		allFormulaFair.insert(i);
}

int
FormulaFairnessMap::getNrFairness() const
{
	return allFormulaFair.size();
}


FastBool
FormulaFairnessMap::satisfiedFairSet(const FairnessMap::FairSet* f) const
{
	const FairSet* nf = safeCast(const FairSet*,f);
	return nf->isSatisfied(allFormulaFair);
}

FairnessMap::FairSet*
FormulaFairnessMap::makeFairSet(const Transition& t)
{
	return new FairSet(property.getFairnessCombination(t.propertyIndex->first.second));
}

void
FormulaFairnessMap::mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g)
{
	FairSet* nf = safeCast(FairSet*,f);
	const FairSet* ng = safeCast(const FairSet*,g);
	nf->merge(ng);
}

FastBool
FormulaFairnessMap::updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f)
{
	return safeCast(FairSet::Goal*,goal)->update(f);
}


}
