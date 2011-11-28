/*
 * ParamRealizedSet.hh
 *
 *  Created on: Dec 23, 2010
 *      Author: kquine
 */

#ifndef PARAMREALIZEDSET_HH_
#define PARAMREALIZEDSET_HH_

namespace ltlrModelChecker {

class ParamRealizedSet
{
public:
	ParamRealizedSet(const NatSet& realizedFair);
	NatSet merge(const ParamRealizedSet* realSet);	// merge, and return intersection
	void dump(ostream& o) const;
protected:
	void fillUnrealized(NatSet& fair, const NatSet& common, const NatSet& unrealizedTrue);
	NatSet realized_fair;
};

}

#endif /* PARAMREALIZEDSET_HH_ */
