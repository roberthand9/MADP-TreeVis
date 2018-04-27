#include "GMAAPlanner.h"

#include <iostream>

#include "GMAA_MAAstar.h"
#include "GMAA_MAAstarClassic.h"
#include "GMAA_MAAstarCluster.h"
#include "GMAA_kGMAA.h"
#include "GMAA_kGMAACluster.h"

#include "BGIP_SolverCreator_AM.h"
#include "BGIP_SolverCreator_BFS.h"
#include "BGIP_SolverCreator_CE.h"
#include "BGIP_SolverCreator_MP.h"
#include "BGIP_SolverCreator_BnB.h"
#include "BGIP_SolverCreator_Random.h"
#include "BGIP_SolverCreator_BFSNonInc.h"

#include "SimulationDecPOMDPDiscrete.h"
#include "AgentRandom.h"

#include "NullPlanner.h"
#include "OptimalValueDatabase.h"

using namespace qheur;
using namespace GMAAtype;
using namespace BGIP_SolverType;


GMAAPlanner::~GMAAPlanner() {
	//std::cout << "~GMAAPlanner" << std::endl;
}


void GMAAPlanner::Plan(ArgumentHandlers::Arguments args) {
	// Avoid output to file
	args.dryrun = true;
	bool errorOccurred = false;

	// Variables for run
	GeneralizedMAAStarPlannerForDecPOMDPDiscrete* gmaaFirstInstance = 0;
	GeneralizedMAAStarPlannerForDecPOMDPDiscrete* gmaa = 0;
	std::unique_ptr<BGIP_SolverCreatorInterface> bgipsc_p = 0;
	std::unique_ptr<QFunctionJAOHInterface> q = 0;

	std::stringstream description;

	switch(args.gmaa) {
		case MAAstar:
			if(args.useBGclustering && args.bgsolver == BnB) {
				description << "GMAA-ICE (JAIR 2013)";

			} else if(args.useBGclustering) {
				description << "GMAA-IC (AAMAS 2009)";

			} else {
				description << "GMAA-MAAstar";
			}
			break;

		case MAAstarClassic:
			description << "GMAA-MAAstar (JAIR 2008)";
			break;

		case FSPC:
			description << "FSPC (JAIR 2008)";
			break;

		case kGMAA:
			description << "kGMAA (JAIR 2008)";
			break;
	}

	if(args.gmaa != MAAstarClassic) {
		description << " using a "  << SoftPrint(args.bgsolver) <<
					   " solver and a "	<< SoftPrint(args.qheur) << " heuristic";
	}

	std::cout << description.str() << std::endl;

	Timing Time;
	Time.Start("Overall");
	Time.Start("PlanningUnit");

	std::cout << "Instantiating the problem..." << std::endl;

	// Get the decpomdp from the given args
	decpomdp = std::unique_ptr<DecPOMDPDiscreteInterface>(
				ArgumentUtils::GetDecPOMDPDiscreteInterfaceFromArgs(args));

	std::cout << "...done." << std::endl;

	// Set parameters
	bool optimalSolutionMethod = (args.gmaa == MAAstar || args.gmaa == MAAstarClassic);
	double V = -DBL_MAX;

	// Setup params for the gmaa unit
	PlanningUnitMADPDiscreteParameters params;
	params.SetComputeAll(false);
	params.SetUseSparseJointBeliefs(args.sparse);

	std::cout << "Instantiating the planning unit..." << std::endl;
	Time.Start("PlanningUnit");

	// Gets bgip solver instance
	bgipsc_p = GetBGIPSolverCreatorInstance(args);

	std::cout << "BGIP_SolverCreatorInterface instance: " << bgipsc_p->SoftPrint() << std::endl;
	std::cout << "Initalising GMAA Instance" << std::endl;

	// We need to make sure that the first GMAA instance exists until
	// the end of the program, as the Q-heuristic will use
	// functionality from it. Circular dependance.
	gmaaFirstInstance = GetGMAAInstance(params, args, bgipsc_p.get());
	gmaa = gmaaFirstInstance;

	Time.Stop("PlanningUnit");

	std::cout << "GMAA Planner and planning unit initialised" << std::endl;

	// Set QHeuristic, wrap in unique so auto deleted
	q = unique_ptr<QFunctionJAOHInterface>(ArgumentUtils::GetQheuristicFromArgs(gmaaFirstInstance, args));

	// Computation of the Q function starts
	try {
		std::cout << "Computing the Q heuristic (" << SoftPrint(args.qheur) << ")..." << std::endl;
		Time.Start("ComputeQ");

		if(args.useQcache || args.requireQcache) {

			if(args.requireQcache) {
				q->ComputeWithCachedQValues(false);
			} else {
				q->ComputeWithCachedQValues(true);
			}

		} else {
			q->Compute();
		}

	} catch(std::bad_alloc &e) {
		std::stringstream stream;
		stream << "GMAA ran out of memory while computing the QHeuristic:\n" << e.what();
		throw(stream);
	}

	Time.Stop("ComputeQ");
	std::cout << SoftPrint(args.qheur) << " heuristic computed" << std::endl;

	double total_value = 0;

	// While a restart left and no error has occurred
	for(int restartI = 0; (restartI < args.nrRestarts) && !errorOccurred; restartI++) {
		std::cout << std::endl << "GMAA run " << restartI+1 << "/"
			 << args.nrRestarts << " starting" << std::endl;

		if(restartI > 0) {
			gmaa = GetGMAAInstance(params, args, bgipsc_p.get());
		}

		// Set the computed huristic on the gmaa planner
		gmaa->SetQHeuristic(q.get());

		// Timer start
		Time.Start("Plan");

		// Try to plan
		try {
			gmaa->Plan();

		  // Catch and propagate back up to planner manager
		} catch(std::bad_alloc &e) {
			throw E("GMAA Ran out or memory whilst planning");

			// Next restart
			errorOccurred = true;
			continue;

		} catch(EDeadline &e) {
			throw E("GMAA exceeded the deadline");

			// Next restart
			errorOccurred = true;
			continue;

		} catch(E &e) {
			throw E("Other exception was thrown while gmaa planning: " + e.SoftPrint());

			// Next restart
			errorOccurred = true;
			continue;
		}

		V = gmaa->GetExpectedReward();
		Time.Stop("Plan");

		// Timer is stopped - ok to do file I/O
		std::cout << "Value = " << V << std::endl;
		total_value += V;

		// Check if the value corresponds to the optimal value in case
		// this is an optimal method and we already computed it before
		OptimalValueDatabase db(gmaa);
		cout << "OptimalValueDatabase: entry '" << db.GetEntryName() << "'" << endl;

		if(optimalSolutionMethod) {
			if(db.IsInDatabase()) {
				if(!db.IsOptimal(V)) {

					std::stringstream ss;
					ss << "OptimalValueDatabase: GMAA error, computed value " << V
					   << " does not match"
					   << " previously computed optimal value "
					   << db.GetOptimalValue();

					std::cout << ss.str() << std::endl;

					// Next restart
					errorOccurred = true;
					continue;
				} else {
					std::cout << "OptimalValueDatabase: Computed value "
							"matches with OptimalValueDatabase" << std::endl;
				}
			} else {

				std::cout << "OptimalValueDatabase: Optimal value unknown." << std::endl;

				// Raise error if testing
				if(args.testMode) {

					// Next restart
					errorOccurred = true;
					continue;
				} else {

					// Otherwise save new value, if this throws it will be caught
					// in planner manager
					db.SetOptimalValue(V);
				}
			}
		} else {

			// Approximate methods
			if(db.IsInDatabase()) {
				std::cout << "OptimalValueDatabase: Computed value is " << V / db.GetOptimalValue()
					 << " of optimal (value " << db.GetOptimalValue() << ")" << std::endl;

				if(V > db.GetOptimalValue()) {

					// Next restart
					errorOccurred = true;
					continue;
				}
			} else {
				std::cout << "OptimalValueDatabase: Optimal value unknown." << std::endl;
			}
		}

		// Simulations running
		std::cout << "Running Simulation to determine control quality..." << std::endl;

		// Get joint policy
		boost::shared_ptr<JointPolicyDiscretePure> found_jpol;
		if(!args.useBGclustering) {
			found_jpol = gmaa->GetJointPolicyDiscretePure()->ToJointPolicyPureVector();
		}

		Time.Start("Simulation");
		double Vsim = 42;
		const int nrSimRuns = 10000;

		SimulationDecPOMDPDiscrete sim(*gmaa, nrSimRuns);
		SimulationResult result;

		// Run simulations
		if(args.useBGclustering) {
			boost::shared_ptr<JointPolicyDiscretePure> jppv = gmaa->GetJointPolicyDiscretePure();
			boost::shared_ptr<JointPolicyPureVectorForClusteredBG> jp4CBG =
					boost::dynamic_pointer_cast<JointPolicyPureVectorForClusteredBG>(jppv);

			if(jp4CBG == 0) {
				throw E("Conversion to JPPVectorForClusteredBG failed!");
			}

			result = sim.RunSimulations(jp4CBG);
		} else {
			result = sim.RunSimulations(found_jpol);
		}

		Time.Stop("Simulation");

		// Set value of simulations
		Vsim = result.GetAvgReward();

		std::cout << "Sampled value = " << Vsim << " (computed was " << V << ")" << std::endl;

		// Output clustering statistics
		if (args.useBGclustering && args.gmaa == MAAstar) {
			std::cout << std::endl << "Cluster statistics:" << std::endl;
			static_cast<GMAA_MAAstarCluster*>(gmaa)->PrintClusteringStats();
		}

		std::cout << "================ GMAA run " << restartI+1 << "/"
			 << args.nrRestarts << " ended, Dec-POMDP value=" << V << std::endl;
	}

	Time.Stop("Overall");

	if(!errorOccurred) {
		std::cout << "Summary of timing results:" << std::endl;
		Time.PrintSummary();
		gmaa->PrintTimersSummary();
		SampleRandomPolicy(args);
	}

	// Take the gmaa pointer and make it unique on the Planner
	pUnit = std::unique_ptr<GeneralizedMAAStarPlannerForDecPOMDPDiscrete>(gmaa);
}

void GMAAPlanner::SampleRandomPolicy(const ArgumentHandlers::Arguments &args) {
	NullPlanner* np = new NullPlanner(args.horizon, decpomdp.get());

	SimulationDecPOMDPDiscrete sim(*np, args);
	sim.SetVerbose(false);

	SimulationResult result;

	vector<AgentFullyObservable*> agents;

	for(unsigned int i=0; i!= np->GetNrAgents(); ++i) {
		AgentFullyObservable* agent1;
		AgentRandom agent(np, 0);
		agent1 = new AgentRandom(agent);
		agent1->SetIndex(i);
		agents.push_back(agent1);
	}

	result = sim.RunSimulations(agents);

	if(args.verbose >= 0) {
		std::cout << "evaluateRandomPolicy sampled value: " << result.GetAvgReward() << std::endl;
	}

	for(unsigned int i=0; i != np->GetNrAgents(); ++i) {
		delete agents.at(i);
	}

	delete np;
}


GeneralizedMAAStarPlannerForDecPOMDPDiscrete*
GMAAPlanner::GetGMAAInstance(const PlanningUnitMADPDiscreteParameters &params,
							 ArgumentHandlers::Arguments &args,
							 BGIP_SolverCreatorInterface* bgipsc_p) {

	GeneralizedMAAStarPlannerForDecPOMDPDiscrete* gmaaToReturn = 0;

	if(args.useBGclustering) {
		switch(args.gmaa) {

			case MAAstar:

				if(!bgipsc_p->IsExactSolver()) {
					std::cout << "Error: MAAstar requires an exact BG solver, and "
						 << bgipsc_p->SoftPrintBrief() << " is not" << std::endl;
					exit(1);
				}

				gmaaToReturn = new GMAA_MAAstarCluster(bgipsc_p, args.horizon, decpomdp.get(), &params, args.verbose);
				break;

			case FSPC:
				args.k=1; // Fall through on purpose

			case kGMAA: {
				GMAA_kGMAACluster* gmaaCluster = new GMAA_kGMAACluster(
							bgipsc_p, args.horizon, decpomdp.get(), &params, args.k,
							static_cast<BayesianGameWithClusterInfo::BGClusterAlgorithm>(args.BGClusterAlgorithm));

				gmaaCluster->SetTresholdJB(args.thresholdJB);
				gmaaCluster->SetTresholdPjaoh(args.thresholdPjaoh);
				gmaaToReturn = gmaaCluster;
				break;
			}

			case MAAstarClassic:
				throw E("MAAstarClassic not implemented for clustered version");
				break;

			default:
				throw E("Unrecognized GMAA type!");
		}

	} else {

		// Regular un-clustered BGs
		switch(args.gmaa) {

			case MAAstar:
				if(!bgipsc_p->IsExactSolver()) {
					std::stringstream ss;

					ss << "Error: MAAstar requires an exact BG solver, and "
						 << bgipsc_p->SoftPrintBrief() << " is not";

					throw E(ss);
				}

				gmaaToReturn = new GMAA_MAAstar(bgipsc_p, args.horizon, decpomdp.get(), &params, args.verbose);
				break;

			case FSPC:
				args.k=1; // Fall through on purpose

			case kGMAA:
				gmaaToReturn = new GMAA_kGMAA(bgipsc_p, args.horizon, decpomdp.get(), &params, args.k);
				break;

			case MAAstarClassic:
				gmaaToReturn = new GMAA_MAAstarClassic(args.horizon, decpomdp.get(), &params, args.verbose);
				break;

			default:
				throw E("Unrecognized GMAA type!");
		}
	}

	gmaaToReturn->SetVerbose(args.verbose);

	if(args.GMAAdeadline) {
		gmaaToReturn->SetDeadline(args.GMAAdeadline);
	}

	return gmaaToReturn;
}

std::unique_ptr<BGIP_SolverCreatorInterface>
GMAAPlanner::GetBGIPSolverCreatorInstance(ArgumentHandlers::Arguments &args) {

	std::unique_ptr<BGIP_SolverCreatorInterface> bgipsc_p = 0;

	// Classic uses a built-in solver
	if(args.gmaa != MAAstarClassic) {

		// for MAAstar we need to make sure we keep all the solutions,
		// so set k to INT_MAX
		if(args.gmaa == MAAstar) {
			args.k = INT_MAX;
		}

		switch(args.bgsolver) {

			case BFS:

				if(args.useBGclustering) {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
								new BGIP_SolverCreator_BFS<JointPolicyPureVectorForClusteredBG>(args.verbose, args.k));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
								new BGIP_SolverCreator_BFS<JointPolicyPureVector>(args.verbose, args.k));
				}

				break;

			case BFSNonInc:

				if(args.useBGclustering) {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
							new BGIP_SolverCreator_BFSNonInc<JointPolicyPureVectorForClusteredBG>(args.verbose, args.k));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
							new BGIP_SolverCreator_BFSNonInc<JointPolicyPureVector>(args.verbose, args.k));
				}

				break;

			case AM:

				if(args.useBGclustering) {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
							new BGIP_SolverCreator_AM<JointPolicyPureVectorForClusteredBG>(args.nrAMRestarts, args.verbose, args.k));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(
							new BGIP_SolverCreator_AM<JointPolicyPureVector>(args.nrAMRestarts, args.verbose, args.k));
				}

				break;

			case CE:

				if(args.useBGclustering) {
					throw(E("BGIP_SolverCE does not work yet with clustered policies"));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_CE(
						args.nrCERestarts,
						args.nrCEIterations,
						args.nrCESamples,
						args.nrCESamplesForUpdate,
						args.CE_use_hard_threshold,
						args.CE_alpha));
				}

				break;

			case MaxPlus:

				if(args.useBGclustering) {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_MP<JointPolicyPureVectorForClusteredBG>(
						args.maxplus_maxiter, args.maxplus_updateT,
						args.maxplus_verbose, args.maxplus_damping,
						args.k, // <- nr solutions to return by BG solver
						args.maxplus_nrRestarts));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_MP<JointPolicyPureVector>(
						args.maxplus_maxiter, args.maxplus_updateT,
						args.maxplus_verbose, args.maxplus_damping,
						args.k, // <- nr solutions to return by BG solver
						args.maxplus_nrRestarts));
				}

				break;

			case BnB:

				if(args.useBGclustering) {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_BnB<JointPolicyPureVectorForClusteredBG>(
						 args.verbose -2 , args.k, args.BnB_keepAll,
						 args.BnBJointTypeOrdering,
						 args.BnB_consistentCompleteInformationHeur));
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_BnB<JointPolicyPureVector>(
						 args.verbose -2 , args.k, args.BnB_keepAll,
						 args.BnBJointTypeOrdering,
						 args.BnB_consistentCompleteInformationHeur));
				}

				break;

			case CGBG_MaxPlus:
				throw E("CGBG max plus only works with factored Dec-POMDPs (use GMAAF instead)"
						" - provided as part of the MADP Toolbox.");
				break;

			case Random:

				if(args.useBGclustering) {
					throw E("Random not implemented for clustered GMAA");
				} else {
					bgipsc_p = std::unique_ptr<BGIP_SolverCreatorInterface>(new BGIP_SolverCreator_Random(args.verbose, args.k));
				}

				break;

			default:
				throw E("BGIP Solver not handled");
				break;
		}

		if(bgipsc_p == 0) {
			throw E("No BGIP Solver Creator instantiated");
		}
	}

	return bgipsc_p;
}
