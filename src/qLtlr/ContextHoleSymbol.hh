/*
 * ContextHoleSymbol.hh
 *
 *  Created on: Dec 9, 2012
 *      Author: kquine
 */

#ifndef CONTEXTHOLESYMBOL_HH_
#define CONTEXTHOLESYMBOL_HH_
#include "freeSymbol.hh"

class ContextHoleSymbol : public FreeSymbol
{
	NO_COPYING(ContextHoleSymbol);
public:
	ContextHoleSymbol(int id);
	bool attachData(const Vector<Sort*>& opDeclaration, const char* purpose, const Vector<const char*>& data);
	void copyAttachments(Symbol* original, SymbolMap* map);
	void getDataAttachments(const Vector<Sort*>& opDeclaration, Vector<const char*>& purposes, Vector<Vector<const char*> >& data);

	const NatSet& getSortIndices() const;
private:
	NatSet sortIndices;
};

inline const NatSet&
ContextHoleSymbol::getSortIndices() const
{
	return sortIndices;
}

#endif /* CONTEXTHOLESYMBOL_HH_ */
