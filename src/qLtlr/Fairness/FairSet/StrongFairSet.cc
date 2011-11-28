/*
 * StrongFairSet.cc
 *
 *  Created on: Dec 10, 2010
 *      Author: kquine
 */

#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "StrongFairSet.hh"

namespace ltlrModelChecker {

StrongFairSet::StrongFairSet(const NatSet& supp, const NatSet& concl): supp_fair(supp), concl_fair(concl) {}


void
StrongFairSet::merge(const StrongFairSet* f)
{
	supp_fair.insert(f->supp_fair);
	concl_fair.insert(f->concl_fair);
}

FastBool
StrongFairSet::isSatisfied() const
{
	return concl_fair.contains(supp_fair);
}


FairSet::Goal*
StrongFairSet::makeFairGoal() const
{
	return new Goal(this);
}

FairSet::Bad*
StrongFairSet::makeBadGoal() const
{
	return new Bad(this);
}

void
StrongFairSet::dump(ostream& o) const
{
	o << "(strong: " << supp_fair << " , " << concl_fair << ")";
}


StrongFairSet::Goal::Goal(const StrongFairSet* fs): strongFairGoal(fs->concl_fair) {}

FastBool
StrongFairSet::Goal::empty() const
{
	return strongFairGoal.empty();
}

FastBool
StrongFairSet::Goal::update(const FairSet* f)
{
	const StrongFairSet* nf = safeCast(const StrongFairSet*,f);
	if (strongFairGoal.disjoint(nf->concl_fair))
		return false;
	else
	{
		strongFairGoal.subtract(nf->concl_fair);
		return true;
	}
}

void
StrongFairSet::Goal::dump(ostream& o) const
{
	o << "[strong goal: " << strongFairGoal << "]";
}

StrongFairSet::Bad::Bad(const StrongFairSet* fs)
{
	badFairs = fs->supp_fair;
	badFairs.subtract(fs->concl_fair);
}

FastBool
StrongFairSet::Bad::isBad(const FairSet* f) const
{
	const StrongFairSet* nf = safeCast(const StrongFairSet*,f);
	return !badFairs.disjoint(nf->supp_fair);
}

FastBool
StrongFairSet::Bad::empty() const
{
	return badFairs.empty();
}

void
StrongFairSet::Bad::merge(const FairSet::Bad* b)
{
	const Bad* nb = safeCast(const Bad*, b);
	badFairs.insert(nb->badFairs);
}

}
