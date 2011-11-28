/*
 * FairnessDecoder.hh
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#ifndef FAIRNESSDECODER_HH_
#define FAIRNESSDECODER_HH_
#include "bddUser.hh"
#include "logicFormula.hh"
#include "Interface/SETemporalFormulaBuilder.hh"

namespace ltlrModelChecker {

//
//	FIXME: do we need to distinguish fairness conditions, if not realized: i) always satisfied ii) never satisfied ??
//         can be passed from user manually, or checked...
//
class FairnessDecoder : private BddUser
{
	enum FairnessType {STRONG_FAIRNESS, WEAK_FAIRNESS};

public:
	FairnessDecoder(SETemporalFormulaBuilder* seTemporalSymbol);

	// compute (supp, concl), where supp is true if weak fairness, and return:
	//  the first component true if the formula contains events
	//  the second component is the pair of boolean result of fairness Bdds when all prop are false
	pair<bool,pair<bool,bool> >
	interpreteFairnessDag(DagNode* fairnessDag, PropDagNodeSet& propositions, pair<Bdd,Bdd>& fairness) const
		throw(InvalidDagException);

	void interpreteFairnessSet(DagNode* fairessSetDag, Vector<DagNode*>& result) const;
	void computeStateRealizedFairDags(DagNode* stateDag, RewritingContext* context, Vector<DagNode*>& result) const;

protected:
    bool attachSymbol(const char* purpose, Symbol* symbol);
    void copyAttachments(FairnessDecoder* original, SymbolMap* map);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);

private:
    // compute the corresponding bdd, and return the boolean result when all props are false (for unrealized cases)..
	FastBool translateFormula(int subformulaIndex, const LogicFormula& formula, Bdd& result) const throw(invalid_argument);

	// symbols for realized fairness
	Symbol* realizedTestSymbol;
	Symbol* realizedResultSymbol;

	// information to interpret fairness
    Symbol* fairnessSymbol;
    Symbol* strongFairTypeSymbol;
    Symbol* weakFairTypeSymbol;
    Symbol* fairnessSetSymbol;
    Symbol* emptyFairnessSetSymbol;
    SETemporalFormulaBuilder* seTemporalSymbol;
};

}

#endif /* FAIRNESSDECODER_HH_ */
