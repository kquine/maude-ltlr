/*
 * WeakFairSet.hh
 *
 *  Created on: Dec 10, 2010
 *      Author: kquine
 */

#ifndef WEAKFAIRSET_HH_
#define WEAKFAIRSET_HH_

#include "FairSet.hh"

namespace ltlrModelChecker {

class WeakFairSet: public ltlrModelChecker::FairSet
{
public:
	struct Goal;
	struct Bad;

	WeakFairSet(const NatSet& satisfied);
	void merge(const WeakFairSet* wf);

	FastBool isSatisfied(const NatSet& all) const;
	void dump(ostream& o) const;
	FairSet::Goal* makeFairGoal() const;
	FairSet::Bad* makeBadGoal() const;

protected:
	NatSet satisfiedWeakFair;
};

class WeakFairSet::Goal: public FairSet::Goal
{
public:
	Goal(const WeakFairSet* fs);
	FastBool empty() const;
	FastBool update(const FairSet* f);
	void dump(ostream& o) const;
protected:
	NatSet weakFairGoal;	// falsified entities
};

struct WeakFairSet::Bad: public FairSet::Bad
{
	FastBool isBad(const FairSet* f) const	{ return false; }
	FastBool empty() const					{ return true; }
	void merge(const FairSet::Bad* b)		{}
};

}

#endif /* WEAKFAIRSET_HH_ */
