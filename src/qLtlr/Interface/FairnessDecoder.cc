/*
 * FairnessDecoder.cc
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */
//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "temporal.hh"
#include "interface.hh"
#include "core.hh"
#include "higher.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"
#include "dagArgumentIterator.hh"

//		temporal class definitions
#include "logicFormula.hh"

//		higher class definitions
#include "stateTransitionGraph.hh"

//		core class definitions
#include "symbolMap.hh"

//		built in class definitions
#include "bindingMacros.hh"

#include "definitions.hh"
#include "FairnessDecoder.hh"


namespace ltlrModelChecker {

FairnessDecoder::FairnessDecoder(SETemporalFormulaBuilder* seTemporalSymbol):
		realizedTestSymbol(NULL), realizedResultSymbol(NULL),
		fairnessSymbol(NULL), fairnessSetSymbol(NULL), emptyFairnessSetSymbol(NULL),
		strongFairTypeSymbol(NULL), weakFairTypeSymbol(NULL), seTemporalSymbol(seTemporalSymbol) {}

pair<bool,pair<bool,bool> >
FairnessDecoder::interpreteFairnessDag(DagNode* fairnessDag, PropDagNodeSet& propositions, pair<Bdd,Bdd>& fairness)
	const throw(InvalidDagException)
{
	if (fairnessDag->symbol() != fairnessSymbol)
		throw InvalidDagException("Invalid Top", fairnessDag);

	LTLRFormula formula1, formula2;
	bool leftRes, rightRes;
	DagArgumentIterator i(*fairnessDag);
	//
	//	fairness type
	//
	if (!i.valid())
		throw InvalidDagException("Invalid Type", fairnessDag);
	Symbol* fairType = i.argument()->symbol();
	//
	//	left condition
	//
	i.next();
	if (!i.valid())
		throw InvalidDagException("Invalid Fairness Formula", fairnessDag);
	seTemporalSymbol->build(formula1, propositions, i.argument());
	try {
		leftRes = translateFormula(formula1.top, formula1.formula, fairness.first);
	}
	catch (invalid_argument& e) {
		throw InvalidDagException("Invalid Fairness Formula", i.argument());
	}
	//
	//	right condition
	//
	i.next();
	if (!i.valid())
		throw InvalidDagException("Invalid Fairness Formula", fairnessDag);
	seTemporalSymbol->build(formula2, propositions, i.argument());
	try {
		rightRes = translateFormula(formula2.top, formula2.formula, fairness.second);
	}
	catch (invalid_argument& e) {
		throw InvalidDagException("Invalid Fairness Formula", i.argument());
	}
	//
	//	generate bdds
	//
	if (fairType == weakFairTypeSymbol)
	{
		fairness.second = fairness.first >> fairness.second;
		fairness.first = bdd_true();
		return make_pair(formula1.event | formula2.event, make_pair(true, ~leftRes | rightRes));
	}
	else if (fairType == strongFairTypeSymbol)
		return make_pair(formula1.event | formula2.event, make_pair(leftRes, rightRes));
	else
		throw InvalidDagException("Invalid Type", fairnessDag);
}

void
FairnessDecoder::interpreteFairnessSet(DagNode* fairnessSetDag, Vector<DagNode*>& result) const
{
	if (fairnessSetDag->symbol() == fairnessSymbol)
		result.append(fairnessSetDag);
	else if (fairnessSetDag->symbol() == fairnessSetSymbol)
	{
		DagArgumentIterator fi(fairnessSetDag);
		while (fi.valid())
		{
			result.append(fi.argument());
			fi.next();
		}
	}
}

void
FairnessDecoder::computeStateRealizedFairDags(DagNode* stateDag, RewritingContext* context, Vector<DagNode*>& result) const
{
	Assert(realizedTestSymbol != NULL && realizedResultSymbol != NULL, "Param Symbols are not attached");

	static Vector<DagNode*> args(1);
	args[0] = stateDag;
	DagNode* testDag = realizedTestSymbol->makeDagNode(args);
	RewritingContext* realContext = context->makeSubcontext(testDag);
	::StateTransitionGraph search(realContext); // NOTE: destructor will also free realContext

	// find all normal forms by rewriting
	for(int explore = 0; explore < search.getNrStates(); ++explore)
	{
		int nextStateNr = NONE;
		int nextArc = 0;
		while ((nextStateNr = search.getNextState(explore,nextArc)) != NONE)
			++nextArc;

		if (search.getContext()->traceAbort())
			break;

		if (nextArc == 0)	// normal form
		{
			DagNode* s = search.getStateDag(explore);
			if (s->symbol() == realizedResultSymbol)	// if it is a realized fairness condition
			{
				DagArgumentIterator i(s);
				Assert(i.valid(), "Invalid realized result symbol");
				result.append(i.argument());
			}
		}
	}
}

FastBool
FairnessDecoder::translateFormula(int subformulaIndex, const LogicFormula& formula, Bdd& result) const throw(invalid_argument)
{
	LogicFormula::Op op = formula.getOp(subformulaIndex);
	switch (op)
	{
	case LogicFormula::PROPOSITION:
	{
		result = ithvar(formula.getProp(subformulaIndex));
		return false;
	}
	case LogicFormula::LTL_TRUE:
	{
		result = bdd_true();
		return true;
	}
	case LogicFormula::LTL_FALSE:
	{
		result = bdd_false();
		return false;
	}
	case LogicFormula::NOT:
	{
		result = nithvar(formula.getProp(formula.getArg(subformulaIndex, 0)));
		return true;
	}
	case LogicFormula::AND:
	case LogicFormula::OR:
	{
		Bdd left, right;
		bool leftRes  = translateFormula(formula.getArg(subformulaIndex,0), formula, left);
		bool rightRes = translateFormula(formula.getArg(subformulaIndex,1), formula, right);

		if (op == LogicFormula::AND)
		{
			result = left & right;
			return leftRes & rightRes;
		}
		else
		{
			result = left | right;
			return leftRes | rightRes;
		}
	}
	default:
		throw invalid_argument("Fairness formula cannot contains temporal operator");
	}
}


bool
FairnessDecoder::attachSymbol(const char* purpose, Symbol* symbol)
{
	// fairness
    BIND_SYMBOL(purpose, symbol, fairnessSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, strongFairTypeSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, weakFairTypeSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, fairnessSetSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, emptyFairnessSetSymbol, Symbol*);

    // realized-map
    BIND_SYMBOL(purpose, symbol, realizedTestSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, realizedResultSymbol, Symbol*);
    return false;
}

void
FairnessDecoder::copyAttachments(FairnessDecoder* orig, SymbolMap* map)
{
	// fairness
	COPY_SYMBOL(orig, fairnessSymbol, map, Symbol*);
	COPY_SYMBOL(orig, strongFairTypeSymbol, map, Symbol*);
	COPY_SYMBOL(orig, weakFairTypeSymbol, map, Symbol*);
	COPY_SYMBOL(orig, fairnessSetSymbol, map, Symbol*);
	COPY_SYMBOL(orig, emptyFairnessSetSymbol, map, Symbol*);

    // realized-map
	COPY_SYMBOL(orig, realizedTestSymbol, map, Symbol*);
	COPY_SYMBOL(orig, realizedResultSymbol, map, Symbol*);
}

void
FairnessDecoder::getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols)
{
	// fairness
    APPEND_SYMBOL(purposes, symbols, fairnessSymbol);
    APPEND_SYMBOL(purposes, symbols, strongFairTypeSymbol);
    APPEND_SYMBOL(purposes, symbols, weakFairTypeSymbol);
    APPEND_SYMBOL(purposes, symbols, fairnessSetSymbol);
    APPEND_SYMBOL(purposes, symbols, emptyFairnessSetSymbol);

    // realized map
    APPEND_SYMBOL(purposes, symbols, realizedTestSymbol);
    APPEND_SYMBOL(purposes, symbols, realizedResultSymbol);
}


}
