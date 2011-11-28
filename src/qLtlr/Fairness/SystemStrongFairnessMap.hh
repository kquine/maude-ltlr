/*
 * SystemStrongFairnessMap.hh
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */

#ifndef SYSTEMSTRONGFAIRNESSMAP_HH_
#define SYSTEMSTRONGFAIRNESSMAP_HH_
#include "FairSet/StrongFairSet.hh"
#include "FairnessMap.hh"

namespace ltlrModelChecker {

class SystemStrongFairnessMap: public ltlrModelChecker::FairnessMap
{
	typedef StrongFairSet		FairSet;
public:
	SystemStrongFairnessMap(SystemGraph& sysGraph, const Vector<pair<Bdd,Bdd> >& fTable);
	FastBool satisfiedFairSet(const FairnessMap::FairSet* f) const;
	int getNrFairness() const;
	FairnessMap::FairSet* makeFairSet(const Transition& t);

	void mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g);
	FastBool updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f);
private:
	const Vector<pair<Bdd,Bdd> >& fairConditions;
	SystemGraph& sysGraph;
};

}

#endif /* SYSTEMSTRONGFAIRNESSMAP_HH_ */
