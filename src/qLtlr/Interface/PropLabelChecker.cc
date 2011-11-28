/*
 * LabelSymbolChecker.cc
 *
 *  Created on: Dec 3, 2010
 *      Author: kquine
 */

//      utility stuff
#include <memory>
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"

//      core class definitions
#include "rewritingContext.hh"
#include "symbolMap.hh"

//		built in class definitions
#include "bindingMacros.hh"

#include "PropLabelChecker.hh"

namespace ltlrModelChecker {


PropLabelChecker::PropLabelChecker(): satisfiesSymbol(NULL), actionmatchSymbol(NULL) {}

FastBool
PropLabelChecker::computeLabel(DagNode* target, DagNode* prop, Symbol* checkSymbol, RewritingContext* context)
{
	static Vector<DagNode*> args(2);
	args[0] = target;
	args[1] = prop;
	DagNode* checkDag = checkSymbol->makeDagNode(args);

	//cout << checkDag << " is reduced to ";

	const auto_ptr<RewritingContext> testContext(context->makeSubcontext(checkDag));
	testContext->reduce();
	bool result = trueTerm.getDag()->equal(testContext->root());

	// cout << (result ? "true" : "false") << endl;

	context->addInCount(*testContext);
	return result;
}

bool
PropLabelChecker::attachSymbol(const char* purpose, Symbol* symbol)
{
    BIND_SYMBOL(purpose, symbol, satisfiesSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, actionmatchSymbol, Symbol*);
    return false;
}

bool
PropLabelChecker::attachTerm(const char* purpose, Term* term)
{
    BIND_TERM(purpose, term, trueTerm);
    return false;
}

void
PropLabelChecker::copyAttachments(PropLabelChecker* orig, SymbolMap* map)
{
    COPY_SYMBOL(orig, satisfiesSymbol, map, Symbol*);
    COPY_SYMBOL(orig, actionmatchSymbol, map, Symbol*);
    COPY_TERM(orig, trueTerm, map);
}

void
PropLabelChecker::getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols)
{
    APPEND_SYMBOL(purposes, symbols, satisfiesSymbol);
    APPEND_SYMBOL(purposes, symbols, actionmatchSymbol);
}

void
PropLabelChecker::getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms)
{
    APPEND_TERM(purposes, terms, trueTerm);
}

void
PropLabelChecker::postInterSymbolPass()
{
    PREPARE_TERM(trueTerm);
}

void
PropLabelChecker::reset()
{
    trueTerm.reset();  // so true dag can be garbage collected
}

}
