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
//	Implementation for class StateTransitionGraph.
//

//	utility stuff
#include "macros.hh"
#include "vector.hh"

//	forward declarations
#include "interface.hh"
#include "core.hh"
#include "higher.hh"
#include "freeTheory.hh"
#include "strategyLanguage.hh"
#include "mixfix.hh"

//	interface class definitions
#include "symbol.hh"
#include "dagNode.hh"

//	higher class definitions
#include "rewriteSearchState.hh"

#include "stateTransitionGraph.hh"

namespace ltlrModelChecker {


StateTransitionGraph::StateTransitionGraph(RewritingContext* initial, ProofTermGenerator* proofTerm):
		DagTransitionGraph(initial, proofTerm)
{
	initial->reduce();
	State* s= new State(this, hashConsSet.insertDag(initial->root(), 0).first);
	seen.append(s);
}

StateTransitionGraph::State::State(const StateTransitionGraph* graph, int hashConsIndex):
	DagTransitionGraph::State(graph,hashConsIndex) {}

DagNode*
StateTransitionGraph::getTransitionDag(int stateNr, int index) const
{
	Assert(stateNr < getNrStates(), "Invalid state lookup (transition dag)");
	if (index == 0 && getNrTransitions(stateNr) == 0)
		return proofTerm->makeDeadlockDag();
	else
	{
		const map<int, Rule*>& fwdArc = static_cast<const State*>(seen[stateNr])->fwdArc;
		map<int, Rule*>::const_iterator res = fwdArc.find(seen[stateNr]->nextStates[index]);
		Assert(res != fwdArc.end(), "Invalid transition search problem");
		return proofTerm->makeProofDag(NULL, *res->second, NULL);
	}
}

int
StateTransitionGraph::getNextState(int stateNr, int index)
{
	State* n = static_cast<State*>(seen[stateNr]);
	int nrTransitions = n->nextStates.length();
	if (index < nrTransitions)
		return n->nextStates[index];

	if (n->rewriteState.get() == 0)	// fully explored
		return NONE;

	RewriteSearchState* rewriteState = n->rewriteState.get();
	RewritingContext *context = rewriteState->getContext();
	while (nrTransitions <= index)
    {
		bool success = rewriteState->findNextRewrite();
		rewriteState->transferCount(*initial);
      
		if (success)
		{
			Rule* rule = rewriteState->getRule();
			bool trace = RewritingContext::getTraceStatus();
			if (trace)
			{
				context->tracePreRuleRewrite(rewriteState->getDagNode(), rule);
				if (context->traceAbort())
					return NONE;
			}
			DagNode* replacement = rewriteState->getReplacement();
			RewriteSearchState::DagPair r = rewriteState->rebuildDag(replacement);
			const auto_ptr<RewritingContext> c(context->makeSubcontext(r.first));
			initial->incrementRlCount();
			if (trace)
			{
				c->tracePostRuleRewrite(r.second);
				if (c->traceAbort())
					return NONE;
			}
			c->reduce();
			if (c->traceAbort())
				return NONE;
			initial->addInCount(*c);

			//
			// insert a new state
			//
			pair<int,bool> ins_res = hashConsSet.insertDag(r.first, seen.size());	// stateHashIndex, changed
			int nextState = hashConsSet.getOppositeIndex(ins_res.first);
			if (ins_res.second)
				seen.append(new State(this, ins_res.first));

			if (n->fwdArc.insert(make_pair(nextState, rule)).second)
			{
				n->nextStates.append(nextState);
				++nrTransitions;
			}

			//
			//	If we didn't do any equational rewriting we will not have had a chance to
			//	collect garbage.
			//
			MemoryCell::okToCollectGarbage();
		}
		else
		{
			n->rewriteState.reset();
			return NONE;
		}
    }
	return n->nextStates[index];
}

// TEST code
void
StateTransitionGraph::dfsDump(ostream& o)
{
	#include <stack>
	stack<int> state_stk;
	stack<int> index_stk;
	NatSet visited;

	int nrStates = 0;
	int nrTransitions = 0;

	state_stk.push(0);
	index_stk.push(0);

	while(!state_stk.empty())
	{
		int s = state_stk.top(), si = index_stk.top();
		int next = getNextState(s, si);

		if (next == NONE)
		{
			state_stk.pop();
			index_stk.pop();
		}
		else
		{
			index_stk.top()++;
			nrTransitions++;
			o << s << "  ==(" << si << ") " << /* rule */ " ==>" << endl;

			if (visited.contains(next))
			{
				o << " skip: " << next << endl;
				continue;
			}
			else
			{
				nrStates++;
				visited.insert(next);
				state_stk.push(next);
				index_stk.push(0);
				o << " visited: " << next << ": " << getStateDag(next) << endl;
			}
		}
	}
	o << "visited state : " << nrStates << ", visited transition: " << nrTransitions << endl;
}

}
