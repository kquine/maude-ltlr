/*
 * SystemWeakFairnessMap.hh
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */

#ifndef SYSTEMWEAKFAIRNESSMAP_HH_
#define SYSTEMWEAKFAIRNESSMAP_HH_
#include "FairSet/WeakFairSet.hh"
#include "FairnessMap.hh"

namespace ltlrModelChecker {

class SystemWeakFairnessMap: public ltlrModelChecker::FairnessMap
{
	typedef WeakFairSet		FairSet;
public:
	SystemWeakFairnessMap(SystemGraph& sysGraph, const Vector<Bdd>& fTable);
	int getNrFairness() const;
	FastBool satisfiedFairSet(const FairnessMap::FairSet* f) const;
	FairnessMap::FairSet* makeFairSet(const Transition& t);

	void mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g);
	FastBool updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f);

private:
	NatSet allWeakFair;
	const Vector<Bdd>& fairConditions;
	SystemGraph& sysGraph;
};

}

#endif /* SYSTEMWEAKFAIRNESSMAP_HH_ */
