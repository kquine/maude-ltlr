/*
 * StateDagMap.hh
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#ifndef STATETRANSITIONDAGMAP_HH_
#define STATETRANSITIONDAGMAP_HH_

namespace ltlrModelChecker {

struct StateTransitionDagMap
{
	virtual DagNode* getStateDag(int stateNr) const = 0;
	virtual DagNode* getTransitionDag(int stateNr, int transitionNr) const = 0;
};

}

#endif /* STATETRANSITIONDAGMAP_HH_ */
