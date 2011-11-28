/*
 * CompositedFairSet.cpp
 *
 *  Created on: Dec 10, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "CompositedFairSet.hh"

namespace ltlrModelChecker {


FairSet::Goal*
CompositedFairSet::makeFairGoal() const
{
	return new Goal(this);
}

FairSet::Bad*
CompositedFairSet::makeBadGoal() const
{
	return new Bad(this);
}


void
CompositedFairSet::dump(ostream& o) const
{
	o << "[composited:";
	FOR_EACH_CONST(i, PtrVector<FairSet>, fairSets)
	{
		o << " ";
		(*i)->dump(o);
	}
	o << "]";
}

CompositedFairSet::Goal::Goal(const CompositedFairSet* fs)
{
	FOR_EACH_CONST(i, PtrVector<FairSet>, fs->fairSets)
		fairGoals.append((*i)->makeFairGoal());
}

FastBool
CompositedFairSet::Goal::empty() const
{
	FOR_EACH_CONST(i,PtrVector<FairSet::Goal>, fairGoals)
		if ( !(*i)->empty())
			return false;
	return true;
}

void
CompositedFairSet::Goal::dump(ostream& o) const
{
	o << "[composited:";
	FOR_EACH_CONST(i, PtrVector<FairSet::Goal>, fairGoals)
	{
		o << " ";
		(*i)->dump(o);
	}
	o << "]";
}

CompositedFairSet::Bad::Bad(const CompositedFairSet* fs)
{
	FOR_EACH_CONST(i, PtrVector<FairSet>, fs->fairSets)
		fairBadSets.append((*i)->makeBadGoal());
}

FastBool
CompositedFairSet::Bad::isBad(const FairSet* f) const
{
	const CompositedFairSet* nf = safeCast(const CompositedFairSet*,f);
	for (int i = 0; i < fairBadSets.size(); ++i)
		if ( fairBadSets[i]->isBad(nf->fairSets[i]) )
			return true;
	return false;
}

FastBool
CompositedFairSet::Bad::empty() const
{
	FOR_EACH_CONST(i, PtrVector<FairSet::Bad>, fairBadSets)
		if ( !(*i)->empty())
			return false;
	return true;
}

void
CompositedFairSet::Bad::merge(const FairSet::Bad* b)
{
	const Bad* nb = safeCast(const Bad*, b);
	for (int i = 0; i < fairBadSets.size(); ++i)
		fairBadSets[i]->merge(nb->fairBadSets[i]);
}

}
