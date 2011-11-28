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
//        Class for building a state transition graph on-the-fly.
//
#ifndef _seStateTransitionGraph_hh_
#define _seStateTransitionGraph_hh_
#include <map>
#include <set>
#include "DagTransitionGraph.hh"

namespace ltlrModelChecker {

class SEStateTransitionGraph : public DagTransitionGraph
{
    //NO_COPYING(SEStateTransitionGraph);

public:
    SEStateTransitionGraph(RewritingContext* initial, ProofTermGenerator* proofTerm);

    DagNode* getTransitionDag(int stateNr, int transitionNr) const;
    int getNextState(int stateNr, int index);
    void dfsDump(ostream&);	// Test code
private:
	struct State: DagTransitionGraph::State
	{
		State(const SEStateTransitionGraph* graph, int hashConsIndex);
		Vector<int> proofIndices;			// transitionNr |-> proofHashIndex
		map<int, set<int> > proofFwdArcs;	// nextStateNr |-> {proofHashIndices}
	};

	int lastProofIndex;					// the last proofterm index in hashCons2dagIndex
};

}
#endif
