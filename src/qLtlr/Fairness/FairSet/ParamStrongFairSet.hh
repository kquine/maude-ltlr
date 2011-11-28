/*
 * ParamStrongFairSet.h
 *
 *  Created on: Dec 12, 2010
 *      Author: kquine
 */

#ifndef PARAMSTRONGFAIRSET_HH_
#define PARAMSTRONGFAIRSET_HH_
#include "ParamRealizedSet.hh"
#include "StrongFairSet.hh"

namespace ltlrModelChecker {

class ParamStrongFairSet: public StrongFairSet, private ParamRealizedSet
{
public:
	struct Goal;

	ParamStrongFairSet(const NatSet& realized, const NatSet& supp, const NatSet& concl);
	void merge(const ParamStrongFairSet* psf, const pair<NatSet,NatSet>& unrealizedTrueFair);
	void dump(ostream& o) const;
	FairSet::Goal* makeFairGoal() const;
};

class ParamStrongFairSet::Goal: public StrongFairSet::Goal
{
public:
	Goal(const ParamStrongFairSet* fs);
	FastBool update(const FairSet* f, const pair<NatSet,NatSet>& unrealizedTrueFair);
};

}

#endif /* PARAMSTRONGFAIRSET_H_ */
