/*

    This file is part of the Maude 2 interpreter.

    Copyright 1997-2003 SRI International, Menlo Park, CA 94025, USA.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef _LTLRFAIRNESSCHECKERSYMBOL_HH
#define _LTLRFAIRNESSCHECKERSYMBOL_HH
#include "Automaton/StateTransitionDagMap.hh"
#include "Automaton/SystemGraph.hh"
#include "Automaton/SEProductAutomaton.hh"
#include "ModelChecker/ModelChecker.hh"
#include "Fairness/StateRealizedFairGenerator.hh"
#include "Interface/SETemporalFormulaBuilder.hh"
#include "Interface/ProofTermGenerator.hh"
#include "Interface/CounterExampleGenerator.hh"
#include "Interface/PropLabelChecker.hh"
#include "Interface/FairnessDecoder.hh"

class LTLRFairnessCheckerSymbol : public FreeSymbol,
								  private ltlrModelChecker::SETemporalFormulaBuilder,
								  private ltlrModelChecker::ProofTermGenerator,
								  private ltlrModelChecker::CounterExampleGenerator,
								  private ltlrModelChecker::PropLabelChecker,
								  private ltlrModelChecker::FairnessDecoder
{
    NO_COPYING(LTLRFairnessCheckerSymbol);
    
public:
    LTLRFairnessCheckerSymbol(int id, int arity);

    bool attachData(const Vector<Sort*>& opDeclaration, const char* purpose, const Vector<const char*>& data);
    bool attachSymbol(const char* purpose, Symbol* symbol);
    bool attachTerm(const char* purpose, Term* term);
    void copyAttachments(Symbol* original, SymbolMap* map);
    void getDataAttachments(const Vector<Sort*>& opDeclaration, Vector<const char*>& purposes, Vector<Vector<const char*> >& data);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);
    void getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms);
    bool eqRewrite(DagNode* subject, RewritingContext& context) throw();
    void postInterSymbolPass();
    void reset();

private:
    struct FairFlag
    {
    	FairFlag();
    	bool hasParamFair;
    	bool hasParamStrongFair;
    	bool hasParamWeakFair;
    	bool hasSEFair;
    };

    void interpreteParamFairFlag(DagNode* fairFlag, FairFlag& flag) const;

    void readStaticFairness(DagNode* fairSetDag, FairFlag& flag, ltlrModelChecker::PropDagNodeSet& atoms,
    		Vector<Bdd>& propWeakFairness, Vector<pair<Bdd,Bdd> >& propStrongFairness);

    template <class Aut> ltlrModelChecker::ProductAutomaton<Aut>*
    createProdAutomaton(bool hasEvent, ltlrModelChecker::SystemGraph* sysGraph, Aut& property) const;

    bool ndfsModelChecking(ltlrModelChecker::LTLRFormula& formula, ltlrModelChecker::SystemGraph* sysGraph,
    		auto_ptr<ltlrModelChecker::ModelChecker>& mc);

    bool fairSccModelChecking(ltlrModelChecker::LTLRFormula& formula, ltlrModelChecker::SystemGraph* sysGraph,
    		auto_ptr<ltlrModelChecker::ModelChecker>& mc,
    		const FairFlag& flag, Vector<Bdd>& propWeakFairness, Vector<pair<Bdd,Bdd> >& propStrongFairness,
    		ltlrModelChecker::StateRealizedFairGenerator* fGenerator);

    Symbol* noParamFlagSymbol;
    Symbol* paramFlagSymbol;
    Symbol* stateParamFlagSymbol;
    Symbol* seParamFlagSymbol;
    Symbol* justParamFlagSymbol;
    Symbol* fairParamFlagSymbol;
    Symbol* jfParamFlagSymbol;
};

inline
LTLRFairnessCheckerSymbol::FairFlag::FairFlag():
hasParamFair(false),hasParamStrongFair(false),hasParamWeakFair(false),hasSEFair(false) {}

template <class Aut> inline ltlrModelChecker::ProductAutomaton<Aut>*
LTLRFairnessCheckerSymbol::createProdAutomaton(bool hasEvent, ltlrModelChecker::SystemGraph* sysGraph, Aut& property) const
{
	using namespace ltlrModelChecker;
	if (hasEvent)
	{
		Verbose("ModelChecker: LTLR model checking with state/event-based algorithm...");
		SESystemAutomaton* seGraph = static_cast<SESystemAutomaton*>(sysGraph);
		return new SEProductAutomaton<Aut>(*seGraph, property);	// State/Event
	}
	else
	{
		Verbose("ModelChecker: LTL model checking with state-based algorithm...");
		return new ProductAutomaton<Aut>(*sysGraph, property);	// State-only
	}

}

#endif        /* _LTLRFAIRNESSCHECKERSYMBOL_HH */
