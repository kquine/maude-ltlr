/*
 * ParamWeakFairSet.cc
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */

#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "ParamWeakFairSet.hh"

namespace ltlrModelChecker {

ParamWeakFairSet::ParamWeakFairSet(const NatSet& realized, const NatSet& satisfied):
		WeakFairSet(satisfied), ParamRealizedSet(realized) {}

void
ParamWeakFairSet::merge(const ParamWeakFairSet* pwf, const NatSet& unrealizedTrue)
{
	const NatSet& common = ParamRealizedSet::merge(pwf);
	WeakFairSet::merge(pwf);
	fillUnrealized(satisfiedWeakFair, common, unrealizedTrue);
}

FastBool
ParamWeakFairSet::isSatisfied() const
{
	return satisfiedWeakFair.contains(realized_fair);
}

void
ParamWeakFairSet::dump(ostream& o) const
{
	o << "(param weak: " << satisfiedWeakFair << ", ";
	ParamRealizedSet::dump(o);
	cout << " )";
}

FairSet::Goal*
ParamWeakFairSet::makeFairGoal() const
{
	return new Goal(this);
}

ParamWeakFairSet::Goal::Goal(const ParamWeakFairSet* fs): WeakFairSet::Goal(fs) {}

FastBool
ParamWeakFairSet::Goal::update(const FairSet* f, const NatSet& unrealizedTrue)
{
	const ParamWeakFairSet* nf = safeCast(const ParamWeakFairSet*,f);
	bool updated = WeakFairSet::Goal::update(f);

	NatSet vacuouslyTrue = unrealizedTrue;
	vacuouslyTrue.intersect(weakFairGoal);		// domain restrict
	vacuouslyTrue.subtract(nf->realized_fair);	// VT = unrealizedTrue - realized_fair

	if (vacuouslyTrue.disjoint(weakFairGoal))
		return updated;
	else
	{
		weakFairGoal.subtract(vacuouslyTrue);
		return true;
	}
}

}
