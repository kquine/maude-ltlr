/*
 * SystemParamWeakFairnessMap.hh
 *
 *  Created on: Dec 15, 2010
 *      Author: kquine
 */

#ifndef SYSTEMPARAMWEAKFAIRNESSMAP_HH_
#define SYSTEMPARAMWEAKFAIRNESSMAP_HH_
#include "FairSet/ParamWeakFairSet.hh"
#include "StateRealizedFairGenerator.hh"
#include "FairnessMap.hh"

namespace ltlrModelChecker {

class SystemParamWeakFairnessMap: public ltlrModelChecker::FairnessMap
{
	typedef ParamWeakFairSet		FairSet;
public:
	SystemParamWeakFairnessMap(SystemGraph& sysGraph, StateRealizedFairGenerator& fGenerator);
	FastBool satisfiedFairSet(const FairnessMap::FairSet* f) const;
	int getNrFairness() const;
	FairnessMap::FairSet* makeFairSet(const Transition& t);

	void mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g);
	FastBool updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f);

private:
	StateRealizedFairGenerator& fGenerator;
	SystemGraph& sysGraph;
};

}

#endif /* SYSTEMPARAMWEAKFAIRNESSMAP_HH_ */
