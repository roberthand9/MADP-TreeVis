#include "PlannerManager.h"

// Include Planners
#include "JESPPlanner.h"
#include "BFSPlanner.h"
#include "GMAAPlanner.h"
#include "DICEPlanner.h"

// Include MADP Files
#include "NullPlanner.h"

// Qt files
#include <QMessageBox>

PlannerManager::~PlannerManager() {
	//std::cout << "~PlannerManager" << std::endl;
}


PlanningUnitDecPOMDPDiscrete* PlannerManager::GetPlanningUnit() {

	// If planned live, we need the planners unit
	if(livePlan) {
		return planner->GetPlanningUnit();
	} else {

		// Otherwise a null planner is being used
		// (policy read in from file)
		return nullPlannerUnit.get();
	}
}


void PlannerManager::Plan(const PlannerType &type, const ArgumentHandlers::Arguments &args) {
	bool successfulPlan = true;

	// Switch the planner type we were given
	switch(type) {

		case JESP:
			planner = std::unique_ptr<Planner>(new JESPPlanner());
			break;

		case BFS:
			planner = std::unique_ptr<Planner>(new BFSPlanner());
			break;

		case GMAA:
			planner = std::unique_ptr<Planner>(new GMAAPlanner());
			break;

		case DICEPS:
			planner = std::unique_ptr<Planner>(new DICEPlanner());
			break;
	}

	// Plan, get the planning unit, and set the policices ready to be used
	try {
		planner->Plan(args);

	} catch(E &e) {
		std::cout << "An exception was thrown: " << e.SoftPrint() << std::endl;
		successfulPlan = false;

		// Failed so false and give message
		emit PlanEnded(false, QString::fromStdString(e.SoftPrint()));
	}

	// Delete the pointer to the char* we created if needed
	if(args.problem_type == ProblemType::PARSE) {
		delete[] args.dpf;
	}

	if(successfulPlan) {
		SetPolicies(type);
		livePlan = true;

		// Might need to construct joint actions
		// if this is true it will be done anyway
		if(!args.cache_flat_models) {

			// Try and cast to factored
			MultiAgentDecisionProcessDiscreteFactoredStates* factored =
					dynamic_cast<MultiAgentDecisionProcessDiscreteFactoredStates*>(GetPlanningUnit()->GetProblem());

			// If the problem is factored, we need joint actions
			if(factored) {
				factored->ConstructJointActions();
				factored->ConstructJointObservations();
			}
		}

		// Plan succeeded
		emit PlanEnded(true);
	} else {
		// Can't be used so reset
		planner.reset(nullptr);
	}
}


bool PlannerManager::HasPlanned() {
	return (livePlan || previousPlan);
}


void PlannerManager::PreviousPlan(std::string policyFilePath, ArgumentHandlers::Arguments args) {

	// If a 'live' plan has taken place
	// we have to reset the planner as we don't reassign here
	// (no memory leak if not just not needed so why still have it)
	if(livePlan) {
		planner.reset(nullptr);
	}

	// Get input stream of policy file
	std::ifstream input(policyFilePath);

	// If for some reason we could not open, alert the user
	if(!input.is_open()) {
		emit PreviousPlanEnded(false, "Could not open policy file");
		return;
	}

	// For extraction
	std::string identifier, tmp;
	Index horizon;
	Index numAgents;

	// Read in horizon, set on args
	input >> identifier >> tmp >> horizon;

	// Read in the identifier, take the whitespace
	// then read the number of agents
	input >> identifier >> tmp >> numAgents;

	// Get the decpomdp from the given args
	decpomdp = std::unique_ptr<DecPOMDPDiscreteInterface>(
				ArgumentUtils::GetDecPOMDPDiscreteInterfaceFromArgs(args));

	// Don't compute anything
	PlanningUnitMADPDiscreteParameters params;
	params.SetComputeAll(false);

	nullPlannerUnit = std::unique_ptr<NullPlanner>(new NullPlanner(horizon, decpomdp.get(), &params));

	// Try and cast to factored
	MultiAgentDecisionProcessDiscreteFactoredStates* factored =
			dynamic_cast<MultiAgentDecisionProcessDiscreteFactoredStates*>(nullPlannerUnit->GetProblem());

	// If the problem is factored, we need joint actions
	if(factored) {
		factored->ConstructJointActions();
		factored->ConstructJointObservations();
	}

	// Delete the pointer to the char* we created if needed
	if(args.problem_type == ProblemType::PARSE) {
		delete[] args.dpf;
	}

	// Create a vector the correct size, bool for error occuring
	std::vector<Index> numObservationHistories(numAgents);
	bool successful = (numAgents == nullPlannerUnit->GetNrAgents());

	// Read in the number of observation histories for each agent
	// from the policy file given
	for(Index i=0; i<numAgents && successful; ++i) {
		input >> identifier >> tmp >> numObservationHistories[i];

		// Ensure at least somewhat correct
		successful = numObservationHistories[i] == nullPlannerUnit->GetNrObservationHistories(i);
	}

	// If successful so far continue to parse
	if(successful) {

		// Clear previous policices if given, reserve as best performing
		//previousPolicies.clear();
		previousPolicies = std::vector<std::vector<Index>>(numAgents);

		// To read into
		Index actionIndex;

		// Get the number for each agent index
		for(Index agentIndex=0; agentIndex<numAgents && successful; ++agentIndex) {

			previousPolicies[agentIndex] = std::vector<Index>();
			previousPolicies[agentIndex].reserve(numObservationHistories[agentIndex]);

			Index policyIndex = 0;

			// Read in indexes
			do {

				// Try read
				if(input >> actionIndex) {
					// Set if possible
					previousPolicies[agentIndex].push_back(actionIndex);

				} else {
					// Otherwise the policy is incorrect so an error has occurred
					successful = false;
				}

			} while(++policyIndex < numObservationHistories[agentIndex] && successful);
		}
	}

	// Simple error handling
	if(!successful) {
		emit PreviousPlanEnded(false, "Something went wrong trying to load the policy. "
									  "Possible bad policy file, are you sure problem & horizon "
									  "were correct and file is in the correct format?");
	} else {
		emit PreviousPlanEnded(true);
	}

	previousPlan = true;
	livePlan = false;
}


void PlannerManager::SetPolicies(const PlannerType &type) {

	switch(type) {

		case BFS:
		case DICEPS:
		case JESP:
			jointPolicy = planner->GetPlanningUnit()->GetJointPolicyPureVector();
			break;

		case GMAA:
			jointPolicy = static_cast<GeneralizedMAAStarPlannerForDecPOMDPDiscrete*>(
						planner->GetPlanningUnit())->GetJointPolicyDiscretePure()->ToJointPolicyPureVector();
			break;
	}

	// Set individual policies
	individualPolicies = jointPolicy->GetIndividualPolicies();
}


Index PlannerManager::GetJointActionIndex(Index johIndex) {
	// If live plan use the policy
	if(livePlan) {
		return jointPolicy->GetJointActionIndex(johIndex);

	} else {
		// Otherwise compute JA based on individual policies
		std::vector<Index> individualObservationHistoryIndexes =
				nullPlannerUnit->JointToIndividualObservationHistoryIndices(johIndex);

		std::vector<Index> individualActions(nullPlannerUnit->GetNrAgents());

		// Get individual actions for each IOH
		for(Index i=0; i<nullPlannerUnit->GetNrAgents(); ++i) {
			individualActions[i] = previousPolicies[i][individualObservationHistoryIndexes[i]];
		}

		// Return the JA from the individual ones
		return nullPlannerUnit->IndividualToJointActionIndices(individualActions);
	}
}


Index PlannerManager::GetActionIndex(Index agentIndex, Index ohIndex) {
	// If live plan use the policy
	if(livePlan) {
		return individualPolicies[agentIndex]->GetActionIndex(ohIndex);
	} else {

		// Otherwise use vector
		return previousPolicies[agentIndex][ohIndex];
	}
}
