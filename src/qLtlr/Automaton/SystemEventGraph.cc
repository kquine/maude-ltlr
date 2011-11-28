/*
 * SystemEventGraph.cc
 *
 *  Created on: Dec 3, 2010
 *      Author: kquine
 */

#include "macros.hh"
#include "bdd.hh"
#include "SystemEventGraph.hh"


namespace ltlrModelChecker {

pair<FastBool,Bdd>
SystemEventGraph::satisfiesFormula(Bdd formula, int stateNr)
{
	Bdd rem = formula;
    for(;;)
    {
        if (formula == bdd_true())
        	return make_pair(true, bdd_true()/*rem*/);
        else if (formula == bdd_false())
            return make_pair(false, bdd_false()/*rem*/);
        else
        {
			int propId = bdd_var(formula);
			Bdd prop = bdd_ithvar(propId);
			if (isEventProp(propId))
			{
				return make_pair(true, formula);
				//formula = bdd_exist(formula, prop);
			}
			else
			{
				//rem = bdd_exist(rem, prop);
				formula = checkStateLabel(propId, stateNr) ? bdd_high(formula) : bdd_low(formula);
			}
        }
    }
}

FastBool
SystemEventGraph::satisfiesFormula(Bdd formula, int stateNr, int transitionNr)
{
    for(;;)
    {
        if (formula == bdd_true())
        	return true;
        else if(formula == bdd_false())
            return false;
        else
        {
			int propId = bdd_var(formula);
			if (isEventProp(propId))
				formula = checkEventLabel(propId, stateNr, transitionNr) ? bdd_high(formula) : bdd_low(formula);
			else
				formula = checkStateLabel(propId, stateNr) ? bdd_high(formula) : bdd_low(formula);
        }
    }
}



}
