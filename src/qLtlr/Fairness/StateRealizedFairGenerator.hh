/*
 * StateRealizedFairGenerator.hh
 *
 *  Created on: Dec 4, 2010
 *      Author: kquine
 */

#ifndef STATEREALIZEDFAIRGENERATOR_HH_
#define STATEREALIZEDFAIRGENERATOR_HH_
#include "Util/PtrVector.hh"
#include "Util/TwoWayHashConsSet.hh"
#include "Automaton/StateTransitionDagMap.hh"
#include "Interface/FairnessDecoder.hh"

namespace ltlrModelChecker {

class StateRealizedFairGenerator
{
	NO_COPYING(StateRealizedFairGenerator);

public:
	StateRealizedFairGenerator(StateTransitionDagMap* map, RewritingContext* context, PropDagNodeSet& props, FairnessDecoder* fd);

	int getNrWeakFairness() const;
	const NatSet& getWeakStateFairness(int stateNr);
	const Bdd& getWeakFairFormula(int weakNr) const;
	const NatSet& getWeakVacuousFair() const;

	int getNrStrongFairness() const;
	const NatSet& getStrongStateFairness(int stateNr);
	const pair<Bdd,Bdd>& getStrongFairFormula(int strongNr) const;
	const pair<NatSet,NatSet>& getStrongVacuousFair() const;


	DagNode* getFairnessDag(int fairnessNr) const;
private:
	struct FairnessCondition
	{
		FairnessCondition(int hashConsIndex): hashConsIndex(hashConsIndex) {}
		int hashConsIndex;
		int fairnessIndex;			// i-th (strong or weak) fairness condition..?
		pair<Bdd,Bdd> fairCond;		// the first comp is true for weak fairness.
	};

	const pair<NatSet,NatSet>& getStateFairness(int stateNr);
	FairnessCondition* insertFairDag(DagNode* dag);

	Vector<pair<NatSet,NatSet>*> stateRelatedFair;	// stateNr |-> realized (weak,strong) fairness indices on the state

	Vector<int> weakFair;					// weakIndex	|-> Fairness Condition index
	Vector<int> strongFair;					// strongIndex	|-> Fairness Condition index
	NatSet vacuousTrueWeak;					// vacuously satisfied weak fairness indices if unrealized
	pair<NatSet,NatSet> vacuousTrueStrong;	// vacuously satisfied strong fairness indices if unrealized

	PropDagNodeSet& propositions;
	TwoWayHashConsSet hashConsSet;						// to store fairness dag
	PtrVector<FairnessCondition> fairnessCondTable;		// fairnessDagIndex |-> hashCons index;

	StateTransitionDagMap* map;
	FairnessDecoder* fairReader;
	RewritingContext* context;
};

}

#endif /* STATEREALIZEDFAIRDAGGENERATOR_HH_ */
