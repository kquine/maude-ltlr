/*
 * LabelSymbolChecker.hh
 *
 *  Created on: Dec 3, 2010
 *      Author: kquine
 */

#ifndef PROPLABELCHECKER_HH_
#define PROPLABELCHECKER_HH_
#include "term.hh"
#include "cachedDag.hh"

namespace ltlrModelChecker {

class PropLabelChecker
{
public:
	PropLabelChecker();
	FastBool computeStateLabel(DagNode* target, DagNode* prop, RewritingContext* context);
	FastBool computeEventLabel(DagNode* target, DagNode* prop, RewritingContext* context);
	DagNode* getTrueDag();

protected:
    bool attachSymbol(const char* purpose, Symbol* symbol);
    bool attachTerm(const char* purpose, Term* term);
    void copyAttachments(PropLabelChecker* original, SymbolMap* map);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);
    void getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms);
    void postInterSymbolPass();
    void reset();

private:
    FastBool computeLabel(DagNode* target, DagNode* prop, Symbol* checkSymbol, RewritingContext* context);

    Symbol* satisfiesSymbol;
    Symbol* actionmatchSymbol;
    CachedDag trueTerm;
};

inline FastBool
PropLabelChecker::computeStateLabel(DagNode* target, DagNode* prop, RewritingContext* context)
{
	return computeLabel(target, prop, satisfiesSymbol, context);
}

inline FastBool
PropLabelChecker::computeEventLabel(DagNode* target, DagNode* prop, RewritingContext* context)
{
	return computeLabel(target, prop, actionmatchSymbol, context);
}

inline DagNode*
PropLabelChecker::getTrueDag()
{
	return trueTerm.getDag();
}

}

#endif /* PROPLABELCHECKER_HH_ */
