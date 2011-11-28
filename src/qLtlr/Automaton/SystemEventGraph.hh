/*
 * SystemEventGraph.hh
 *
 *  Created on: Dec 2, 2010
 *      Author: kquine
 */

#ifndef SYSTEMEVENTGRAPH_HH_
#define SYSTEMEVENTGRAPH_HH_
#include "SystemGraph.hh"

namespace ltlrModelChecker {

//
//	State/Event Automaton Interface
//
class SystemEventGraph: public SystemGraph
{
public:
	// we assume that the formula contains event props.
	pair<FastBool,Bdd> satisfiesFormula(Bdd formula, int stateNr);
	FastBool satisfiesFormula(Bdd formula, int stateNr, int transitionNr);
protected:
	virtual FastBool checkEventLabel(int propId, int stateNr, int transitionNr) = 0;
	virtual FastBool isEventProp(int propId) const = 0;
};

}

#endif /* SYSTEMEVENTGRAPH_HH_ */
