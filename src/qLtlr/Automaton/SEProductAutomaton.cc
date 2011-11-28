/*
 * SEProductAutomaton.cc
 *
 *  Created on: Dec 2, 2010
 *      Author: kquine
 */

#include "SEProductAutomaton.hh"

namespace ltlrModelChecker {

template <typename PA>
SEProductAutomaton<PA>::SEProductAutomaton(SystemEventGraph& system, const PA& property):
	ProductAutomaton<PA>(system,property) {}

template <typename PA> typename SEProductAutomaton<PA>::SETransitionIterator*
SEProductAutomaton<PA>::makeTransitionIterator(const State& state)
{
	SETransitionIterator* itor = new SETransitionIterator(this, state);
	itor->init();
	return itor;
}

namespace product {


	template <typename PA>
	SETransitionIterator<PA>::SETransitionIterator(SEProductAutomaton<PA>* parent, const State& state):
		TransitionIterator<PA>(parent,state) {}


	template <typename PA> void
	SETransitionIterator<PA>::computeNextTransition(FastBool first)
	{
		SystemEventGraph* graph = static_cast<SystemEventGraph*>(this->sysGraph);
		pair<FastBool,Bdd> test;

		switch(first)
		{
		case 1:
			for (this->tr.propertyIndex = this->ts->begin(); this->tr.propertyIndex != this->ts->end(); ++this->tr.propertyIndex)
			{
				// check state-props
				test = graph->satisfiesFormula(
						PropertyTransitionAdaptor<PA>::getFormula(*this->tr.propertyIndex), this->tr.source.system);
				if (test.first)
				{
					label = test.second;
					for (this->tr.systemIndex = 0; ; ++this->tr.systemIndex)
					{
						this->tr.target.system = this->sysGraph->getNextState(this->tr.source.system,this->tr.systemIndex);
						if (this->tr.target.system == NONE)
							break;

						// check event-props
						if (graph->satisfiesFormula(label, this->tr.source.system, this->tr.systemIndex))
						{
							this->tr.target.property = PropertyTransitionAdaptor<PA>::getNextState(*this->tr.propertyIndex);
							return;
		default:;
						}
					}
				}
			}
		}
		this->tr.systemIndex = NONE;
		this->tr.target.system = NONE;
		this->tr.target.property = NONE;
	}

}

}
