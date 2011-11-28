

//      utility stuff
#include <map>
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
#include "rewriteSequenceSearch.hh"

//      free theory class definitions
#include "freeDagNode.hh"

//		built in class definitions
#include "bindingMacros.hh"

//		temporal class definitions
#include "logicFormula.hh"

//		ltlr modelchecker
#include "Interface/definitions.hh"
#include "Interface/ProofTermGenerator.hh"
#include "ModelChecker/NDFSModelChecker.hh"
#include "ModelChecker/SCCBuchiModelChecker.hh"
#include "ModelChecker/StreettModelChecker.hh"

//		fairness
#include "Fairness/FormulaFairnessMap.hh"
#include "Fairness/SystemWeakFairnessMap.hh"
#include "Fairness/SystemStrongFairnessMap.hh"
#include "Fairness/SystemParamWeakFairnessMap.hh"
#include "Fairness/SystemParamStrongFairnessMap.hh"
#include "Fairness/CompositedFairnessMap.hh"

#include "ltlrFairnessCheckerSymbol.hh"


//#define TDEBUG

LTLRFairnessCheckerSymbol::LTLRFairnessCheckerSymbol(int id, int arity)
  : FreeSymbol(id, arity),
    ltlrModelChecker::FairnessDecoder(this), 
    noParamFlagSymbol(NULL), paramFlagSymbol(NULL), 
    stateParamFlagSymbol(NULL), seParamFlagSymbol(NULL), 
    justParamFlagSymbol(NULL), fairParamFlagSymbol(NULL), 
    jfParamFlagSymbol(NULL) {}

bool
LTLRFairnessCheckerSymbol::attachData(const Vector<Sort*>& opDeclaration,
                              const char* purpose,
                              const Vector<const char*>& data)
{
    NULL_DATA(purpose, LTLRFairnessCheckerSymbol, data);
    return  FreeSymbol::attachData(opDeclaration, purpose, data);
}

bool
LTLRFairnessCheckerSymbol::attachSymbol(const char* purpose, Symbol* symbol)
{
    // fairness flag
    BIND_SYMBOL(purpose, symbol, noParamFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, paramFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, stateParamFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, seParamFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, justParamFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, fairParamFlagSymbol, Symbol*);
    BIND_SYMBOL(purpose, symbol, jfParamFlagSymbol, Symbol*);
	if (SETemporalFormulaBuilder::attachSymbol(purpose, symbol)) return true;
	if (ProofTermGenerator::attachSymbol(purpose, symbol))		 return true;
	if (CounterExampleGenerator::attachSymbol(purpose, symbol))	 return true;
	if (PropLabelChecker::attachSymbol(purpose, symbol))	 	 return true;
	if (FairnessDecoder::attachSymbol(purpose, symbol))	 		 return true;
	return FreeSymbol::attachSymbol(purpose, symbol);
}

bool
LTLRFairnessCheckerSymbol::attachTerm(const char* purpose, Term* term)
{
	if (ProofTermGenerator::attachTerm(purpose, term))		return true;
	if (CounterExampleGenerator::attachTerm(purpose, term))	return true;
	if (PropLabelChecker::attachTerm(purpose, term))		return true;
    return FreeSymbol::attachTerm(purpose, term);
}

void
LTLRFairnessCheckerSymbol::copyAttachments(Symbol* original, SymbolMap* map)
{
	LTLRFairnessCheckerSymbol* orig = safeCast(LTLRFairnessCheckerSymbol*, original);
    COPY_SYMBOL(orig, noParamFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, paramFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, stateParamFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, seParamFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, justParamFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, fairParamFlagSymbol, map, Symbol*);
    COPY_SYMBOL(orig, jfParamFlagSymbol, map, Symbol*);
	SETemporalFormulaBuilder::copyAttachments(orig, map);
	ProofTermGenerator::copyAttachments(orig, map);
	CounterExampleGenerator::copyAttachments(orig, map);
	PropLabelChecker::copyAttachments(orig, map);
	FairnessDecoder::copyAttachments(orig, map);
	FreeSymbol::copyAttachments(original, map);
}

void
LTLRFairnessCheckerSymbol::getDataAttachments(const Vector<Sort*>& opDeclaration,
                                       Vector<const char*>& purposes,
                                       Vector<Vector<const char*> >& data)
{
    APPEND_DATA(purposes, data, LTLRFairnessCheckerSymbol);
    FreeSymbol::getDataAttachments(opDeclaration, purposes, data);
}

void
LTLRFairnessCheckerSymbol::getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols)
{
    APPEND_SYMBOL(purposes, symbols, noParamFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, paramFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, stateParamFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, seParamFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, justParamFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, fairParamFlagSymbol);
    APPEND_SYMBOL(purposes, symbols, jfParamFlagSymbol);
	SETemporalFormulaBuilder::getSymbolAttachments(purposes,symbols);
	ProofTermGenerator::getSymbolAttachments(purposes, symbols);
	CounterExampleGenerator::getSymbolAttachments(purposes, symbols);
	PropLabelChecker::getSymbolAttachments(purposes, symbols);
	FairnessDecoder::getSymbolAttachments(purposes, symbols);
	FreeSymbol::getSymbolAttachments(purposes, symbols);
}

void
LTLRFairnessCheckerSymbol::getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms)
{
	ProofTermGenerator::getTermAttachments(purposes, terms);
	CounterExampleGenerator::getTermAttachments(purposes, terms);
	PropLabelChecker::getTermAttachments(purposes, terms);
    FreeSymbol::getTermAttachments(purposes, terms);
}

void
LTLRFairnessCheckerSymbol::postInterSymbolPass()
{
	ProofTermGenerator::postInterSymbolPass();
	CounterExampleGenerator::postInterSymbolPass();
	PropLabelChecker::postInterSymbolPass();
    FreeSymbol::postInterSymbolPass();
}

void
LTLRFairnessCheckerSymbol::reset()
{
	ProofTermGenerator::reset();
	CounterExampleGenerator::reset();
	PropLabelChecker::reset();
    FreeSymbol::reset();  // parents reset() tasks
}

bool
LTLRFairnessCheckerSymbol::eqRewrite(DagNode* subject, RewritingContext& context) throw()
{
    Assert(this == subject->symbol(), "bad symbol");

    // TODO: further optimization, e.g.,
    //  [a] free state/proofterm dag space if it is not required anymore (when it is poped..)
    //  [b] visit all transitions first for each state in order not to save proof-terms
    //  [c] fairness cache + state-space garbage collection (use [a])

    using namespace ltlrModelChecker;
    //
    //	arguments
    //
    FreeDagNode* d = safeCast(FreeDagNode*, subject);
    auto_ptr<RewritingContext> sysContext(context.makeSubcontext(d->getArgument(0)));
    auto_ptr<RewritingContext> formulaContext(context.makeSubcontext(negate(d->getArgument(1))));
    auto_ptr<RewritingContext> fairnessContext(context.makeSubcontext(d->getArgument(2)));
    auto_ptr<RewritingContext> flagContext(context.makeSubcontext(d->getArgument(3)));
    //
    //	formulas
    //
    PropDagNodeSet atoms;
    LTLRFormula formula;
    //
    //	graphs
    //
    bool result;
    auto_ptr<SystemGraph> sysGraph;
    auto_ptr<ModelChecker> mc;
    StateTransitionDagMap* dagMap;	// to show counter example..
    //
    //	fairness
    //
    FairFlag flag;
    Vector<Bdd> propWeakFairness;				// weak fairness sets
    Vector<pair<Bdd,Bdd> > propStrongFairness;	// strong fairness sets

    //
	//	Compute normalization of negated formula.
	//
	formulaContext->reduce();
	context.addInCount(*formulaContext);
#ifdef TDEBUG
	cout << "Negated formula: " << formulaContext->root() << endl;
#endif
	//
	//	Convert Dag into a LogicFormula.
	//
	try
	{
		build(formula, atoms, formulaContext->root());
	}
	catch ( InvalidDagException& e)
	{
		IssueAdvisory("negated LTL formula " << QUOTE(e.getDag()) <<
				" did not reduce to a valid negative normal form: " << e.what());
		return FreeSymbol::eqRewrite(subject, context);
	}
#ifdef TDEBUG
	cout << "top = " << formula.top << endl;
	formula.formula.dump(cout);
#endif

    //
    //	Interpret  fairness conditions
    //
	fairnessContext->reduce();
	context.addInCount(*fairnessContext);
	readStaticFairness(fairnessContext->root(), flag, atoms, propWeakFairness, propStrongFairness);

	flagContext->reduce();
	context.addInCount(*flagContext);
	interpreteParamFairFlag(flagContext->root(), flag);

    //
    //	construct system automaton. (system/event version created if there exist system/event properties.)
    //
    if (formula.event || flag.hasSEFair)
    {
    	SESystemAutomaton* sys = new SESystemAutomaton(sysContext.get(), atoms, this, this);
    	sysGraph.reset(sys);
    	dagMap = sys;
    }
    else
    {
    	SystemAutomaton* sys =  new SystemAutomaton(sysContext.get(), atoms, this, this);
    	sysGraph.reset(sys);
    	dagMap = sys;
    }

    //
    //        Do the model check
    //
    if (propWeakFairness.size() > 0 || propStrongFairness.size() > 0 || flag.hasParamFair)	// fairness
    {
    	auto_ptr<StateRealizedFairGenerator> srfm;
    	if (flag.hasParamFair)
    		srfm.reset(new StateRealizedFairGenerator(dagMap, &context, atoms, this));
	    result = fairSccModelChecking(formula, sysGraph.get(), mc, flag, propWeakFairness, propStrongFairness, srfm.get());
    }
    else	// If no fairness, use NDFS model checking
    {
    	result = ndfsModelChecking(formula, sysGraph.get(), mc);
    }

    //
    //	Generate a counter example
    //	FIXME: use a new graph to generate different transition??
    //
    DagNode* resultDag = result
    		? this->makeCounterexample(dagMap, mc->getLeadIn(), mc->getCycle()) : getTrueDag();

    //
    // print state space size..
    //
    if (globalVerboseFlag)
    {
		int nrSystemStates = sysGraph->getNrStates();
		int totalNrTransitions = 0;
		for (int i=0; i < nrSystemStates; i++)
			totalNrTransitions += sysGraph->getNrTransitions(i);

		cout << "ModelChecker: Examined " << nrSystemStates <<
				" system state" << pluralize(nrSystemStates) << " and " <<
				totalNrTransitions << " transition" << pluralize(totalNrTransitions) << '.' << endl;
    }

    context.addInCount(*sysContext);
    return context.builtInReplace(subject, resultDag);
}

void
LTLRFairnessCheckerSymbol::interpreteParamFairFlag(DagNode* fairFlagDag, FairFlag& flag) const
{
	Assert(noParamFlagSymbol != NULL, "param flag symbols are not attached.");
	if (fairFlagDag->symbol() != noParamFlagSymbol)
	{
		flag.hasParamFair = true;
		DagArgumentIterator fi(fairFlagDag);
		if (fi.valid())
		{
			if (fi.argument()->symbol() == seParamFlagSymbol)
				flag.hasSEFair = true;
			fi.next();
			if (fi.valid())
			{
				if (fi.argument()->symbol() == jfParamFlagSymbol)
				{
					flag.hasParamStrongFair = true;
					flag.hasParamWeakFair = true;
				}
				else if (fi.argument()->symbol() == fairParamFlagSymbol)
					flag.hasParamStrongFair = true;
				else if (fi.argument()->symbol() == justParamFlagSymbol)
					flag.hasParamWeakFair = true;
			}
		}
		Assert(flag.hasParamStrongFair || flag.hasParamWeakFair, "Invalid Param Fair Flag!");
	}
}

void
LTLRFairnessCheckerSymbol::readStaticFairness(DagNode* fairSetDag, FairFlag& flag, ltlrModelChecker::PropDagNodeSet& atoms,
		Vector<Bdd>& propWeakFairness, Vector<pair<Bdd,Bdd> >& propStrongFairness)
{
	Vector<DagNode*> fairDags;
	interpreteFairnessSet(fairSetDag, fairDags);

	FOR_EACH_CONST(i, Vector<DagNode*>, fairDags)
	{
		pair<Bdd,Bdd> res;
		pair<bool, pair<bool,bool> > result = interpreteFairnessDag(*i, atoms, res);
		flag.hasSEFair |= result.first;
		if (res.first == bdd_true())
			propWeakFairness.append(res.second);	// weak fairness
		else
			propStrongFairness.append(res);		// strong fairness
	}
}


bool
LTLRFairnessCheckerSymbol::ndfsModelChecking(ltlrModelChecker::LTLRFormula& formula, ltlrModelChecker::SystemGraph* sysGraph,
											 auto_ptr<ltlrModelChecker::ModelChecker>& mc)
{
	using namespace ltlrModelChecker;
	BuchiAutomaton2 propertyAut(&formula.formula, formula.top);
	auto_ptr<ProductAutomaton<BuchiAutomaton2> > prod(createProdAutomaton(formula.event, sysGraph, propertyAut));
	Verbose("ModelChecker: Use NDFS algorithm with Buchi automaton (" << propertyAut.getNrStates() << " states).");

	mc.reset(new NDFSModelChecker(*prod.get()));
	return mc->findCounterExample();
}

bool
LTLRFairnessCheckerSymbol::fairSccModelChecking(ltlrModelChecker::LTLRFormula& formula, ltlrModelChecker::SystemGraph* sysGraph,
		auto_ptr<ltlrModelChecker::ModelChecker>& mc, const FairFlag& flag,
		Vector<Bdd>& propWeakFairness, Vector<pair<Bdd,Bdd> >& propStrongFairness,
		ltlrModelChecker::StateRealizedFairGenerator* fGenerator)
{
	using namespace ltlrModelChecker;
	GenBuchiAutomaton propertyAut(&formula.formula, formula.top);
	propertyAut.simplify();
	auto_ptr<ProductAutomaton<GenBuchiAutomaton> > prod(
			createProdAutomaton(formula.event|flag.hasSEFair, sysGraph, propertyAut));
	Verbose("ModelChecker: Use SCC-based algorithm with Gen-Buchi automaton (" << propertyAut.getNrStates() << " states).");

	//
	// fairness Maps
	//
	bool strong_fair = false;

	FormulaFairnessMap ffm(propertyAut);
	auto_ptr<SystemWeakFairnessMap> wfm;
	auto_ptr<SystemStrongFairnessMap> sfm;
	auto_ptr<SystemParamWeakFairnessMap> pwfm;
	auto_ptr<SystemParamStrongFairnessMap> psfm;

	CompositedFairnessMap cfm;
	cfm.addFairnessMap(&ffm);				// Formula Buchi

	if (propWeakFairness.size() > 0)
	{
		wfm.reset(new SystemWeakFairnessMap(*sysGraph, propWeakFairness));
		cfm.addFairnessMap(wfm.get());		// Buchi
	}
	if (propStrongFairness.size() > 0)
	{
		sfm.reset(new SystemStrongFairnessMap(*sysGraph, propStrongFairness));
		cfm.addFairnessMap(sfm.get());		// Streett
		strong_fair = true;
	}
	if (flag.hasParamWeakFair)
	{
		pwfm.reset(new SystemParamWeakFairnessMap(*sysGraph, *fGenerator));
		cfm.addFairnessMap(pwfm.get());			// Buchi
	}
	if (flag.hasParamStrongFair)
	{
		psfm.reset(new SystemParamStrongFairnessMap(*sysGraph, *fGenerator));
		cfm.addFairnessMap(psfm.get());			// Streett
		strong_fair = true;
	}

	//
	// If there are strong or param fairness conditions, do Streett model checking
	//
	if (strong_fair)
	{
		Verbose("ModelChecker: Use Streett Model Checker..");
		mc.reset(new StreettModelChecker(*prod.get(), cfm));
	}
	else
	{
		Verbose("ModelChecker: Use Buchi Model Checker..");
		mc.reset(new SCCBuchiModelChecker(*prod.get(), cfm));
	}

	bool result = mc->findCounterExample();

	if (fGenerator != NULL)
	{
		Verbose("ModelChecker: Total " << fGenerator->getNrWeakFairness() <<
					" weak fairness condition" << pluralize(fGenerator->getNrWeakFairness()) << " and " <<
					fGenerator->getNrStrongFairness() << " strong fairness condition"
					<< pluralize(fGenerator->getNrStrongFairness()));

#ifdef TDEBUG
		for (int k = 0; k < fGenerator->getNrWeakFairness() + fGenerator->getNrStrongFairness(); ++k)
			cout << k << " : " <<  fGenerator->getFairnessDag(k) << endl;
#endif
	}

    return result;
}


