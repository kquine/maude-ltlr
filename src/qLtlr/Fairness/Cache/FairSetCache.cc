/*
 * FairSetCache.cc
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#include "macros.hh"
#include "vector.hh"

#include "Automaton/SystemEventGraph.hh"


namespace ltlrModelChecker {

/////////////////////////////////////////////////
//	Specialization for state-only
/////////////////////////////////////////////////
template <>
class FairSetCache<SystemGraph>
{
	typedef FairnessMap::FairSet	FairSet;
public:
	~FairSetCache();
	FairSet* get(int stateNr, int transitionNr);
	void set(FairSet*, int stateNr, int transitionNr);
private:
	Vector<FairSet*> fairCache;
};

FairSetCache<SystemGraph>::~FairSetCache()
{
	FOR_EACH_CONST(i, Vector<FairSet*>, fairCache)
		delete *i;
}

inline FairnessMap::FairSet*
FairSetCache<SystemGraph>::get(int stateNr, int transitionNr)
{
	int size1 = fairCache.size();
	if (stateNr >= size1)
	{
		fairCache.expandTo(stateNr + 1);
		for(int i = size1; i <= stateNr; ++i)
			fairCache[stateNr] = NULL;
	}
	return fairCache[stateNr];
}

inline void
FairSetCache<SystemGraph>::set(FairSet* fs, int stateNr, int transitionNr)
{
	fairCache[stateNr] = fs;
}

/////////////////////////////////////////////////
//	Specialization for state-event
/////////////////////////////////////////////////

template <>
class FairSetCache<SystemEventGraph>
{
	typedef FairnessMap::FairSet	FairSet;
public:
	~FairSetCache();
	FairSet* get(int stateNr, int transitionNr);
	void set(FairSet*, int stateNr, int transitionNr);
private:
	Vector<Vector<FairSet*> > fairCache;
};

FairSetCache<SystemEventGraph>::~FairSetCache()
{
	FOR_EACH_CONST(i, Vector<Vector<FairSet*> >, fairCache)
	{
		FOR_EACH_CONST(j, Vector<FairSet*>, *i)
			delete *j;
	}
}

inline FairnessMap::FairSet*
FairSetCache<SystemEventGraph>::get(int stateNr, int transitionNr)
{
	int size1 = fairCache.size();
	if (stateNr >= size1)
	{
		fairCache.expandTo(stateNr + 1);
		for(int i = size1; i <= stateNr; ++i)
			fairCache[stateNr] = NULL;
	}
	int size2 = fairCache[stateNr].size();
	if (transitionNr >= size2)
	{
		fairCache[stateNr].expandTo(transitionNr + 1);
		for(int j = size2; j <= transitionNr; ++j)
			fairCache[stateNr][transitionNr] = NULL;
	}
	return fairCache[stateNr][transitionNr];
}

inline void
FairSetCache<SystemEventGraph>::set(FairSet* fs, int stateNr, int transitionNr)
{
	fairCache[stateNr][transitionNr] = fs;
}


}
