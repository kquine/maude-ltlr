/*
 * WeakFairSet.cc
 *
 *  Created on: Dec 10, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "WeakFairSet.hh"

namespace ltlrModelChecker {


WeakFairSet::WeakFairSet(const NatSet& satisfied): satisfiedWeakFair(satisfied) {}

void
WeakFairSet::merge(const WeakFairSet* wf)
{
	satisfiedWeakFair.insert(wf->satisfiedWeakFair);
}


FastBool
WeakFairSet::isSatisfied(const NatSet& all) const
{
	return all == satisfiedWeakFair;
}

void
WeakFairSet::dump(ostream& o) const
{
	o << "(weak: " << satisfiedWeakFair << ")";
}


FairSet::Goal*
WeakFairSet::makeFairGoal() const
{
	return new Goal(this);
}

FairSet::Bad*
WeakFairSet::makeBadGoal() const
{
	return new Bad;
}

WeakFairSet::Goal::Goal(const WeakFairSet* fs): weakFairGoal(fs->satisfiedWeakFair) {}

FastBool
WeakFairSet::Goal::empty() const
{
	return weakFairGoal.empty();
}

FastBool
WeakFairSet::Goal::update(const FairSet* f)
{
	const WeakFairSet* nf = safeCast(const WeakFairSet*,f);
	if (weakFairGoal.disjoint(nf->satisfiedWeakFair))
		return false;
	else
	{
		weakFairGoal.subtract(nf->satisfiedWeakFair);
		return true;
	}
}

void
WeakFairSet::Goal::dump(ostream& o) const
{
	o << "[weak goal: " << weakFairGoal << "]";
}

}
