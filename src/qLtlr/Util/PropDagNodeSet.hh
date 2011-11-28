/*
 * PropDagNodeSet.hh
 *
 *  Created on: Dec 3, 2010
 *      Author: kquine
 */

#ifndef PROPDAGNODESET_HH_
#define PROPDAGNODESET_HH_
#include "dagNodeSet.hh"
#include "natSet.hh"

namespace ltlrModelChecker {

class PropDagNodeSet: public DagNodeSet
{
public:
	FastBool hasEvent()				{ return events.size() > 0; }
	FastBool isEvent(int propId)	{ return events.contains(propId); }
	void setEvent(int propId)		{ events.insert(propId); }
private:
	NatSet events;
};

}

#endif /* PROPDAGNODESET_HH_ */
