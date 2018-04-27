#include "StartPlanWizard.h"

// TreeVis files
#include "PlannerManager.h"
#include "MainWindow.h"

// MADP Files
#include "argumentUtils.h"
#include "gmaatype.h"
#include "BnB_JointTypeOrdering.h"
#include "qheur.h"
#include "ProblemAloha.h"

// QT
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

StartPlanWizard::StartPlanWizard(QWidget* parent) : QWizard(parent) {

	// Set pages
	setPage(PlannerSelection, new SPPlannerSelectionPage());
	setPage(GMAAPlannerOptions, new SPGMAAPlannerOptionsPage());
	setPage(JESPPlannerOptions, new SPJESPPlannerOptionsPage());
	setPage(DICEPlannerOptions, new SPDICEPlannerOptionsPage());
	setPage(ProblemSelection, new SPProblemSelectionPage());
	setPage(FFProblemOptions, new SPFireFightingProblemOptionsPage());
	setPage(AlohaProblemOptions, new SPAlohaProblemOptionsPage());
	setPage(GeneralProblemOptions, new SPGeneralProblemOptionsPage());

	setButtonText(QWizard::FinishButton, "Plan");

	// Connect finish button to our method
	connect(button(QWizard::FinishButton), &QAbstractButton::clicked,
			this, &StartPlanWizard::FinishButtonClicked);

	setMinimumSize(900, 500);

	// Start on the planner selection page, set the title of the wizard
	setStartId(PlannerSelection);
	setWindowTitle("Start a Plan");
}


StartPlanWizard::~StartPlanWizard() {
	//std::cout << "~StartPlanWiard()" << std::endl;
}


void StartPlanWizard::FinishButtonClicked() {
	// Get planner type and our args to send
	PlannerManager::PlannerType type = GetPlannerType();
	ArgumentHandlers::Arguments args;

	// Set problem based options
	SetProblemType(args);
	SetProblemOptions(args);

	// Set planner specific problems
	switch(type) {

		case PlannerManager::BFS:
			// No planner specific options to set
			break;
		case PlannerManager::GMAA:
			SetGMAAOptions(args);
			break;
		case PlannerManager::JESP:
			SetJESPOptions(args);
			break;
		case PlannerManager::DICEPS:
			SetDICEPSOptions(args);
			break;
	}

	// Start Plan on Main Window giving the args and planner type
	emit StartPlan(type, args);
}


void StartPlanWizard::SetProblemType(ArgumentHandlers::Arguments &args) {

	// If the parse radio button is checked
	if(field("parseProblem").toBool()) {

		// Set the dpf to the path given by the user
		char* problemFilePath =
			new char[field("parseProblemFilePath").toString().trimmed().toStdString().length()+1];

		strcpy(problemFilePath,
			   field("parseProblemFilePath").toString().trimmed().toStdString().c_str());

		args.dpf = problemFilePath;
	} else {
		// Otherwise a class problem was given
		if(field("aloha").toBool()) {
			args.problem_type = ProblemType::Aloha;

		} else if(field("decTiger").toBool()) {
			args.problem_type = ProblemType::DT;

		} else if(field("fireFighting").toBool()) {
			args.problem_type = ProblemType::FF;

		} else if(field("fireFightingFactored").toBool()) {
			args.problem_type = ProblemType::FFF;

		} else if(field("fireFightingGraph").toBool()) {
			args.problem_type = ProblemType::FFG;
		}
	}
}


void StartPlanWizard::SetProblemOptions(ArgumentHandlers::Arguments &args) {
	args.horizon = field("horizon").toInt();
	args.nrRestarts = field("restarts").toInt();

	// If the discount has changed from the default
	if(field("discount").toDouble() != -1) {
		args.discount = field("discount").toDouble();
	}

	args.cache_flat_models = field("cacheFlatModels").toBool();
	args.sparse = field("sparse").toBool();

	// Set problem specific options, if needed
	switch(args.problem_type) {

		case ProblemType::FF:
		case ProblemType::FFF:
		case ProblemType::FFG:
			SetFFProblemOptions(args);
			break;

		case ProblemType::Aloha:
			SetAlohaProblemOptions(args);
			break;

		default:
			break;
	}
}


void StartPlanWizard::SetAlohaProblemOptions(ArgumentHandlers::Arguments &args) {
	args.alohaVariation = static_cast<ProblemAloha::AlohaVariation>(field("alohaProblemVariation").toInt());
	args.islandConf = static_cast<ProblemAloha::IslandConfiguration>(field("alohaProblemIslandConfig").toInt());
	args.maxBacklog = field("alohaProblemMaxBacklog").toInt();
}


void StartPlanWizard::SetFFProblemOptions(ArgumentHandlers::Arguments &args) {
	args.nrAgents = field("ffProblemNumAgents").toInt();
	args.extinguishProb = field("ffProblemExtinghishProbability").toDouble();
	args.nrFLs = field("ffProblemFireLevels").toInt();
	args.nrHouses = field("ffProblemNumHouses").toInt();
}


void StartPlanWizard::SetGMAAOptions(ArgumentHandlers::Arguments &args) {
	// GMAA Options
	args.nrAMRestarts = field("gmaaRestarts").toInt();
	args.k = field("gmaaK").toInt();
	args.gmaa = static_cast<GMAAtype::GMAA_t>(field("gmaaType").toInt());
	args.CE_use_hard_threshold = field("gmaaApproxInference").toBool();

	// BnB Options
	args.BnB_consistentCompleteInformationHeur = field("bnbCIHeuristic").toBool();
	args.BnB_keepAll = field("bnbKeepAll").toBool();
	args.BnBJointTypeOrdering = static_cast<BGIP_BnB::BnB_JointTypeOrdering>(field("bnbType").toInt());

	// CE Options
	args.CE_alpha = field("gmaaCeAlpha").toDouble();
	args.nrCERestarts = field("gmaaCeRestarts").toInt();
	args.nrCEIterations = field("gmaaCeIterations").toInt();
	args.nrCESamples = field("gmaaCeSamples").toInt();
	args.nrCESamplesForUpdate = field("gmaaCeUpdateSamples").toInt();
	args.CE_use_hard_threshold = field("gmaaCeNotStrictlyImproving").toBool();

	// Q Heuristic Options
	args.qheur = static_cast<qheur::Qheur_t>(field("qHurType").toInt());

	// Only two options
	args.QHybridFirstTS =
			field("qHurFirstTSHuristic").toInt() == 0 ? qheur::Qheur_t::eQBG : qheur::Qheur_t::eQPOMDP;

	qheur::Qheur_t QHybridLastTS;

	switch(field("qHurLastTSHeuristic").toInt()) {
		case 0:
			QHybridLastTS = qheur::Qheur_t::eQMDP;
			break;
		case 1:
			QHybridLastTS = qheur::Qheur_t::eQBG;
			break;
		default:
			QHybridLastTS = qheur::Qheur_t::eQPOMDP;
			break;
	}

	args.QHybridLastTS = QHybridLastTS;
	args.QHybridHorizonLastTimeSteps = field("qHurLastTSHorizon").toInt();
	args.acceleratedPruningThreshold = field("qHurPruningThreshold").toInt();
	args.TreeIPpruneAfterCrossSum = field("qHurPruneAfterSum").toBool();
	args.TreeIPpruneAfterUnion = field("qHurPruneAfterUnion").toBool();
	args.TreeIPuseVectorCache = field("qHurUseVectorCache").toBool();

	// Clustering options
	args.BGClusterAlgorithm = field("clusterAlgorithm").toInt();
	args.useBGclustering = field("useBGClustering").toBool();
	args.thresholdJB = field("jbThreshold").toDouble();
	args.thresholdPjaoh = field("jaohThreshold").toDouble();

	// Max Plus options
	args.maxplus_damping = field("maxPlusDampingValue").toDouble();
	args.maxplus_maxiter = field("maxPlusIterations").toInt();
	args.maxplus_nrRestarts = field("maxPlusRestarts").toInt();
	args.maxplus_updateT = field("maxPlusUpdate").toString().toStdString();

	// BGIP Solver Type options
	args.bgsolver = static_cast<BGIP_SolverType::BGIP_Solver_t>(field("bgipSolverType").toInt());
}


void StartPlanWizard::SetDICEPSOptions(ArgumentHandlers::Arguments &args) {
	args.CE_alpha = field("diceCeAlpha").toDouble();
	args.nrCERestarts = field("diceCeRestarts").toInt();
	args.nrCEIterations = field("diceCeIterations").toInt();
	args.nrCESamples = field("diceCeSamples").toInt();
	args.nrCESamplesForUpdate = field("diceCeUpdateSamples").toInt();
	args.CE_use_hard_threshold = field("diceCeNotStrictlyImproving").toBool();
}


void StartPlanWizard::SetJESPOptions(ArgumentHandlers::Arguments &args) {

	// Set jesp planner type
	if(field("jespDP").toBool()) {
		args.jesp = JESPtype::JESPDP;
	} else {
		args.jesp = JESPtype::JESPExhaustive;
	}
}


PlannerManager::PlannerType StartPlanWizard::GetPlannerType() {

	// Get the planner type they selected
	if(field("bfsRadioBtn").toBool()) {
		return PlannerManager::BFS;

	} else if(field("jespRadioBtn").toBool()) {
		return PlannerManager::JESP;

	} else if(field("gmaaRadioBtn").toBool()) {
		return PlannerManager::GMAA;

	} else {
		return PlannerManager::DICEPS;
	}
}


SPPlannerSelectionPage::SPPlannerSelectionPage(QWidget* parent) : QWizardPage(parent) {
	setTitle("Planning Method");
	setSubTitle("Select the method to use for planning");

	// Create radio buttons
	bfsRadioBtn = new QRadioButton("BFS");
	jespRadioBtn = new QRadioButton("JESP");
	gmaaRadioBtn = new QRadioButton("GMAA*");
	diceRadioBtn = new QRadioButton("DICEPS");

	// Default to BFS
	bfsRadioBtn->setChecked(true);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(bfsRadioBtn);
	layout->addWidget(jespRadioBtn);
	layout->addWidget(gmaaRadioBtn);
	layout->addWidget(diceRadioBtn);

	// Register the fields
	RegisterFields();
	setLayout(layout);
}


int SPPlannerSelectionPage::nextId() const {

	if(diceRadioBtn->isChecked()) {
		return StartPlanWizard::DICEPlannerOptions;

	} else if(jespRadioBtn->isChecked()) {
		return StartPlanWizard::JESPPlannerOptions;

	} else if(gmaaRadioBtn->isChecked()) {
		return StartPlanWizard::GMAAPlannerOptions;

	} else {
		return StartPlanWizard::ProblemSelection;
	}
}


void SPPlannerSelectionPage::RegisterFields() {
	registerField("bfsRadioBtn", bfsRadioBtn);
	registerField("jespRadioBtn", jespRadioBtn);
	registerField("gmaaRadioBtn", gmaaRadioBtn);
	registerField("diceRadioBtn", diceRadioBtn);
}

// ---

SPGeneralProblemOptionsPage::SPGeneralProblemOptionsPage(QWidget* parent) : QWizardPage(parent) {
	setTitle("General Problem Options");
	setSubTitle("Provide the general problem parameters here, such as the horizon. " \
				"Leave the discount value at -1 to use the default, changing will override.");

	// Get set of args to access the default values
	ArgumentHandlers::Arguments args;

	// Create layouts
	QFormLayout* pageLayout = new QFormLayout();

	// Create elements
	horizonSpinBox = new QSpinBox();
	restartsSpinBox = new QSpinBox();
	discountSpinBox = new QDoubleSpinBox();
	cacheFlatModelsCheckBox = new QCheckBox();
	sparseCheckBox = new QCheckBox();

	horizonSpinBox->setRange(1, std::numeric_limits<int>::max());
	restartsSpinBox->setRange(1, std::numeric_limits<int>::max());
	discountSpinBox->setRange(-1, 1);

	// Set default values
	horizonSpinBox->setValue(args.horizon);
	restartsSpinBox->setValue(args.nrRestarts);
	discountSpinBox->setValue(args.discount);
	cacheFlatModelsCheckBox->setChecked(args.cache_flat_models);
	sparseCheckBox->setChecked(args.sparse);

	// Add to page
	pageLayout->addRow("Horizon", horizonSpinBox);
	pageLayout->addRow("Restarts", restartsSpinBox);
	pageLayout->addRow("Discount", discountSpinBox);
	pageLayout->addRow("Cache Flat Models", cacheFlatModelsCheckBox);
	pageLayout->addRow("Sparse", sparseCheckBox);

	// Register the fields
	RegisterFields();

	setLayout(pageLayout);
}


int SPGeneralProblemOptionsPage::nextId() const {
	return -1;
}


void SPGeneralProblemOptionsPage::RegisterFields() {
	registerField("horizon", horizonSpinBox);
	registerField("restarts", restartsSpinBox);
	registerField("discount", discountSpinBox);
	registerField("cacheFlatModels", cacheFlatModelsCheckBox);
	registerField("sparse", sparseCheckBox);
}

// ---

SPProblemSelectionPage::SPProblemSelectionPage(QWidget* parent) : QWizardPage(parent) {
	setTitle("Problem Selection");
	setSubTitle("Select either one of the class based problems or browse for a problem file that will be parsed.");

	// Create layouts
	QVBoxLayout* pageLayout = new QVBoxLayout();

	QGroupBox* specifyProblemGroupBox = new QGroupBox("Specify Problem");
	cppClassProblemsGroupBox = new QGroupBox("Select Problem");

	QVBoxLayout* specifyProblemBoxLayout = new QVBoxLayout(specifyProblemGroupBox);
	QVBoxLayout* cppProblemBoxLayout = new QVBoxLayout(cppClassProblemsGroupBox);

	browseFieldWrapper = new QWidget();
	QHBoxLayout* browseFieldLayout = new QHBoxLayout(browseFieldWrapper);

	// Create buttons
	cppProblemRadioBtn = new QRadioButton("C++ Class Problem");
	parseProblemRadioBtn = new QRadioButton("Parse an Input File");

	alohaProblemRadioBtn = new QRadioButton("Aloha");
	decTigerProblemRadioBtn = new QRadioButton("Dec Tiger");
	ffProblemRadioBtn = new QRadioButton("Fire Fighting");
	fffProblemRadioBtn = new QRadioButton("Fire Fighting Factored");
	ffgProblemRadioBtn = new QRadioButton("Fire Fighting Graph");

	// Browse button and path field
	browseBtn = new QPushButton("Browse", browseFieldWrapper);
	problemFilePath = new QLineEdit(browseFieldWrapper);

	// Setup layouts
	cppProblemBoxLayout->addWidget(alohaProblemRadioBtn);
	cppProblemBoxLayout->addWidget(decTigerProblemRadioBtn);
	cppProblemBoxLayout->addWidget(ffProblemRadioBtn);
	cppProblemBoxLayout->addWidget(fffProblemRadioBtn);
	cppProblemBoxLayout->addWidget(ffgProblemRadioBtn);

	browseFieldLayout->addWidget(problemFilePath);
	browseFieldLayout->addWidget(browseBtn);

	specifyProblemBoxLayout->addWidget(cppProblemRadioBtn);
	specifyProblemBoxLayout->addWidget(cppClassProblemsGroupBox);
	specifyProblemBoxLayout->addWidget(parseProblemRadioBtn);
	specifyProblemBoxLayout->addWidget(browseFieldWrapper);

	// Disable class based problems as default is to parse
	cppClassProblemsGroupBox->setEnabled(false);
	parseProblemRadioBtn->setChecked(true);

	// Connect browse button to slot and others to alert interface enabling methods
	connect(cppProblemRadioBtn, &QRadioButton::toggled, this, &SPProblemSelectionPage::ClassParseRadioBtnToggle);
	connect(parseProblemRadioBtn, &QRadioButton::toggled, this, &SPProblemSelectionPage::ClassParseRadioBtnToggle);
	connect(browseBtn, &QPushButton::clicked, this, &SPProblemSelectionPage::BrowseBtnClicked);
	connect(problemFilePath, &QLineEdit::textChanged, this, &SPProblemSelectionPage::completeChanged);

	// Alert complete changed when radio button pressed
	connect(alohaProblemRadioBtn, &QRadioButton::pressed, this, &SPProblemSelectionPage::completeChanged);
	connect(decTigerProblemRadioBtn, &QRadioButton::pressed, this, &SPProblemSelectionPage::completeChanged);
	connect(ffProblemRadioBtn, &QRadioButton::pressed, this, &SPProblemSelectionPage::completeChanged);
	connect(fffProblemRadioBtn, &QRadioButton::pressed, this, &SPProblemSelectionPage::completeChanged);
	connect(ffgProblemRadioBtn, &QRadioButton::pressed, this, &SPProblemSelectionPage::completeChanged);

	// Register the fields
	RegisterFields();

	pageLayout->addWidget(specifyProblemGroupBox);
	setLayout(pageLayout);
}


int SPProblemSelectionPage::nextId() const {

	// If cpp problem selected
	if(cppProblemRadioBtn->isChecked()) {

		// Check if problem specific options need setting and return those
		if(alohaProblemRadioBtn->isChecked()) {
			return StartPlanWizard::AlohaProblemOptions;
		} else if(ffProblemRadioBtn->isChecked() || fffProblemRadioBtn->isChecked() || ffgProblemRadioBtn->isChecked()) {
			return StartPlanWizard::FFProblemOptions;
		}
	}

	// If we get here, it's either parse or no problem specific options need setting
	return StartPlanWizard::GeneralProblemOptions;
}


void SPProblemSelectionPage::RegisterFields() {
	registerField("cppProblem", cppProblemRadioBtn);
	registerField("parseProblem", parseProblemRadioBtn);
	registerField("aloha", alohaProblemRadioBtn);
	registerField("decTiger", decTigerProblemRadioBtn);
	registerField("fireFighting", ffProblemRadioBtn);
	registerField("fireFightingFactored", fffProblemRadioBtn);
	registerField("fireFightingGraph", ffgProblemRadioBtn);
	registerField("parseProblemFilePath", problemFilePath);
}


void SPProblemSelectionPage::BrowseBtnClicked() {
	// Get directory
	QString directory = QDir::toNativeSeparators(
				QFileDialog::getOpenFileName(
					this,
					"Open Problem File",
					QDir::homePath()));

	// Set directory
	problemFilePath->setText(directory);
}


bool SPProblemSelectionPage::isComplete() const {

	// If parsing
	if(parseProblemRadioBtn->isChecked()) {

		// Has the user given the file
		return !problemFilePath->text().isEmpty();
	} else {

		// Class based problem, return if one of the radio buttons is checked
		return (alohaProblemRadioBtn->isChecked() || decTigerProblemRadioBtn->isChecked() || ffProblemRadioBtn->isChecked() ||
				ffgProblemRadioBtn->isChecked() || fffProblemRadioBtn->isChecked());
	}
}


void SPProblemSelectionPage::ClassParseRadioBtnToggle() {

	// Get sender
	QRadioButton* sender = static_cast<QRadioButton*>(QObject::sender());

	// If cpp problem button was clicked
	if(sender == cppProblemRadioBtn) {
		cppClassProblemsGroupBox->setEnabled(true);
		browseFieldWrapper->setEnabled(false);
	} else {
		browseFieldWrapper->setEnabled(true);
		cppClassProblemsGroupBox->setEnabled(false);
	}
}

// ---

SPGMAAPlannerOptionsPage::SPGMAAPlannerOptionsPage(QWidget* parent) : QWizardPage(parent) {
	setTitle("GMAA Planner Options");
	setSubTitle("Set the options to use for the GMAA Planner");

	// Get args to set fields to default values
	ArgumentHandlers::Arguments args;

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QScrollArea* scrollArea = new QScrollArea(this);

	scrollArea->setWidgetResizable(true);
	mainLayout->addWidget(scrollArea);

	QWidget* scrollAreaContents = new QWidget();
	scrollArea->setWidget(scrollAreaContents);

	// Grid layout for 'Global' Layout of the page
	QGridLayout* gridLayout = new QGridLayout(scrollAreaContents);

	// Create GMAA Options Interface
	QGroupBox* gmaaOptionsGroupBox = new QGroupBox("GMAA* Options");
	QFormLayout* gmaaOptionsFormLayout = new QFormLayout(gmaaOptionsGroupBox);

	// Create items
	gmaaRestartsSpinBox = new QSpinBox();
	gmaaKSpinBox = new QSpinBox();
	gmaaTypeComboBox = new QComboBox();
	gmaaTypeComboBox->insertItems(0, QStringList() << "MAAStar" << "kGMAA"
												   << "FSPC" << "MAAStarClassic");
	gmaaApproxInferenceCheckBox = new QCheckBox();

	gmaaRestartsSpinBox->setRange(1, std::numeric_limits<int>::max());
	gmaaKSpinBox->setRange(1, std::numeric_limits<int>::max());

	// Set default values
	gmaaRestartsSpinBox->setValue(args.nrAMRestarts);
	gmaaKSpinBox->setValue(args.k);
	gmaaApproxInferenceCheckBox->setChecked(args.exactBGs);

	// Add items
	gmaaOptionsFormLayout->addRow("AM Restarts", gmaaRestartsSpinBox);
	gmaaOptionsFormLayout->addRow("K Value", gmaaKSpinBox);
	gmaaOptionsFormLayout->addRow("GMAA Type", gmaaTypeComboBox);
	gmaaOptionsFormLayout->addRow("Approx Interface", gmaaApproxInferenceCheckBox);


	// Create BnB Options Interface
	QGroupBox* bnbOptionsGroupBox = new QGroupBox("Branch and Bound Options");
	QFormLayout* bnbOptionsFormLayout = new QFormLayout(bnbOptionsGroupBox);

	// Create items
	bnbCIHeuristicCheckBox = new QCheckBox();
	bnbKeepAllCheckBox = new QCheckBox();
	bnbTypeComboBox = new QComboBox();
	bnbTypeComboBox->insertItems(0, QStringList() << "Identity Mapping" << "Max Contribution"
												  << "Min Contribution" << "Max Contribution Difference"
												  << "Descending Probability" << "Basis Types"
												  << "Consistent Max Contribution" << "Consistent Min Contribution"
												  << "Consistent Max Contribution Difference");

	// Set default values
	bnbCIHeuristicCheckBox->setChecked(args.BnB_consistentCompleteInformationHeur);
	bnbKeepAllCheckBox->setChecked(args.BnB_keepAll);

	// Add items
	bnbOptionsFormLayout->addRow("Use CI Heuristic", bnbCIHeuristicCheckBox);
	bnbOptionsFormLayout->addRow("Keep All", bnbKeepAllCheckBox);
	bnbOptionsFormLayout->addRow("Ordering Type", bnbTypeComboBox);


	// Create CE Options Interface
	QGroupBox* ceOptionsGroupBox = new QGroupBox("CE Options");
	QFormLayout* ceOptionsFormLayout = new QFormLayout(ceOptionsGroupBox);

	// Create items
	ceAlphaSpinBox = new QDoubleSpinBox();
	ceRestartsSpinBox = new QSpinBox();
	ceIterationsSpinBox = new QSpinBox();
	ceSamplesSpinBox = new QSpinBox();
	ceUpdateSamplesSpinBox = new QSpinBox();
	ceNotStrictlyImprovingCheckBox = new QCheckBox();

	ceRestartsSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceSamplesSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceUpdateSamplesSpinBox->setRange(1, std::numeric_limits<int>::max());

	// Set default values
	ceAlphaSpinBox->setValue(args.CE_alpha);
	ceRestartsSpinBox->setValue(args.nrCERestarts);
	ceIterationsSpinBox->setValue(args.nrCEIterations);
	ceSamplesSpinBox->setValue(args.nrCESamples);
	ceUpdateSamplesSpinBox->setValue(args.nrCESamplesForUpdate);
	ceNotStrictlyImprovingCheckBox->setChecked(args.CE_use_hard_threshold);

	// Add items
	ceOptionsFormLayout->addRow("Alpha", ceAlphaSpinBox);
	ceOptionsFormLayout->addRow("Restarts", ceRestartsSpinBox);
	ceOptionsFormLayout->addRow("Iterations", ceIterationsSpinBox);
	ceOptionsFormLayout->addRow("Samples", ceSamplesSpinBox);
	ceOptionsFormLayout->addRow("Update Samples", ceUpdateSamplesSpinBox);
	ceOptionsFormLayout->addRow("Not Strictly Improving", ceNotStrictlyImprovingCheckBox);


	// Create Q Heuristic Options Interface
	QGroupBox* qHurOptionsGroupBox = new QGroupBox("Q Heuristic Options");
	QFormLayout* qHurOptionsFormLayout = new QFormLayout(qHurOptionsGroupBox);

	// Create items
	qHurComboBox = new QComboBox();
	qHurComboBox->insertItems(0, QStringList() << "QMDP" << "QDOMDP" << "QBG" << "QMDPc" << "QPOMDPav"
											   << "QBGav" << "QHybrid" << "QPOMDPHybrid" << "QBGHybrid"
											   << "QBGTreeIncPrune" << "QBGTreeIncPruneBnB" << "");
	qHurFirstTHeuristicComboBox = new QComboBox();
	qHurFirstTHeuristicComboBox->insertItems(0, QStringList() << "QBG" << "QPOMDP");
	qHurLastTSHeuristicComboBox = new QComboBox();
	qHurLastTSHeuristicComboBox->insertItems(0, QStringList() << "QMDP" <<"QBG" << "QPOMDP");
	qHurHorLastTSpinBox = new QSpinBox();
	qHurAcceleratedPruningThresholdSpinBox = new QSpinBox();
	qHurPruneAfterCrossSumCheckBox = new QCheckBox();
	qHurPruneAfterUnionCheckBox = new QCheckBox();
	qHurUseVectorCacheCheckBox = new QCheckBox();

	// Set default values
	qHurAcceleratedPruningThresholdSpinBox->setMaximum(std::numeric_limits<int>::max());
	qHurHorLastTSpinBox->setValue(args.QHybridHorizonLastTimeSteps);
	qHurAcceleratedPruningThresholdSpinBox->setValue(args.acceleratedPruningThreshold);
	qHurPruneAfterCrossSumCheckBox->setChecked(args.TreeIPpruneAfterCrossSum);
	qHurPruneAfterUnionCheckBox->setChecked(args.TreeIPpruneAfterUnion);
	qHurUseVectorCacheCheckBox->setChecked(args.TreeIPuseVectorCache);

	// Default to undefined
	qHurComboBox->setCurrentIndex(11);

	// Add items
	qHurOptionsFormLayout->addRow("Q Heuristic to Use", qHurComboBox);
	qHurOptionsFormLayout->addRow("First Time Step Heuristic", qHurFirstTHeuristicComboBox);
	qHurOptionsFormLayout->addRow("Last Time Step Heuristic", qHurLastTSHeuristicComboBox);
	qHurOptionsFormLayout->addRow("Last Time Step Horizon", qHurHorLastTSpinBox);
	qHurOptionsFormLayout->addRow("Acc Pruning Threshold", qHurAcceleratedPruningThresholdSpinBox);
	qHurOptionsFormLayout->addRow("Prune After Cross Sum", qHurPruneAfterCrossSumCheckBox);
	qHurOptionsFormLayout->addRow("Prune After Union", qHurPruneAfterUnionCheckBox);
	qHurOptionsFormLayout->addRow("Use Vector Cache", qHurUseVectorCacheCheckBox);


	// Create Cluster Options Interface
	QGroupBox* clusterOptionsGroupBox = new QGroupBox("Clustsering Options");
	QFormLayout* clusterOptionsFormLayout = new QFormLayout(clusterOptionsGroupBox);

	// Create items
	clusterAlgorithmComboBox = new QComboBox();
	clusterAlgorithmComboBox->insertItems(0, QStringList() << "Lossless" << "ApproxJB"
														   << "ApproxPJAOH" << "ApproxPJAOHJB");
	clusterUseBGClustering = new QCheckBox();
	clusterJBThresholdSpinBox = new QDoubleSpinBox();
	clusterJAOHThresholdSpinBox = new QDoubleSpinBox();

	// Set default values
	clusterUseBGClustering->setChecked(args.useBGclustering != 0);
	clusterJBThresholdSpinBox->setValue(args.thresholdJB);
	clusterJAOHThresholdSpinBox->setValue(args.thresholdPjaoh);

	// Add items
	clusterOptionsFormLayout->addRow("Clustering Algorithm", clusterAlgorithmComboBox);
	clusterOptionsFormLayout->addRow("Use BG Clustering", clusterUseBGClustering);
	clusterOptionsFormLayout->addRow("JB Threshold", clusterJBThresholdSpinBox);
	clusterOptionsFormLayout->addRow("JAOH Threshold", clusterJAOHThresholdSpinBox);


	// Create Max Plus Options Interface
	QGroupBox* maxPlusOptionsGroupBox = new QGroupBox("Max Plus Options");
	QFormLayout* maxPlusOptionsFormLayout = new QFormLayout(maxPlusOptionsGroupBox);

	// Create Items
	maxPlusDampingSpinBox = new QDoubleSpinBox();
	maxPlusIterationsSpinBox = new QSpinBox();
	maxPlusRestartsSpinBox = new QSpinBox();
	maxPlusUpdateComboBox = new QComboBox();
	maxPlusUpdateComboBox->insertItems(0, QStringList() << "PARALL" << "SEQRND" << "SEQMAX");

	// Set default values
	maxPlusDampingSpinBox->setValue(args.maxplus_damping);
	maxPlusIterationsSpinBox->setValue(args.maxplus_maxiter);
	maxPlusRestartsSpinBox->setValue(args.maxplus_nrRestarts);

	// Add items
	maxPlusOptionsFormLayout->addRow("Damping Value", maxPlusDampingSpinBox);
	maxPlusOptionsFormLayout->addRow("Iterations", maxPlusIterationsSpinBox);
	maxPlusOptionsFormLayout->addRow("Restarts", maxPlusRestartsSpinBox);
	maxPlusOptionsFormLayout->addRow("Update", maxPlusUpdateComboBox);


	// Create BGIP Solver Options Interface
	QGroupBox* bgipOptionsGroupBox = new QGroupBox("BGIP Solver Options");
	QFormLayout* bgipOptionsFormLayout = new QFormLayout(bgipOptionsGroupBox);

	// Create items
	bgipSolverTypeComboBox = new QComboBox();
	bgipSolverTypeComboBox->insertItems(0, QStringList() << "BFS" << "AM" << "CE" << "MaxPlus"
											<< "BnB" << "BFSNonInc" << "CGBG_MaxPlus"
											 << "NDP" << "Random");

	// Add items
	bgipOptionsFormLayout->addRow("BGIP Solver Type", bgipSolverTypeComboBox);

	// Register the fields
	RegisterFields();

	// Add Group Boxes to the grid, defining how the wizard page is laid out
	gridLayout->addWidget(gmaaOptionsGroupBox, 0, 0);
	gridLayout->addWidget(bnbOptionsGroupBox, 0, 1);
	gridLayout->addWidget(ceOptionsGroupBox, 1, 0);
	gridLayout->addWidget(qHurOptionsGroupBox, 1, 1);
	gridLayout->addWidget(clusterOptionsGroupBox, 2, 0);
	gridLayout->addWidget(maxPlusOptionsGroupBox, 2, 1);
	gridLayout->addWidget(bgipOptionsGroupBox, 3, 0, 1, 2);

	mainLayout->addWidget(scrollArea);
	setLayout(mainLayout);

	this->resize(1000, 1000);

}


int SPGMAAPlannerOptionsPage::nextId() const {
	return StartPlanWizard::ProblemSelection;
}


void SPGMAAPlannerOptionsPage::RegisterFields() {
	registerField("gmaaRestarts", gmaaRestartsSpinBox);
	registerField("gmaaK", gmaaKSpinBox);
	registerField("gmaaType", gmaaTypeComboBox);
	registerField("gmaaApproxInference", gmaaApproxInferenceCheckBox);

	registerField("bnbCIHeuristic", bnbCIHeuristicCheckBox);
	registerField("bnbKeepAll", bnbKeepAllCheckBox);
	registerField("bnbType", bnbTypeComboBox);

	registerField("gmaaCeAlpha", ceAlphaSpinBox);
	registerField("gmaaCeRestarts", ceRestartsSpinBox);
	registerField("gmaaCeIterations", ceIterationsSpinBox);
	registerField("gmaaCeSamples", ceSamplesSpinBox);
	registerField("gmaaCeUpdateSamples", ceUpdateSamplesSpinBox);
	registerField("gmaaCeNotStrictlyImproving", ceNotStrictlyImprovingCheckBox);

	registerField("qHurType", qHurComboBox);
	registerField("qHurFirstTSHuristic", qHurFirstTHeuristicComboBox);
	registerField("qHurLastTSHeuristic", qHurLastTSHeuristicComboBox);
	registerField("qHurLastTSHorizon", qHurHorLastTSpinBox);
	registerField("qHurPruningThreshold", qHurAcceleratedPruningThresholdSpinBox);
	registerField("qHurPruneAfterSum", qHurPruneAfterCrossSumCheckBox);
	registerField("qHurPruneAfterUnion", qHurPruneAfterUnionCheckBox);
	registerField("qHurUseVectorCache", qHurUseVectorCacheCheckBox);

	registerField("clusterAlgorithm", clusterAlgorithmComboBox);
	registerField("useBGClustering", clusterUseBGClustering);
	registerField("jbThreshold", clusterJBThresholdSpinBox);
	registerField("jaohThreshold", clusterJAOHThresholdSpinBox);

	registerField("maxPlusDampingValue", maxPlusDampingSpinBox);
	registerField("maxPlusIterations", maxPlusIterationsSpinBox);
	registerField("maxPlusRestarts", maxPlusRestartsSpinBox);
	registerField("maxPlusUpdate", maxPlusUpdateComboBox);

	registerField("bgipSolverType", bgipSolverTypeComboBox);
}

// ---

SPJESPPlannerOptionsPage::SPJESPPlannerOptionsPage(QWidget *parent) : QWizardPage(parent) {
	setTitle("JESP Planner Options");
	setSubTitle("Set the options to use for the JESP Planner");

	jespDynamicProgrammingRadioButton = new QRadioButton("Dynamic Programming Planner");
	jespExhaustiveRadioButton = new QRadioButton("Exhaustive Planner");

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(jespDynamicProgrammingRadioButton);
	layout->addWidget(jespExhaustiveRadioButton);

	jespDynamicProgrammingRadioButton->setChecked(true);

	// Register the fields
	RegisterFields();

	setLayout(layout);
}


int SPJESPPlannerOptionsPage::nextId() const {
	return StartPlanWizard::ProblemSelection;
}


void SPJESPPlannerOptionsPage::RegisterFields() {
	registerField("jespDP", jespDynamicProgrammingRadioButton);
	registerField("jespExh", jespExhaustiveRadioButton);
}

// ---

SPDICEPlannerOptionsPage::SPDICEPlannerOptionsPage(QWidget *parent) : QWizardPage(parent) {
	setTitle("DICE Planner Options");
	setSubTitle("Set the options to use for the DICE Planner");

	// Get args to set fields to default values
	ArgumentHandlers::Arguments args;

	// Global layout for the wizard page
	QVBoxLayout* layout = new QVBoxLayout();

	// Create CE Options Interface
	QGroupBox* ceOptionsGroupBox = new QGroupBox("CE Options");
	QFormLayout* ceOptionsFormLayout = new QFormLayout(ceOptionsGroupBox);

	// Create items
	ceAlphaSpinBox = new QDoubleSpinBox();
	ceRestartsSpinBox = new QSpinBox();
	ceIterationsSpinBox = new QSpinBox();
	ceSamplesSpinBox = new QSpinBox();
	ceUpdateSamplesSpinBox = new QSpinBox();
	ceNotStrictlyImprovingCheckBox = new QCheckBox();

	ceRestartsSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceSamplesSpinBox->setRange(1, std::numeric_limits<int>::max());
	ceUpdateSamplesSpinBox->setRange(1, std::numeric_limits<int>::max());

	// Set default values
	ceAlphaSpinBox->setValue(args.CE_alpha);
	ceRestartsSpinBox->setValue(args.nrCERestarts);
	ceIterationsSpinBox->setValue(args.nrCEIterations);
	ceSamplesSpinBox->setValue(args.nrCESamples);
	ceUpdateSamplesSpinBox->setValue(args.nrCESamplesForUpdate);
	ceNotStrictlyImprovingCheckBox->setChecked(args.CE_use_hard_threshold);

	// Add items
	ceOptionsFormLayout->addRow("Alpha", ceAlphaSpinBox);
	ceOptionsFormLayout->addRow("Restarts", ceRestartsSpinBox);
	ceOptionsFormLayout->addRow("Iterations", ceIterationsSpinBox);
	ceOptionsFormLayout->addRow("Samples", ceSamplesSpinBox);
	ceOptionsFormLayout->addRow("Update Samples", ceUpdateSamplesSpinBox);
	ceOptionsFormLayout->addRow("Not Strictly Improving", ceNotStrictlyImprovingCheckBox);

	// Register the fields
	RegisterFields();

	// Add the layout and set it
	layout->addWidget(ceOptionsGroupBox);
	setLayout(layout);
}


int SPDICEPlannerOptionsPage::nextId() const {
	return StartPlanWizard::ProblemSelection;
}


void SPDICEPlannerOptionsPage::RegisterFields() {
	registerField("diceCeAlpha", ceAlphaSpinBox);
	registerField("diceCeRestarts", ceRestartsSpinBox);
	registerField("diceCeIterations", ceIterationsSpinBox);
	registerField("diceCeSamples", ceSamplesSpinBox);
	registerField("diceCeUpdateSamples", ceUpdateSamplesSpinBox);
	registerField("diceCeNotStrictlyImproving", ceNotStrictlyImprovingCheckBox);
}

// ---

SPFireFightingProblemOptionsPage::SPFireFightingProblemOptionsPage(QWidget* parent) : QWizardPage(parent) {
	setTitle("Fire Fighting Problem Options");
	setSubTitle("Set the Fire Fighting Problem options here");

	// Get args to set fields to default values
	ArgumentHandlers::Arguments args;

	// Create layout
	QFormLayout* formLayout = new QFormLayout(this);

	// Create items
	ffProblemNumAgentsSpinBox = new QSpinBox();
	ffProblemExtinguishProbSpinBox = new QDoubleSpinBox();
	ffProblemFireLevelsSpinBox = new QSpinBox();
	ffProblemNumHousesSpinBox = new QSpinBox();

	// Set default values
	ffProblemNumAgentsSpinBox->setValue(args.nrAgents);
	ffProblemExtinguishProbSpinBox->setValue(0); // No default value from args
	ffProblemFireLevelsSpinBox->setValue(args.nrFLs);
	ffProblemNumHousesSpinBox->setValue(args.nrHouses);

	// Add items
	formLayout->addRow("Number of Agents", ffProblemNumAgentsSpinBox);
	formLayout->addRow("Extinguish Probability", ffProblemExtinguishProbSpinBox);
	formLayout->addRow("Fire Levels", ffProblemFireLevelsSpinBox);
	formLayout->addRow("Num Houses", ffProblemNumHousesSpinBox);

	// Register the fields
	RegisterFields();
	setLayout(formLayout);
}


int SPFireFightingProblemOptionsPage::nextId() const {
	return StartPlanWizard::GeneralProblemOptions;
}


void SPFireFightingProblemOptionsPage::RegisterFields() {
	registerField("ffProblemNumAgents", ffProblemNumAgentsSpinBox);
	registerField("ffProblemExtinghishProbability", ffProblemExtinguishProbSpinBox);
	registerField("ffProblemFireLevels", ffProblemFireLevelsSpinBox);
	registerField("ffProblemNumHouses", ffProblemNumHousesSpinBox);
}

// ---

SPAlohaProblemOptionsPage::SPAlohaProblemOptionsPage(QWidget *parent) : QWizardPage(parent) {
	setTitle("Aloha Problem Options");
	setSubTitle("Set the Aloha Problem options here");

	// Get args to set fields to default values
	ArgumentHandlers::Arguments args;

	// Create layout
	QFormLayout* formLayout = new QFormLayout(this);

	// Create items
	alohaProblemMaxBacklogSpinBox = new QSpinBox();
	alohaProblemIslandConfigurationComboBox = new QComboBox();
	alohaProblemIslandConfigurationComboBox->insertItems(0, QStringList() << "Two Islands"
														 << "One Island" << "Two Independent Islands"
														 << "Three Islands In Line" << " Three Islands Clustered"
														 << "Small Big Small In Line" << "Five Islands In Line"
														 << "Four Islands In Line" << "Four Islands In Square"
														 << "Six Islands In Line" << "Seven Islands In Line" << "Inline");
	alohaProblemVariationComboBox = new QComboBox();
	alohaProblemVariationComboBox->insertItems(0, QStringList() << "No New Packet" << "New Packet"
											   << "New Packet Send All" << "New Packet Progressive Penalty");

	// Set default values
	alohaProblemMaxBacklogSpinBox->setValue(args.maxBacklog);
	alohaProblemIslandConfigurationComboBox->setCurrentIndex(11);

	// Add items
	formLayout->addRow("Max Backlog", alohaProblemMaxBacklogSpinBox);
	formLayout->addRow("Island Configuration", alohaProblemIslandConfigurationComboBox);
	formLayout->addRow("Variation", alohaProblemVariationComboBox);

	// Register the fields
	RegisterFields();
	setLayout(formLayout);
}


int SPAlohaProblemOptionsPage::nextId() const {
	return StartPlanWizard::GeneralProblemOptions;
}


void SPAlohaProblemOptionsPage::RegisterFields() {
	registerField("alohaProblemMaxBacklog", alohaProblemMaxBacklogSpinBox);
	registerField("alohaProblemIslandConfig", alohaProblemIslandConfigurationComboBox);
	registerField("alohaProblemVariation", alohaProblemVariationComboBox);
}
