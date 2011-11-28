/*
 * ModelChecker.hh
 *
 *  Created on: Nov 26, 2010
 *      Author: kquine
 */

#ifndef MODELCHECKER_HH_
#define MODELCHECKER_HH_
#include <list>
#include "macros.hh"

namespace ltlrModelChecker {

class ModelChecker {
public:
	virtual ~ModelChecker()	{}
	virtual bool findCounterExample() = 0;

	const list<pair<int,int> >& getLeadIn() const;
	const list<pair<int,int> >& getCycle() const;

protected:
	list<pair<int,int> > leadIn;
	list<pair<int,int> > cycle;
};

inline const list<pair<int,int> >&
ModelChecker::getLeadIn() const
{
	return leadIn;
}

inline const list<pair<int,int> >&
ModelChecker::getCycle() const
{
	return cycle;
}

}

#endif /* MODELCHECKER_HH_ */
