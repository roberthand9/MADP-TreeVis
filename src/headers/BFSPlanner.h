#ifndef BFSPLANNER_H
#define BFSPLANNER_H

// TreeVis
#include "Planner.h"

// MADP Files
#include "argumentHandlers.h"
#include "BruteForceSearchPlanner.h"

///
/// \brief The BFSPlanner class permits planning
/// via the BruteForceSearchPlanner as part of the MADP Toolbox
/// This file is largely based on BFS.cpp file that can
/// be found as part of the MADP toolbox.
///
class BFSPlanner : public Planner {

	public:

		BFSPlanner() = default;
		~BFSPlanner();

		///
		/// \brief Plans for the given problem with the given args
		/// using the MADP BFS Planner.
		///
		/// \param args The arguments to use to plan with.
		///
		void Plan(ArgumentHandlers::Arguments args);
};

#endif // BFSPLANNER_H
