/*
 * ProofTermGenerator.hh
 *
 *  Created on: Feb 1, 2011
 *      Author: kquine
 */

#ifndef PROOFTERMGENERATOR_HH_
#define PROOFTERMGENERATOR_HH_
#include "term.hh"
#include "variableTerm.hh"
#include "quotedIdentifierSymbol.hh"
#include "quotedIdentifierDagNode.hh"
#include "cachedDag.hh"

namespace ltlrModelChecker {

class ProofTermGenerator {
public:
	ProofTermGenerator();

    DagNode* makeProofDag(const PositionState* ps, const Rule& rule, const Substitution* subst);
    DagNode* makeDeadlockDag();

protected:
    bool attachSymbol(const char* purpose, Symbol* symbol);
    bool attachTerm(const char* purpose, Term* term);
    void copyAttachments(ProofTermGenerator* original, SymbolMap* map);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);
    void getTermAttachments(Vector<const char*>& purposes, Vector<Term*>& terms);
    void postInterSymbolPass();
    void reset();

private:
    DagNode* makeContextDag(const PositionState* ps, const Sort* holeSort) const;
    DagNode* makeRuleNameDag(int ruleId) const;
    DagNode* makeSubstitutionDag(const Substitution* substitution, const VariableInfo* variableInfoconst) const;
    DagNode* makeAssignmentDag(const Term* variable, DagNode* value, Symbol* assignmentSymbol) const;

    void initOps();

    bool init;
    Vector<Symbol*> assignOps;
    Vector<Symbol*> holeOps;

	Symbol* prooftermSymbol;
    Symbol* substitutionSymbol;
    Symbol* emptySubstSymbol;
    QuotedIdentifierSymbol* qidSymbol;
    Symbol* unlabeledSymbol;
    Symbol* noContextSymbol;
    CachedDag deadlockTerm;
};

}

#endif /* PROOFTERMGENERATOR_HH_ */
