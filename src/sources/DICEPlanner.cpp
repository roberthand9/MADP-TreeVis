#include "DICEPlanner.h"

// System files
#include <iostream>

// MADP Files
#include "DICEPSPlanner.h"
#include "Timing.h"
#include "argumentUtils.h"


DICEPlanner::~DICEPlanner() {
	// std::cout << "~DICEPlanner()" << std::endl;
}


void DICEPlanner::Plan(ArgumentHandlers::Arguments args) {
	std::cout << "Running DICE Planner..." << std::endl;
	args.dryrun = true; // Avoid output to file

	// Start timers
	Timing Time;
	Time.Start("Overall");

	// Get the decpomdp from the given args
	decpomdp = std::unique_ptr<DecPOMDPDiscreteInterface>(
				ArgumentUtils::GetDecPOMDPDiscreteInterfaceFromArgs(args));

	DICEPSPlanner* diceps = 0;

	Time.Start("PlanningUnit");

	// Typical Paramaters for DICEPS Algorithm
	PlanningUnitMADPDiscreteParameters params;
	params.SetComputeAll(false);
	params.SetComputeIndividualObservationHistories(true);

	// Joint observations histories needed for efficient computation of joint actions
	params.SetComputeJointObservationHistories(true);
	params.SetUseSparseJointBeliefs(args.sparse);

	std::cout << "Instantiating the planning unit..." << std::endl;

	// Create a new DICEPS Planner
	diceps = new DICEPSPlanner(
		args.horizon,
		decpomdp.get(), // Pass raw pointer
		1,
		args.nrCEIterations,
		args.nrCESamples,
		args.nrCESamplesForUpdate,
		args.CE_use_hard_threshold,
		args.CE_alpha,
		args.nrCEEvaluationRuns,
		&params,
		false, // No convergence stats
		0, // No out stream
		args.verbose
	);

	// Planning Unit Created
	Time.Stop("PlanningUnit");
	std::cout << "Planning unit instantiated" << std::endl;

	double totalValue = 0;

	// Go until the number of restarts
	for(Index restartI = 0; restartI < args.nrCERestarts; restartI++) {
		std::cout << "DICE Run: "  << restartI+1 << "/" << args.nrCERestarts << std::endl;

		// Start timers
		Time.Start("Plan");

		// Plan
		diceps->Plan();

		// Stop timer
		Time.Stop("Plan");

		std::cout << diceps->GetExpectedReward() << std::endl;
		totalValue += diceps->GetExpectedReward();
	}

	std::cout << "Average Value of run: " << totalValue/args.nrCERestarts << std::endl;

	// Stop the overall
	Time.Stop("Overall");
	Time.PrintSummary();
	diceps->PrintTimersSummary();

	// Set the planning unit
	pUnit = std::unique_ptr<DICEPSPlanner>(diceps);
}
