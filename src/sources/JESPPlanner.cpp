#include "JESPPlanner.h"

#include "JESPExhaustivePlanner.h"
#include "JESPDynamicProgrammingPlanner.h"
#include "Timing.h"

#include "argumentUtils.h"

JESPPlanner::~JESPPlanner() {
	//std::cout << "~JESPPlanner" << std::endl;
}


void JESPPlanner::Plan(ArgumentHandlers::Arguments args) {
	// Avoid output to file
	args.dryrun = true;

	// Start timers
	Timing Time;
	Time.Start("Overall");

	decpomdp = std::unique_ptr<DecPOMDPDiscreteInterface>(ArgumentUtils::GetDecPOMDPDiscreteInterfaceFromArgs(args));

	//Initialization of the planner with typical options for JESP:
	Time.Start("PlanningUnit");

	PlanningUnitMADPDiscreteParameters params;
	params.SetComputeAll(true);
	params.SetComputeJointActionObservationHistories(false);
	params.SetComputeJointObservationHistories(false);
	params.SetComputeJointBeliefs(false);
	params.SetUseSparseJointBeliefs(args.sparse);

	// Sufficient to set directly on the pUnit in this class
	if(args.jesp == JESPtype::JESPExhaustive) {
		pUnit = std::unique_ptr<JESPExhaustivePlanner>(new JESPExhaustivePlanner(args.horizon, decpomdp.get(), &params));
		std::cout << "JESPExhaustivePlanner initialized" << std::endl;

	} else if(args.jesp == JESPtype::JESPDP) {
		pUnit = std::unique_ptr<JESPDynamicProgrammingPlanner>(new JESPDynamicProgrammingPlanner(args.horizon, decpomdp.get(), &params));
		std::cout << "JESPDynamicProgrammingPlanner initialized" << std::endl;
	}

	Time.Stop("PlanningUnit");
	std::cout << "JESP Planner initialized" << std::endl;

	// Do for all restarts
	for(int restartI = 0; restartI < args.nrRestarts; restartI++) {
		std::cout << "JESP Run: "  << restartI+1 << "/" << args.nrCERestarts << std::endl;

		//start all timers:
		Time.Start("Plan");

		pUnit->Plan();

		std::cout << "Value=" << pUnit->GetExpectedReward() << std::endl;

		// Stop all timers
		Time.Stop("Plan");
	}

	Time.Stop("Overall");
	Time.PrintSummary();
}
