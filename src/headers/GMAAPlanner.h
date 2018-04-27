#ifndef GMAAPLANNER_H
#define GMAAPLANNER_H

// TreeVis
#include "Planner.h"

// MADP Files
#include "GeneralizedMAAStarPlannerForDecPOMDPDiscrete.h"
#include "BGIP_SolverCreatorInterface.h"
#include "argumentUtils.h"

///
/// \brief The GMAAPlanner class permits planning
/// via the GMAA Planner as part of the MADP Toolbox
/// This file is largely based on GMAA.cpp file that can
/// be found as part of the MADP toolbox.
///
class GMAAPlanner : public Planner {

	public:

		GMAAPlanner() = default;
		~GMAAPlanner();

		///
		/// \brief Plans for the given problem with the given args
		/// using the MADP GMAA Planner.
		/// \param args The arguments to use to plan with.
		///
		void Plan(ArgumentHandlers::Arguments args);

	private:
		///
		/// \brief GetGMAAInstance Gets a GMAA Instance for the solver.
		/// \param params The params constructed in the plan method
		/// \param args The args given to the plan method
		/// \param bgipsc_p The BGIP solver
		/// \return A GMAA Planner
		///
		GeneralizedMAAStarPlannerForDecPOMDPDiscrete* GetGMAAInstance(const PlanningUnitMADPDiscreteParameters &params,
																	  ArgumentHandlers::Arguments& args,
																	  BGIP_SolverCreatorInterface* bgipsc_p);

		///
		/// \brief Gets a BGIP solver interface, ownership is given to the return value
		/// \param args The args given to the plan method
		/// \return A BGIP Solver interface based on the arguments given
		///
		std::unique_ptr<BGIP_SolverCreatorInterface> GetBGIPSolverCreatorInstance(ArgumentHandlers::Arguments &args);

		///
		/// \brief Samples a random policy
		/// \param args The args given to the plan method
		///
		void SampleRandomPolicy(const ArgumentHandlers::Arguments &args);
};

#endif // GMAAPLANNER_H
