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
//      Implementation for class TemporalSymbol.
//

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"

//      core class definitions
#include "dagArgumentIterator.hh"
#include "symbolMap.hh"

//		built in class definitions
#include "bindingMacros.hh"

//	temporal class definitions
#include "logicFormula.hh"

#include "SETemporalFormulaBuilder.hh"

namespace ltlrModelChecker
{

SETemporalFormulaBuilder::SETemporalFormulaBuilder()
  : trueSymbol(NULL), falseSymbol(NULL),
    notSymbol(NULL), nextSymbol(NULL), andSymbol(NULL), orSymbol(NULL),
    untilSymbol(NULL), releaseSymbol(NULL), tauSymbol(NULL) {}


int
SETemporalFormulaBuilder::build(LTLRFormula& f, PropDagNodeSet& propositions, DagNode* dagNode) const
	throw(InvalidDagException)
{
	Symbol* s = dagNode->symbol();
	if (s == trueSymbol)
		return (f.top = f.formula.makeOp(LogicFormula::LTL_TRUE));
	else if (s == falseSymbol)
		return (f.top = f.formula.makeOp(LogicFormula::LTL_FALSE));
	else if (s == notSymbol || s == nextSymbol)
	{
		LogicFormula::Op op = (s == notSymbol) ? LogicFormula::NOT : LogicFormula::NEXT;
		DagArgumentIterator i(*dagNode);
		if (i.valid())
		{
			int a = build(f, propositions, i.argument());
			if (op != LogicFormula::NOT || f.formula.getOp(f.top) == LogicFormula::PROPOSITION)
				return (f.top = f.formula.makeOp(op, a));
		}
		throw InvalidDagException("NOT operator", dagNode);
	}
	else if (s == andSymbol || s == orSymbol)
	{
		LogicFormula::Op op = (s == andSymbol) ? LogicFormula::AND : LogicFormula::OR;
		DagArgumentIterator i(*dagNode);
		if (i.valid())
		{
			int a = build(f, propositions, i.argument());
			i.next();
			if (i.valid())
			{
				int b;
				for (;;)
				{
					b = build(f, propositions, i.argument());
					a = (f.top = f.formula.makeOp(op, a, b));
					i.next();
					if (!(i.valid()))
						return a;
				}
			}
		}
		throw InvalidDagException( (s == andSymbol ? "AND operator" : "OR operator"), dagNode);
	}
	else if (s == untilSymbol || s == releaseSymbol)
	{
		LogicFormula::Op op = (s == untilSymbol) ? LogicFormula::UNTIL : LogicFormula::RELEASE;
		DagArgumentIterator i(*dagNode);
		if (i.valid())
		{
			int a = build(f, propositions, i.argument());
			i.next();
			if (i.valid())
			{
				int b = build(f, propositions, i.argument());
				return (f.top = f.formula.makeOp(op, a, b));
			}
		}
		throw InvalidDagException( (s == untilSymbol ? "UNTIL operator" : "RELEASE operator"), dagNode);
	}
	else
	{
		int propIndex = propositions.dagNode2Index(dagNode);
		if (propIndex == NONE)
		{
			propIndex = propositions.insert(dagNode);
			if ( isEventProp(dagNode) )
			{
				propositions.setEvent(propIndex);
				f.event = true;
			}
		}
		return (f.top = f.formula.makeProp(propIndex));
	}
}

bool
SETemporalFormulaBuilder::attachSymbol(const char* purpose, Symbol* symbol)
{
	BIND_SYMBOL(purpose, symbol, tauSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, trueSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, falseSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, notSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, nextSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, andSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, orSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, untilSymbol, Symbol*);
	BIND_SYMBOL(purpose, symbol, releaseSymbol, Symbol*);
	return false;
}


void
SETemporalFormulaBuilder::copyAttachments(SETemporalFormulaBuilder* orig, SymbolMap* map)
{
	COPY_SYMBOL(orig, tauSymbol, map, Symbol*);
	COPY_SYMBOL(orig, trueSymbol, map, Symbol*);
	COPY_SYMBOL(orig, falseSymbol, map, Symbol*);
	COPY_SYMBOL(orig, notSymbol, map, Symbol*);
	COPY_SYMBOL(orig, nextSymbol, map, Symbol*);
	COPY_SYMBOL(orig, andSymbol, map, Symbol*);
	COPY_SYMBOL(orig, orSymbol, map, Symbol*);
	COPY_SYMBOL(orig, untilSymbol, map, Symbol*);
	COPY_SYMBOL(orig, releaseSymbol, map, Symbol*);
}



void
SETemporalFormulaBuilder::getSymbolAttachments(Vector<const char*>& purposes, Vector<Symbol*>& symbols)
{
	APPEND_SYMBOL(purposes, symbols, tauSymbol);
	APPEND_SYMBOL(purposes, symbols, trueSymbol);
	APPEND_SYMBOL(purposes, symbols, falseSymbol);
	APPEND_SYMBOL(purposes, symbols, notSymbol);
	APPEND_SYMBOL(purposes, symbols, nextSymbol);
	APPEND_SYMBOL(purposes, symbols, andSymbol);
	APPEND_SYMBOL(purposes, symbols, orSymbol);
	APPEND_SYMBOL(purposes, symbols, untilSymbol);
	APPEND_SYMBOL(purposes, symbols, releaseSymbol);
}

}

