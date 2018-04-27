#ifndef PLANNERMANAGER_H
#define PLANNERMANAGER_H

// TreeVis files
#include "Planner.h"

// MADP Files
#include "NullPlanner.h"
#include "PolicyPureVector.h"
#include "argumentHandlers.h"

// Smart pointers
#include <memory>

// Qt
#include <QWidget>
#include <QStringList>

///
/// \brief The PlannerManager class is used to abstract
/// the planning process and subsequent use of the planner
/// object after planning has occurred. It allows the user
/// to plan for a given problem using any of the 4 planners
/// specified in the PlannerType enum to plan for the given
/// arguments. It provides an interface to use either a live
/// plan or read in a policy from a file and provides the abstraction
/// needed in order to get actions from these.
///
class PlannerManager : public QWidget {
	Q_OBJECT

	public:
		/// Constructor
		PlannerManager() = default;
		~PlannerManager();

		// The type of planner used
		typedef enum {
			BFS,
			GMAA,
			JESP,
			DICEPS
		} PlannerType;

		///
		/// \brief Plan for the given problem using the planner type
		/// specified and the arguments given
		/// \param type The type of planner to plan with
		/// \param args The args to pass to the planner
		///
		void Plan(const PlannerType &type,
				  const ArgumentHandlers::Arguments &args);

		///
		/// \brief Allows a previous plan to be read in to the program
		/// \param policyFilePath The file path of the saved policy
		/// \param args The args to pass to the NullPlanner
		///
		void PreviousPlan(std::string policyFilePath,
						  ArgumentHandlers::Arguments args);

		///
		/// \brief Gets the JAI specified by the policy for the given
		/// JOH index. Can be called either
		/// on a live plan or a previous plan
		/// \param johIndex The JOHI index to get the JAI for
		/// \return The JAI Specified by the policy for the given JOHI
		///
		Index GetJointActionIndex(Index johIndex);

		///
		/// \brief Gets the AI specified by the policy for the given agent
		/// \param agentIndex The agent index to get the AI for
		/// \param ohIndex The individual OH index to get the AI for
		/// \return The AI for the OH index for the agent index
		///
		Index GetActionIndex(Index agentIndex, Index ohIndex);

		///
		/// \brief Allows access to the planning unit used to plan.
		/// The Manager retains ownership of the object.
		/// \return A pointer to the planning unit.
		///
		PlanningUnitDecPOMDPDiscrete* GetPlanningUnit();

		///
		/// \brief Returns if either a live plan or an offline read in
		/// has occurred
		/// \return True if a live plan or a previous policy has been read in,
		/// false otherwise
		///
		bool HasPlanned();

	signals:
		///
		/// \brief PlanEnded Signal emitted when a live plan has ended
		/// \param success True if the plan succeeded, false otherwise
		/// \param message Error message if the plan failed
		///
		void PlanEnded(bool success, QString message = "");

		///
		/// \brief PreviousPlanEnded Signal emitted when a previous
		/// policy/plan has finished being read in
		/// \param success True if the policy was successfully read in, false otherwise
		/// \param message Error message if reading in the policy failed
		///
		void PreviousPlanEnded(bool success, QString message = "");

	private:

		/// Live plan taken place
		bool livePlan = false;

		/// Previous plan has been read in
		bool previousPlan = false;

		/// The planner used for live plans
		std::unique_ptr<Planner> planner;

		/// Individual policies for live plans
		std::vector<PolicyPureVector*> individualPolicies;

		/// Joint policy for live plans
		boost::shared_ptr<JointPolicyPureVector> jointPolicy;

		/// DecPOMDP for previous plans
		std::unique_ptr<DecPOMDPDiscreteInterface> decpomdp;

		///
		/// \brief Populated when a previous policy is read in.
		/// Each vector is the indivdual policy for that agent.
		/// Lookups can be done easily as
		/// previousPolicy[agentIndex][ohIndex] and should be
		/// constant time.
		///
		std::vector<std::vector<Index>> previousPolicies;

		/// Used for previous plans instead of a Planner object
		std::unique_ptr<NullPlanner> nullPlannerUnit;

		///
		/// \brief Sets the class based policy variables at the
		/// end of a live plan
		/// \param type The planner type used in the live plan
		///
		void SetPolicies(const PlannerType &type);
};

#endif // PLANNERMANAGER_H
