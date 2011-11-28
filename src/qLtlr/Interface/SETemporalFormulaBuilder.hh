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

//
//      Class for symbols that need to manipulate LTLR formulae.
//
#ifndef _SETEMPORALFORMULABUILDER_HH
#define _SETEMPORALFORMULABUILDER_HH
#include "logicFormula.hh"
#include "definitions.hh"
#include "Util/PropDagNodeSet.hh"

namespace ltlrModelChecker
{

class SETemporalFormulaBuilder
{
	NO_COPYING(SETemporalFormulaBuilder);

public:
	SETemporalFormulaBuilder();
	int build(LTLRFormula& formula, PropDagNodeSet& propositions, DagNode* dagNode) const throw(InvalidDagException);
	DagNode* negate(DagNode* original) const;

protected:
    bool attachSymbol(const char* purpose, Symbol* symbol);
    void copyAttachments(SETemporalFormulaBuilder* original, SymbolMap* map);
    void getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols);

private:
	//
	//	Symbols needed for LTLR formulae.
	//
	Symbol* trueSymbol;
	Symbol* falseSymbol;
	Symbol* notSymbol;
	Symbol* nextSymbol;
	Symbol* andSymbol;
	Symbol* orSymbol;
	Symbol* untilSymbol;
	Symbol* releaseSymbol;
	//
	//  Symbol to characterize an action
	//
	Symbol* tauSymbol;

private:
	FastBool isEventProp(DagNode* propDag) const;

};

inline DagNode*
SETemporalFormulaBuilder::negate(DagNode* original) const
{
	static Vector<DagNode*> arg(1);
	arg[0] = original;
	return notSymbol->makeDagNode(arg);
}

inline FastBool
SETemporalFormulaBuilder::isEventProp(DagNode* propDag) const
{
	Assert(tauSymbol != NULL, "No tau symbol attached");
	Assert(propDag->getSortIndex() != Sort::SORT_UNKNOWN, "The sort of " << QUOTE(propDag) << " is not computed yet.");
	return propDag->leq(tauSymbol->getOpDeclarations()[0].getDomainAndRange()[tauSymbol->arity()]);
}

}

#endif
