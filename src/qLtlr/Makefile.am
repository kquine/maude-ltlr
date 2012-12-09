noinst_LIBRARIES = libqLtlr.a

libqLtlr_a_CPPFLAGS = \
	-I$(top_srcdir)/src/Utility \
	-I$(top_srcdir)/src/Temporal \
	-I$(top_srcdir)/src/Interface \
	-I$(top_srcdir)/src/Core \
	-I$(top_srcdir)/src/Variable \
	-I$(top_srcdir)/src/FullCompiler \
	-I$(top_srcdir)/src/Higher \
	-I$(top_srcdir)/src/CUI_Theory \
	-I$(top_srcdir)/src/S_Theory \
	-I$(top_srcdir)/src/NA_Theory \
	-I$(top_srcdir)/src/FreeTheory \
	-I$(top_srcdir)/src/ObjectSystem \
	-I$(top_srcdir)/src/Mixfix \
	-I$(top_srcdir)/src/BuiltIn \
	-I$(top_srcdir)/src/MSCP10 \
	-I$(top_srcdir)/src/IO_Stuff \
	-I$(top_srcdir)/src/ACU_Persistent \
	-I$(top_srcdir)/src/ACU_Theory \
	-I$(top_srcdir)/src/AU_Persistent \
	-I$(top_srcdir)/src/AU_Theory \
	-I$(top_srcdir)/src/Meta \
	-I$(top_srcdir)/src/3rdParty \
	-I$(top_srcdir)/src/FullCompiler \
	-I$(top_srcdir)/src/qLtlr \
	-I$(top_srcdir)/src/StrategyLanguage

libqLtlr_a_SOURCES = \
	Util/TwoWayHashConsSet.cc \
	Interface/SETemporalFormulaBuilder.cc \
	Interface/FairnessDecoder.cc \
	Interface/PropLabelChecker.cc \
	Interface/ProofTermGenerator.cc \
	Interface/CounterExampleGenerator.cc \
	SearchGraph/DagTransitionGraph.cc \
	SearchGraph/stateTransitionGraph.cc \
	SearchGraph/seStateTransitionGraph.cc \
	Automaton/SystemGraph.cc \
	Automaton/SystemEventGraph.cc \
	Automaton/SystemAutomaton.cc \
	Automaton/SESystemAutomaton.cc \
	Fairness/FairSet/WeakFairSet.cc \
	Fairness/FairSet/StrongFairSet.cc \
	Fairness/FairSet/ParamRealizedSet.cc \
	Fairness/FairSet/ParamWeakFairSet.cc \
	Fairness/FairSet/ParamStrongFairSet.cc \
	Fairness/FairSet/CompositedFairSet.cc \
	Fairness/FormulaFairnessMap.cc \
	Fairness/SystemWeakFairnessMap.cc \
	Fairness/SystemStrongFairnessMap.cc \
	Fairness/StateRealizedFairGenerator.cc \
	Fairness/SystemParamWeakFairnessMap.cc \
	Fairness/SystemParamStrongFairnessMap.cc \
	Fairness/CompositedFairnessMap.cc \
	ModelChecker/NDFSModelChecker.cc \
	ModelChecker/SCCModelChecker.cc \
	ModelChecker/SCCBuchiModelChecker.cc \
	ModelChecker/StreettModelChecker.cc \
	ContextHoleSymbol.cc \
	ltlrFairnessCheckerSymbol.cc 

	
noinst_HEADERS = \
	ContextHoleSymbol.hh \
	ltlrFairnessCheckerSymbol.hh 





	
