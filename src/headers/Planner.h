#ifndef PLANNER_H
#define PLANNER_H

// MADP Files
#include "PlanningUnitDecPOMDPDiscrete.h"
#include "argumentHandlers.h"

// Smart pointers
#include <memory>

///
/// \brief The Planner class is a super class for planners.
/// It holds the ownership to the PlanningUnitDecPOMDPDiscrete
/// and DecPOMDPDiscreteInterface (both MADP types) used
/// by the planner.
///
class Planner {

	public:
		/// Destructor
		virtual ~Planner() {
			//std::cout << "~Planner" << std::endl;
		}

		/// Pure virtual, implemented by base classes
		virtual void Plan(ArgumentHandlers::Arguments args) = 0;

		///
		/// \brief Gets the planning unit. This still
		/// holds the ownership to the object.
		/// \return The planning unit
		///
		PlanningUnitDecPOMDPDiscrete* GetPlanningUnit() {
			return pUnit.get();
		}

	protected:
		/// The planning unit used by the planner
		std::unique_ptr<PlanningUnitDecPOMDPDiscrete> pUnit = 0;

		/// The DecPOMDP used by the planner during planning
		std::unique_ptr<DecPOMDPDiscreteInterface> decpomdp = 0;
};

#endif // PLANNER_H
