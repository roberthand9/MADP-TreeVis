#include "BFSPlanner.h"

#include "argumentUtils.h"
#include "Timing.h"

void BFSPlanner::Plan(ArgumentHandlers::Arguments args) {
	std::cout << "Running BFS Planner..." << std::endl;

	// Avoid output to file
	args.dryrun = true;

	// Start timers
	Timing Time;
	Time.Start("Overall");
	Time.Start("PlanningUnit");

	// Set decpomdp
	decpomdp = std::unique_ptr<DecPOMDPDiscreteInterface>(
				ArgumentUtils::GetDecPOMDPDiscreteInterfaceFromArgs(args));

	Time.Stop("PlanningUnit");

	// Set parameters as necessary
	PlanningUnitMADPDiscreteParameters params;
	params.SetComputeAll(true);
	params.SetUseSparseJointBeliefs(args.sparse);

	std::cout << "Instantiating the planning unit..." << std::endl;

	// Create a new Brute Force Search planner, sufficient to set
	// pUnit directly here
	pUnit = std::unique_ptr<BruteForceSearchPlanner>(
				new BruteForceSearchPlanner(args.horizon, decpomdp.get(), &params));

	std::cout << "Planning unit instantiated" << std::endl;

	// For the number of restarts
	for(int restartI = 0; restartI < args.nrRestarts; restartI++) {
		Time.Start("Plan");

		std::cout << "BFS Run: "  << restartI+1 << "/" << args.nrRestarts << std::endl;

		// Plan
		pUnit->Plan();

		Time.Stop("Plan");

		std::cout << "Value = " << pUnit->GetExpectedReward() << std::endl;
	}

	Time.Stop("Overall");
	Time.PrintSummary();
}

BFSPlanner::~BFSPlanner() {
	// std::cout << "~BFSPlanner()" << std::endl;
}
