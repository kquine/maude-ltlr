/*
 * StreettModelChecker.hh
 *
 *  Created on: Dec 8, 2010
 *      Author: kquine
 */

#ifndef STREETTMODELCHECKER_HH_
#define STREETTMODELCHECKER_HH_
#include "Fairness/CompositedFairnessMap.hh"
#include "SCCModelChecker.hh"

namespace ltlrModelChecker {

class StreettModelChecker: public ltlrModelChecker::SCCModelChecker
{
	NO_COPYING(StreettModelChecker);
public:
	StreettModelChecker(Automaton& prod, CompositedFairnessMap& fm);

private:
	SCC* findAcceptedSCC(const Vector<State>& initials);
	SCC* findAcceptedSCC(queue<State>& region, FairSet::Bad* bad);
	FairSet::Bad* makeNewBadGoal(const auto_ptr<FairSet>& fair, const FairSet::Bad* old);
};


}

#endif /* STREETTMODELCHECKER_HH_ */
