/*
 * ProductState.hh
 *
 *  Created on: Dec 2, 2010
 *      Author: kquine
 */

#ifndef PRODUCTSTATE_HH_
#define PRODUCTSTATE_HH_

namespace ltlrModelChecker {

namespace product {

struct State
{
	int system, property;

	State();
	State(int system, int property);
	bool operator==(const State& o) const;
	bool operator!=(const State& o) const;
	friend std::ostream& operator<<(std::ostream& o,const State& state);
};



inline
State::State():
system(NONE), property(NONE) {}

inline
State::State(int system, int property):
system(system), property(property) {}

inline bool
State::operator==(const State& o) const
{
	return system == o.system && property == o.property;
}

inline bool
State::operator!=(const State& o) const
{
	return system != o.system || property != o.property;
}

inline std::ostream& operator <<(std::ostream &o,const State& state)
{
	o << "(" << state.system << "," <<  state.property << ")";
	return o;
}

}

}

#endif /* PRODUCTSTATE_HH_ */
