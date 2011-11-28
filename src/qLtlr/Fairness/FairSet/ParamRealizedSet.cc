/*
 * ParamRealizedSet.cc
 *
 *  Created on: Dec 23, 2010
 *      Author: kquine
 */
#include "macros.hh"
#include "vector.hh"

#include "natSet.hh"
#include "ParamRealizedSet.hh"

namespace ltlrModelChecker {


ParamRealizedSet::ParamRealizedSet(const NatSet& realizedFair): realized_fair(realizedFair) {}

void
ParamRealizedSet::fillUnrealized(NatSet& fair, const NatSet& common, const NatSet& unrealizedTrue)
{
	NatSet sum = realized_fair;		// should be merged realized set
	sum.intersect(unrealizedTrue);
	sum.subtract(common);
	fair.insert(sum);
}

NatSet
ParamRealizedSet::merge(const ParamRealizedSet* realSet)
{
	NatSet common = realized_fair;
	common.intersect(realSet->realized_fair);		// ir = R1 \cap R2, where R1 = this, R2 = psf
	realized_fair.insert(realSet->realized_fair);
	return common;
}

void
ParamRealizedSet::dump(ostream& o) const
{
	o << "[realized = " << realized_fair << "]";
}

}
