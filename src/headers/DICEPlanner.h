#ifndef DICEPLANNER_H
#define DICEPLANNER_H

// TreeVis
#include "Planner.h"

// MADP Files
#include "argumentHandlers.h"

///
/// \brief The DICEPlanner class permits planning
/// via the DICEPSPlanner as part of the MADP Toolbox.
/// This file is largely based on DICEPS.cpp file that can
/// be found as part of the MADP toolbox.
///
class DICEPlanner : public Planner {

	public:

		DICEPlanner() = default;
		~DICEPlanner();

		///
		/// \brief Plans for the given problem with the given args
		/// using the MADP Dice Planner.
		/// \param args The arguments to use to plan with.
		///
		void Plan(ArgumentHandlers::Arguments args);
};

#endif // DICEPSPLANNER_H
