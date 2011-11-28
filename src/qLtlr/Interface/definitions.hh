/*
 * definitions.hh
 *
 *  Created on: Dec 14, 2010
 *      Author: kquine
 */

#ifndef DEFINITIONS_HH_
#define DEFINITIONS_HH_
#include <stdexcept>

namespace ltlrModelChecker
{

struct LTLRFormula
{
	LTLRFormula(): top(NONE), event(false) {}
	LogicFormula formula;
	int top;
	bool event;
};

class InvalidDagException: public runtime_error
{
public:
	InvalidDagException(const string& msg, DagNode* dagNode) throw(): runtime_error(msg), dag(dagNode) {}
	DagNode* getDag() const { return dag; }
private:
	DagNode* dag;
};
}
#endif /* DEFINITIONS_HH_ */
