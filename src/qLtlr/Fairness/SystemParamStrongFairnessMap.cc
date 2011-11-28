/*
 * SystemParamStrongFairnessMap.cc
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

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"

#include "SystemParamStrongFairnessMap.hh"

namespace ltlrModelChecker {

SystemParamStrongFairnessMap::SystemParamStrongFairnessMap(SystemGraph& sysGraph, StateRealizedFairGenerator& fGenerator):
		sysGraph(sysGraph), fGenerator(fGenerator) {}

FastBool
SystemParamStrongFairnessMap::satisfiedFairSet(const FairnessMap::FairSet* f) const
{
	const FairSet* nf = safeCast(const FairSet*,f);
	return nf->isSatisfied();
}

int
SystemParamStrongFairnessMap::getNrFairness() const
{
	return fGenerator.getNrStrongFairness();
}

FairnessMap::FairSet*
SystemParamStrongFairnessMap::makeFairSet(const Transition& t)
{
	NatSet supp, concl;
	const NatSet& real = fGenerator.getStrongStateFairness(t.source.system);

	FOR_EACH_CONST(i, NatSet, real)
	{
		const pair<Bdd,Bdd>& f = fGenerator.getStrongFairFormula(*i);
		if (sysGraph.satisfiesFormula(f.first, t.source.system, t.systemIndex))
			supp.insert(*i);
		if (sysGraph.satisfiesFormula(f.second, t.source.system, t.systemIndex))
			concl.insert(*i);
	}
	return new FairSet(real, supp, concl);
}

void
SystemParamStrongFairnessMap::mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g)
{
	FairSet* nf = safeCast(FairSet*,f);
	const FairSet* ng = safeCast(const FairSet*,g);
	nf->merge(ng, fGenerator.getStrongVacuousFair());
}

FastBool
SystemParamStrongFairnessMap::updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f)
{
	return safeCast(FairSet::Goal*,goal)->update(f, fGenerator.getStrongVacuousFair());
}


}
