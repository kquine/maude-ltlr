/*
 * ParamStrongFairSet.cpp
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "ParamStrongFairSet.hh"

namespace ltlrModelChecker {

ParamStrongFairSet::ParamStrongFairSet(const NatSet& realized, const NatSet& supp, const NatSet& concl):
		StrongFairSet(supp, concl), ParamRealizedSet(realized) {}

void
ParamStrongFairSet::merge(const ParamStrongFairSet* psf, const pair<NatSet,NatSet>& unrealizedTrueFair)
{
	const NatSet& common = ParamRealizedSet::merge(psf);
	StrongFairSet::merge(psf);

	fillUnrealized(supp_fair, common, unrealizedTrueFair.first);
	fillUnrealized(concl_fair, common, unrealizedTrueFair.second);
}

void
ParamStrongFairSet::dump(ostream& o) const
{
	o << "(param strong: " << supp_fair << " => " << concl_fair << ", ";
	ParamRealizedSet::dump(o);
	cout << " )";
}

FairSet::Goal*
ParamStrongFairSet::makeFairGoal() const
{
	return new Goal(this);
}

ParamStrongFairSet::Goal::Goal(const ParamStrongFairSet* fs): StrongFairSet::Goal(fs) {}

FastBool
ParamStrongFairSet::Goal::update(const FairSet* f, const pair<NatSet,NatSet>& unrealizedTrueFair)
{
	const ParamStrongFairSet* nf = safeCast(const ParamStrongFairSet*,f);
	bool updated = StrongFairSet::Goal::update(f);

	NatSet vacuouslyTrue = unrealizedTrueFair.second;
	vacuouslyTrue.intersect(strongFairGoal);		// domain restrict
	vacuouslyTrue.subtract(nf->realized_fair);	// VT = unrealizedTrue - realized_fair

	if (vacuouslyTrue.disjoint(strongFairGoal))
		return updated;
	else
	{
		strongFairGoal.subtract(vacuouslyTrue);
		return true;
	}
}

}
