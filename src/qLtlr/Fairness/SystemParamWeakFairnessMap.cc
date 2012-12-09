/*
 * SystemParamWeakFairnessMap.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"
#include "natSet.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"
#include "strategyLanguage.hh"
#include "mixfix.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"

#include "SystemParamWeakFairnessMap.hh"

namespace ltlrModelChecker {

SystemParamWeakFairnessMap::SystemParamWeakFairnessMap(SystemGraph& sysGraph, StateRealizedFairGenerator& fGenerator):
		sysGraph(sysGraph), fGenerator(fGenerator) {}

FastBool
SystemParamWeakFairnessMap::satisfiedFairSet(const FairnessMap::FairSet* f) const
{
	const FairSet* nf = safeCast(const FairSet*,f);
	return nf->isSatisfied();
}

int
SystemParamWeakFairnessMap::getNrFairness() const
{
	return fGenerator.getNrWeakFairness();
}


FairnessMap::FairSet*
SystemParamWeakFairnessMap::makeFairSet(const Transition& t)
{
	NatSet concl;
	const NatSet& real = fGenerator.getWeakStateFairness(t.source.system);

	FOR_EACH_CONST(i, NatSet, real)
	{
		if (sysGraph.satisfiesFormula(fGenerator.getWeakFairFormula(*i), t.source.system, t.systemIndex))
				concl.insert(*i);
	}
	return new FairSet(real, concl);
}

void
SystemParamWeakFairnessMap::mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g)
{
	FairSet* nf = safeCast(FairSet*,f);
	const FairSet* ng = safeCast(const FairSet*,g);
	nf->merge(ng, fGenerator.getWeakVacuousFair());
}

FastBool
SystemParamWeakFairnessMap::updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f)
{
	return safeCast(FairSet::Goal*,goal)->update(f, fGenerator.getWeakVacuousFair());
}

}
