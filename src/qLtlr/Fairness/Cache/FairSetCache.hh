/*
 * FairSetCache.hh
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#ifndef FAIRSETCACHE_HH_
#define FAIRSETCACHE_HH_
#include "FairnessMap.hh"

namespace ltlrModelChecker {

template <typename _System>
struct FairSetCache
{
	typedef FairnessMap::FairSet	FairSet;

	~FairSetCache();
	FairSet* get(int stateNr, int transitionNr);
	void set(int stateNr, int transitionNr);
};

}

#include "FairSetCache.cc"

#endif /* FAIRSETCACHE_HH_ */
