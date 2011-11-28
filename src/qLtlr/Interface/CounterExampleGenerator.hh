/*
 * CounterExampleSymbol.hh
 *
 *  Created on: Nov 6, 2010
 *      Author: kquine
 */

#ifndef COUNTEREXAMPLESYMBOL_HH_
#define COUNTEREXAMPLESYMBOL_HH_
#include <list>
#include "term.hh"
#include "cachedDag.hh"
#include "Automaton/StateTransitionDagMap.hh"

namespace ltlrModelChecker {

class CounterExampleGenerator
{
public:
	CounterExampleGenerator();
    DagNode* makeCounterexample(const StateTransitionDagMap* map,
    		const list<pair<int,int> >& path, const list<pair<int,int> >& cycle);

protected:
    bool attachSymbol(const char* purpose, Symbol* symbol);
    bool attachTerm(const char* purpose, Term* term);
    void copyAttachments(CounterExampleGenerator* original, SymbolMap* map);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);
    void getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms);
    void postInterSymbolPass();
    void reset();

private:
    DagNode* makeTransitionList(const StateTransitionDagMap* map, const list<pair<int,int> >& path) const;

    Symbol* counterexampleSymbol;
    Symbol* transitionSymbol;
    Symbol* transitionListSymbol;
    Symbol* nilTransitionListSymbol;
    CachedDag falseTerm;

};

}


#endif /* COUNTEREXAMPLESYMBOL_HH_ */
