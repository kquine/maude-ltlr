/*
 * SCCBuchiModelChecker.h
 *
 *  Created on: Nov 30, 2010
 *      Author: kquine
 */

#ifndef SCCBUCHIMODELCHECKER_HH_
#define SCCBUCHIMODELCHECKER_HH_
#include "SCCModelChecker.hh"
#include "Fairness/CompositedFairnessMap.hh"

namespace ltlrModelChecker {

class SCCBuchiModelChecker : public ltlrModelChecker::SCCModelChecker
{
	NO_COPYING(SCCBuchiModelChecker);
public:
	SCCBuchiModelChecker(Automaton& prod, CompositedFairnessMap& fm);

private:
	SCC* findAcceptedSCC(const Vector<State>& initials);
};

}

#endif /* SCCBUCHIMODELCHECKER_H_ */
