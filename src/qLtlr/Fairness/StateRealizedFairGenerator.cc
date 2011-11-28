/*
 * StateRealizedFairGenerator.cc
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */
//      utility stuff
#include "macros.hh"
#include "vector.hh"

//      forward declarations
#include "interface.hh"
#include "core.hh"

//      interface class definitions
#include "symbol.hh"
#include "dagNode.hh"

//		automaton definitions
#include "Automaton/SystemGraph.hh"

//		ltlr interface definitions
#include "Interface/FairnessDecoder.hh"
#include "Util/PropDagNodeSet.hh"

#include "StateRealizedFairGenerator.hh"

namespace ltlrModelChecker {

StateRealizedFairGenerator::StateRealizedFairGenerator(
		StateTransitionDagMap* map, RewritingContext* context, PropDagNodeSet& props, FairnessDecoder* fd):
		map(map), fairReader(fd), context(context), propositions(props) {}

DagNode*
StateRealizedFairGenerator::getFairnessDag(int fairnessNr) const
{
	Assert(fairnessCondTable.size() > fairnessNr, "Invalid fairness dag lookup " <<
			fairnessNr << "(" << fairnessCondTable.size() << ")");
	return hashConsSet.getCanonical(fairnessCondTable[fairnessNr]->hashConsIndex);
}

int
StateRealizedFairGenerator::getNrWeakFairness() const
{
	return weakFair.size();
}

const NatSet&
StateRealizedFairGenerator::getWeakStateFairness(int stateNr)
{
	return getStateFairness(stateNr).first;
}

const Bdd&
StateRealizedFairGenerator::getWeakFairFormula(int weakNr) const
{
	Assert(weakNr < weakFair.size(), "Invalid param weak fairness number");
	return fairnessCondTable[weakFair[weakNr]]->fairCond.second;
}

const NatSet&
StateRealizedFairGenerator::getWeakVacuousFair() const
{
	return vacuousTrueWeak;
}

int
StateRealizedFairGenerator::getNrStrongFairness() const
{
	return strongFair.size();
}

const NatSet&
StateRealizedFairGenerator::getStrongStateFairness(int stateNr)
{
	return getStateFairness(stateNr).second;
}

const pair<Bdd,Bdd>&
StateRealizedFairGenerator::getStrongFairFormula(int strongNr) const
{
	Assert(strongNr < strongFair.size(), "Invalid param strong fairness number");
	return fairnessCondTable[strongFair[strongNr]]->fairCond;
}

const pair<NatSet,NatSet>&
StateRealizedFairGenerator::getStrongVacuousFair() const
{
	return vacuousTrueStrong;
}

const pair<NatSet,NatSet>&
StateRealizedFairGenerator::getStateFairness(int stateNr)
{
	int oldSize = stateRelatedFair.size();
	if (stateNr >= oldSize)
	{
		stateRelatedFair.expandTo(stateNr + 1);
		for (int i = oldSize; i <= stateNr; ++i)
			stateRelatedFair[i] = NULL;
	}
	if (stateRelatedFair[stateNr] == NULL)
	{
		stateRelatedFair[stateNr] = new pair<NatSet,NatSet>;
		Vector<DagNode*> fairDags;
		fairReader->computeStateRealizedFairDags(map->getStateDag(stateNr), context, fairDags);
		FOR_EACH_CONST(i, Vector<DagNode*>, fairDags)
		{
			FairnessCondition* fair = insertFairDag(*i);
			if (fair->fairCond.first == bdd_true())
				stateRelatedFair[stateNr]->first.insert(fair->fairnessIndex);	// weak
			else
				stateRelatedFair[stateNr]->second.insert(fair->fairnessIndex);	// strong
		}
	}
	return *stateRelatedFair[stateNr];

}

StateRealizedFairGenerator::FairnessCondition*
StateRealizedFairGenerator::insertFairDag(DagNode* dag)
{
	pair<int,bool> fair_inc = hashConsSet.insertDag(dag, fairnessCondTable.size());
	int newFair = hashConsSet.getOppositeIndex(fair_inc.first);
	if (fair_inc.second)
	{
		FairnessCondition* fair = new FairnessCondition(fair_inc.first);
		DagNode* fairDag = hashConsSet.getCanonical(fair_inc.first);
		pair<bool,pair<bool,bool> > result = fairReader->interpreteFairnessDag(fairDag, propositions, fair->fairCond);

		if (fair->fairCond.first == bdd_true())
		{
			int wi = weakFair.size();
			fair->fairnessIndex = wi;
			weakFair.append(newFair);
			if (result.second.second)
				vacuousTrueWeak.insert(wi);
		}
		else
		{
			int si = strongFair.size();
			strongFair.append(newFair);
			fair->fairnessIndex = si;
			if (result.second.first)
				vacuousTrueStrong.first.insert(si);
			if (result.second.second)
				vacuousTrueStrong.second.insert(si);
		}
		fairnessCondTable.append(fair);
	}
	return fairnessCondTable[newFair];
}

}


