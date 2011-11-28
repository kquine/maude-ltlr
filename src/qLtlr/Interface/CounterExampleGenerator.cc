/*
 * CounterExampleSymbol.cc
 *
 *  Created on: Nov 6, 2010
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"
#include "interface.hh"
#include "core.hh"
#include "freeTheory.hh"
#include "builtIn.hh"
#include "higher.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"
#include "term.hh"

//      core class definitions
#include "dagArgumentIterator.hh"
#include "rewritingContext.hh"
#include "symbolMap.hh"
#include "dagNodeSet.hh"
#include "rule.hh"

//      built in class definitions
#include "bindingMacros.hh"

#include "CounterExampleGenerator.hh"

//#define TDEBUG

namespace ltlrModelChecker {

CounterExampleGenerator::CounterExampleGenerator():
		counterexampleSymbol(NULL), transitionSymbol(NULL), transitionListSymbol(NULL), nilTransitionListSymbol(NULL) {}


DagNode*
CounterExampleGenerator::makeCounterexample(const StateTransitionDagMap* map,
								  const list<pair<int,int> >& path, const list<pair<int,int> >& cycle)
{
#ifdef TDEBUG
	typedef pair<int,int> CTransition;
	cout << "counter example: " << endl;
	cout << "  path: ";
	FOR_EACH_CONST(i, list<CTransition>, path) cout << i->first << "=(" << i->second << ")=> ";
	cout << "\n  cycle: ";
	FOR_EACH_CONST(j, list<CTransition>, cycle) cout << j->first << "=(" << j->second << ")=> ";
	cout << endl;
#endif

	static Vector<DagNode*> args(2);
	args[0] = makeTransitionList(map, path);
	args[1] = makeTransitionList(map, cycle);
	return counterexampleSymbol->makeDagNode(args);
}


DagNode*
CounterExampleGenerator::makeTransitionList(const StateTransitionDagMap* map, const list<pair<int,int> >& path) const
{
    if (path.empty())
        return nilTransitionListSymbol->makeDagNode();
    else
    {
		Vector<DagNode*> args;

		typedef pair<int,int> Transition;
		FOR_EACH_CONST(i, list<Transition>, path)
		{
			static Vector<DagNode*> targs(2);
			targs[0] = map->getStateDag(i->first);
			targs[1] = map->getTransitionDag(i->first, i->second);
			args.append(transitionSymbol->makeDagNode(targs));
		}
		return (args.length() == 1) ? args[0] : transitionListSymbol->makeDagNode(args);
    }
}

bool
CounterExampleGenerator::attachSymbol(const char* purpose, Symbol* symbol)
{
    BIND_SYMBOL(purpose, symbol, transitionSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, transitionListSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, nilTransitionListSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, counterexampleSymbol, Symbol*);
	return false;
}

bool
CounterExampleGenerator::attachTerm(const char* purpose, Term* term)
{
    BIND_TERM(purpose, term, falseTerm);
    return false;
}

void
CounterExampleGenerator::copyAttachments(CounterExampleGenerator* orig, SymbolMap* map)
{
    COPY_TERM(orig, falseTerm, map);
	COPY_SYMBOL(orig, transitionSymbol, map, Symbol*);
	COPY_SYMBOL(orig, transitionListSymbol, map, Symbol*);
	COPY_SYMBOL(orig, nilTransitionListSymbol, map, Symbol*);
	COPY_SYMBOL(orig, counterexampleSymbol, map, Symbol*);
}

void
CounterExampleGenerator::getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols)
{
    APPEND_SYMBOL(purposes, symbols, transitionSymbol);
	APPEND_SYMBOL(purposes, symbols, transitionListSymbol);
	APPEND_SYMBOL(purposes, symbols, nilTransitionListSymbol);
	APPEND_SYMBOL(purposes, symbols, counterexampleSymbol);
}

void
CounterExampleGenerator::getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms)
{
    APPEND_TERM(purposes, terms, falseTerm);
}

void
CounterExampleGenerator::postInterSymbolPass()
{
    PREPARE_TERM(falseTerm);
}

void
CounterExampleGenerator::reset()
{
    falseTerm.reset();	// so false dag can be garbage collected
}

}

