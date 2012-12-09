/*
 * StreettModelChecker.cc
 *
 *  Created on: Dec 8, 2010
 *      Author: kquine
 */

//      utility stuff
#include <queue>
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"
#include "strategyLanguage.hh"
#include "mixfix.hh"

//      utility class definitions
#include "natSet.hh"
#include "BFSGraph.hh"
#include "Util/PtrStack.hh"


#include "StreettModelChecker.hh"

namespace ltlrModelChecker {

StreettModelChecker::StreettModelChecker(Automaton& prod, CompositedFairnessMap& fm): SCCModelChecker(prod,fm) {}

SCCModelChecker::SCC*
StreettModelChecker::findAcceptedSCC(const Vector<State>& initials)
{
	queue<State> region;
	FOR_EACH_CONST(i, Vector<State>, initials)
		region.push(*i);
	return findAcceptedSCC(region, NULL);
}

SCCModelChecker::SCC*
StreettModelChecker::findAcceptedSCC(queue<State>& region, FairSet::Bad* bad)
{
	SCCStack stack(this);
	auto_ptr<FairSet> emptyFairPtr;

	while ( !region.empty())
	{
		State cur = region.front();
		region.pop();
		if (H.expand(cur) || !H.contains(cur))
		{
			stack.dfsPush(cur, emptyFairPtr);
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

					/*
					cout << t.source << " --> " << t.target << " " << flush;
					a->dump(cout);
					cout << endl;
					*/

					if (bad != NULL && bad->isBad(a.get()))		// if bad transition
						region.push(t.target);
					else
					{
						if (H.expand(t.target) || !H.contains(t.target))	// if not visited yet
							stack.dfsPush(t.target, a);
						else if (H.get(t.target) > 0)						// if on the dfs stack..
						{
							stack.merge(H.get(t.target), a);

							/*
							cout << "merged:  ";
							stack.topSCC()->acc_fair->dump(cout);
							cout << endl;
							*/


							if ( fairMap.satisfiedFairSet(stack.topSCC()->acc_fair.get()) )
							{
								return new SCC(*stack.topSCC());
							}
						}
					}
				}
				else	// SCC pop
				{
					auto_ptr<FairSet::Bad> new_bad(makeNewBadGoal(stack.topSCC()->acc_fair, bad));
					//
					//	If there is a new bad goal, try to revisit the SCC
					//
					if (new_bad.get() != NULL)
					{
						queue<State> new_region;
						new_region.push(stack.sccPop(true));	// pop the top SCC with *unvisit*
						SCC* scc = findAcceptedSCC(new_region, new_bad.get());
						if (scc != NULL)
							return scc;
					}
					else
						stack.sccPop();
				}
			}
		}
	}
	return NULL;
}

FairSet::Bad*
StreettModelChecker::makeNewBadGoal(const auto_ptr<FairSet>& fair, const FairSet::Bad* old)
{
	if (fair.get() != NULL)
	{
		auto_ptr<FairSet::Bad> new_bad(fair->makeBadGoal());
		if ( !new_bad->empty())
		{
			if (old != NULL)
				new_bad->merge(old);
			return new_bad.release();
		}
	}
	return NULL;
}

}
