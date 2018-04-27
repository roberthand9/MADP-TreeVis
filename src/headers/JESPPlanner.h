#ifndef JESPPLANNER_H
#define JESPPLANNER_H

// TreeVis
#include "Planner.h"

// MADP Files
#include "PlanningUnitDecPOMDPDiscrete.h"
#include "argumentHandlers.h"

///
/// \brief The JESPPlanner class permits planning
/// via the JESP Planners (DP and Exhaustive) as part of the MADP Toolbox.
/// This file is largely based on JESP.cpp file that can
/// be found as part of the MADP toolbox.
///
class JESPPlanner : public Planner {

	public:
		JESPPlanner() = default;
		~JESPPlanner();

		/// \brief Plans for the given problem with the given args
		/// using the MADP JESP Planner.
		/// \param args The arguments to plan with
		///
		void Plan(ArgumentHandlers::Arguments args);
};

#endif // JESPPLANNER_H
