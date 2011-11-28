/*
 * SCCBuchiModelChecker.cc
 *
 *  Created on: Dec 6, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"

//      utility class definitions
#include "natSet.hh"
#include "BFSGraph.hh"
#include "Util/PtrStack.hh"

#include "SCCBuchiModelChecker.hh"

namespace ltlrModelChecker {

SCCBuchiModelChecker::SCCBuchiModelChecker(Automaton& prod, CompositedFairnessMap& fm): SCCModelChecker(prod,fm) {}


SCCModelChecker::SCC*
SCCBuchiModelChecker::findAcceptedSCC(const Vector<State>& initials)
{
	SCCStack stack(this);
	auto_ptr<FairSet> emptyFairPtr;

	FOR_EACH_CONST(i, Vector<State>, initials)
	{
		if (H.expand(*i) || !H.contains(*i))	// if the initial state has not visited yet..
		{
			stack.dfsPush(*i, emptyFairPtr);
			//
			// main loop
			//
			while (! stack.empty() )
			{
				if (stack.hasNextSucc())
				{
					Transition t = stack.pickSucc();
					auto_ptr<FairSet> a(fairMap.makeFairSet(t));
					stack.nextSucc();

					if (H.expand(t.target) || !H.contains(t.target))	// if the next state not visited yet..
						stack.dfsPush(t.target, a);
					else if (H.get(t.target) > 0)		// if on the dfs stack..
					{
						stack.merge(H.get(t.target), a);
						if ( fairMap.satisfiedFairSet(stack.topSCC()->acc_fair.get()) )
							return new SCC(*stack.topSCC());
					}
				}
				else
					stack.sccPop();
			}
		}
	}
	return NULL;
}


}
