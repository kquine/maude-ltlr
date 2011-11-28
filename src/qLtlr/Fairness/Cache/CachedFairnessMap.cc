/*
 * CachedFairnessMap.cc
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */
//      utility stuff
#include "macros.hh"
#include "vector.hh"

#include "ParamFairnessMap.hh"
#include "CachedFairnessMap.hh"

namespace ltlrModelChecker {


template <typename FM, typename SA>
CachedFairnessMap<FM,SA>::CachedFairnessMap(SA* sysGraph, FairnessTable& fTable): FM(sysGraph,fTable) {}


template <typename FM, typename SA> typename CachedFairnessMap<FM,SA>::FairSet*
CachedFairnessMap<FM,SA>::getFairSet(int stateNr, int transitionNr)
{
	FairSet* f = static_cast<FairSet*>(cache.get(stateNr, transitionNr));
	if (f == NULL)
	{
		f = FM::makeFairSet(stateNr, transitionNr);
		cache.set(f, stateNr, transitionNr);
	}
	return f;
}

////////////////////////////////////////////////////////////////////////////////////
//	specialization for param fairness
////////////////////////////////////////////////////////////////////////////////////

template <>
class CachedFairnessMap<ParamFairnessMap,SystemGraph>: public ParamFairnessMap
{
public:
	typedef ParamFairnessMap::FairSet			FairSet;
	typedef ParamFairnessMap::FairnessTable		FairnessTable;

	CachedFairnessMap(SystemGraph* sysGraph, FairnessTable& fTable): ParamFairnessMap(sysGraph,fTable) {}
	FairSet* getFairSet(int stateNr, int transitionNr)
	{
		FairSet* f = static_cast<FairSet*>(cache.get(stateNr, transitionNr));
		if (f == NULL)
		{
			f = ParamFairnessMap::makeFairSet(stateNr, transitionNr, fairConditions.getStateRelatedFair(stateNr));
			cache.set(f, stateNr, transitionNr);
		}
		return f;
	}

private:
	FairSetCache<SystemGraph> cache;
};

template <>
class CachedFairnessMap<ParamFairnessMap,SystemEventGraph>: public ParamFairnessMap
{
public:
	typedef ParamFairnessMap::FairSet			FairSet;
	typedef ParamFairnessMap::FairnessTable		FairnessTable;

	CachedFairnessMap(SystemGraph* sysGraph, FairnessTable& fTable): ParamFairnessMap(sysGraph,fTable) {}
	~CachedFairnessMap()
	{
		FOR_EACH_CONST(i, Vector<NatSet*>, stateRelatedFair)
			delete *i;
	}
	FairSet* getFairSet(int stateNr, int transitionNr)
	{
		FairSet* f = static_cast<FairSet*>(cache.get(stateNr, transitionNr));
		if (f == NULL)
		{
			f = ParamFairnessMap::makeFairSet(stateNr, transitionNr, getStateRealizedFair(stateNr));
			cache.set(f, stateNr, transitionNr);
		}
		return f;
	}

private:
	const NatSet& getStateRealizedFair(int stateNr)
	{
		int srSize = stateRelatedFair.size();
		if (srSize <= stateNr)
		{
			stateRelatedFair.expandTo(stateNr + 1);
			for(int i = srSize; i <= stateNr; ++i)
				stateRelatedFair[i] = NULL;
		}
		if (stateRelatedFair[stateNr] == NULL)
			stateRelatedFair[stateNr] = new NatSet(fairConditions.getStateRelatedFair(stateNr));
		return *stateRelatedFair[stateNr];
	}

	Vector<NatSet*> stateRelatedFair;
	FairSetCache<SystemGraph> cache;
};

}
