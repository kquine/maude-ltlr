/*
 * TwoWayHashConsSet.hh
 *
 *  Created on: Dec 1, 2010
 *      Author: kquine
 */

#ifndef TWOWAYHASHCONSSET_HH_
#define TWOWAYHASHCONSSET_HH_
#include "hashConsSet.hh"

namespace ltlrModelChecker {

class TwoWayHashConsSet
{
public:
	DagNode* getCanonical(int index) const;
	int getOppositeIndex(int hashConsIndex) const;
	pair<int,bool> insertDag(DagNode* dag, int value);	// return (hashConsIndex, isNew)

private:
	HashConsSet hset;
	Vector<int> hashCons2dagIndex;		// partial map of hashCons indices to dag info
};

inline DagNode*
TwoWayHashConsSet::getCanonical(int index) const
{
	return hset.getCanonical(index);
}

inline int
TwoWayHashConsSet::getOppositeIndex(int hashConsIndex) const
{
	return hashCons2dagIndex[hashConsIndex];
}

}

#endif /* TWOWAYHASHCONSSET_HH_ */
