/*
 * TwoWayHashConsSet.cc
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

//	utility stuff
#include "macros.hh"
#include "vector.hh"

//	forward declarations
#include "interface.hh"
#include "core.hh"
#include "dagNode.hh"

//	core class definitions
#include "TwoWayHashConsSet.hh"

namespace ltlrModelChecker {

pair<int,bool>
TwoWayHashConsSet::insertDag(DagNode* dag, int value)
{
	int hashConsIndex = hset.insert(dag);
	int mapSize = hashCons2dagIndex.size();

	//DebugAdvisory("replacement dag = " << r.first << "hashConsIndex = " << hashConsIndex);
	if (hashConsIndex >= mapSize)
	{
		//
		//	Definitely a new dag.
		//
		hashCons2dagIndex.resize(hashConsIndex + 1);
		for (int i = mapSize; i < hashConsIndex; ++i)
			hashCons2dagIndex[i] = NONE;
		hashCons2dagIndex[hashConsIndex] = value;
		/*DebugAdvisory("new dag = " << dag << " hashConsIndex = " << hashConsIndex <<
				" collisionCounter = " << collisionCounter);*/
		return make_pair(hashConsIndex,true);
	}
	else	//	Seen before.
	{
		//
		//	 But only as a subdag, not as a state or proof-term dag, so it counts as a new dag.
		//
		if (hashCons2dagIndex[hashConsIndex] == NONE)
		{
			hashCons2dagIndex[hashConsIndex] = value;
			return make_pair(hashConsIndex,true);
		}
	}
	return make_pair(hashConsIndex,false);
}

}
