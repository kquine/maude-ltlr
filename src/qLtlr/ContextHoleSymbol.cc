/*
 * ContextHoleSymbol.cc
 *
 *  Created on: Dec 9, 2012
 *      Author: kquine
 */

//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"
#include "freeTheory.hh"
#include "strategyLanguage.hh"
#include "mixfix.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"
#include "term.hh"

//      free theory class definitions
#include "freeDagNode.hh"

//		built in class definitions
#include "bindingMacros.hh"

//      front end class definitions
#include "token.hh"
#include "mixfixModule.hh"

#include "ContextHoleSymbol.hh"

ContextHoleSymbol::ContextHoleSymbol(int id): FreeSymbol(id, 0) {}

bool
ContextHoleSymbol::attachData(const Vector<Sort*>& opDeclaration, const char* purpose, const Vector<const char*>& data)
{
	if (strcmp(purpose, "ContextHoleSymbol") == 0)
	{
		for (int i = 0; i < data.length(); ++i)
		{
			crope name(data[i]);
			int id = Token::ropeToPrefixNameCode(name);
			if (id != NONE)
			{
				switch (Token::auxProperty(id))
				{
					case Token::AUX_SORT:
					case Token::AUX_STRUCTURED_SORT:
					{
						Sort* s = safeCast(MixfixModule*,this->getModule())->findSort(id);
						if (s != NULL)
						{
							sortIndices.insert(s->getIndexWithinModule());
							break;
						}
						else
						{
							IssueAdvisory("could not find sort " << QUOTE(Token::name(id)) <<
									" in module " << QUOTE(this->getModule()) << '.');
							return false;
						}
					}
				    default:
				    {
				    	IssueAdvisory("expected sort, not " << QUOTE(Token::name(id)) <<
						      " in module " << (this->getModule()) << '.');
				    	return false;
				    }
				}
			}
		}
		return sortIndices.size() > 0;
	}
	return  FreeSymbol::attachData(opDeclaration, purpose, data);
}

void
ContextHoleSymbol::copyAttachments(Symbol* original, SymbolMap* map)
{
	ContextHoleSymbol* orig = safeCast(ContextHoleSymbol*, original);
	sortIndices.insert(orig->sortIndices);
	FreeSymbol::copyAttachments(original, map);
}

void
ContextHoleSymbol::getDataAttachments(const Vector<Sort*>& opDeclaration, Vector<const char*>& purposes, Vector<Vector<const char*> >& data)
{
	int nrDataAttachments = purposes.length();
	purposes.resize(nrDataAttachments + 1);
	purposes[nrDataAttachments] = "ContextHoleSymbol";
	data.resize(nrDataAttachments + 1);
	data[nrDataAttachments].clear();
	FOR_EACH_CONST(i, NatSet, sortIndices)
	{
		data[nrDataAttachments].append(Token::name(this->getModule()->getSorts()[*i]->id()));
	}
	FreeSymbol::getDataAttachments(opDeclaration, purposes, data);
}


