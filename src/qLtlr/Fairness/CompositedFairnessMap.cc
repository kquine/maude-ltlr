/*
 * CompositedFairnessMap.cc
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//		forward declarations
#include "strategyLanguage.hh"
#include "mixfix.hh"

#include "natSet.hh"
#include "CompositedFairnessMap.hh"

namespace ltlrModelChecker {

void
CompositedFairnessMap::addFairnessMap(FairnessMap* fairnessMap)
{
	fairMaps.append(fairnessMap);
}

int
CompositedFairnessMap::getNrFairness() const
{
	int n = 0;
	FOR_EACH_CONST(i, Vector<FairnessMap*>, fairMaps)
		n += (*i)->getNrFairness();
	return n;

}

FastBool
CompositedFairnessMap::satisfiedFairSet(const FairnessMap::FairSet* f) const
{
	const FairSet* nf = safeCast(const FairSet*,f);
	for (int i = 0; i < fairMaps.size(); ++i)
		if (!fairMaps[i]->satisfiedFairSet(nf->fairSets[i]))
			return false;
	return true;

}

FairnessMap::FairSet*
CompositedFairnessMap::makeFairSet(const Transition& t)
{
	FairSet* f = new FairSet;
	FOR_EACH_CONST(i, Vector<FairnessMap*>, fairMaps)
		f->fairSets.append((*i)->makeFairSet(t));
	return f;
}


void
CompositedFairnessMap::mergeFairSet(FairnessMap::FairSet* f, const FairnessMap::FairSet* g)
{
	FairSet* nf = safeCast(FairSet*,f);
	const FairSet* ng = safeCast(const FairSet*,g);
	for (int i = 0; i < fairMaps.size(); ++i)
		fairMaps[i]->mergeFairSet(nf->fairSets[i], ng->fairSets[i]);
}

FastBool
CompositedFairnessMap::updateFairGoal(FairnessMap::FairSet::Goal* goal, const FairnessMap::FairSet* f)
{
	FairSet::Goal* ngoal = safeCast(FairSet::Goal*,goal);
	const FairSet* nf = safeCast(const FairSet*,f);
	bool updated = false;
	for (int i = 0; i < fairMaps.size(); ++i)
		updated |= fairMaps[i]->updateFairGoal(ngoal->fairGoals[i], nf->fairSets[i]);
	return updated;
}


}
