/*
 * CachedFairnessMap.hh
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#ifndef CACHEDFAIRNESSMAP_HH_
#define CACHEDFAIRNESSMAP_HH_
#include "bdd.hh"
#include "FairSetCache.hh"

namespace ltlrModelChecker
{

template <typename _FairnessMap, typename _System>
class CachedFairnessMap: public _FairnessMap
{
public:
	typedef typename _FairnessMap::FairSet			FairSet;
	typedef typename _FairnessMap::FairnessTable	FairnessTable;

	CachedFairnessMap(_System* sysGraph, FairnessTable& fTable);
	FairSet* getFairSet(int stateNr, int transitionNr);

private:
	FairSetCache<_System> cache;
};

}

#include "CachedFairnessMap.cc"

#endif /* CACHEDFAIRNESSMAP_HH_ */
