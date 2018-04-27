#include "PreviousPlanWizard.h"

// Qt
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFileDialog>


PreviousPlanWizard::PreviousPlanWizard(QWidget* parent) : QWizard(parent) {
	// Set pages
	setPage(ProblemSelection, new PPProblemSelectionPage());
	setPage(FFProblemOptions, new PPFireFightingProblemOptionsPage());
	setPage(AlohaProblemOptions, new PPAlohaProblemOptionsPage());
	setPage(GeneralOptions, new PPGeneralOptionsPage());

	setButtonText(QWizard::FinishButton, "Load");

	// Start on the planner selection page, set the title of the wizard
	setStartId(ProblemSelection);
	setWindowTitle("Load a Policy");

	// Connect finish button to our slot
	connect(button(QWizard::FinishButton), SIGNAL(clicked()),
			this, SLOT(FinishButtonClicked()));
}

PreviousPlanWizard::~PreviousPlanWizard() {
	//std::cout << "~PreviousPlanWizard()" << std::endl;
}


void PreviousPlanWizard::FinishButtonClicked() {
	// Arguments to return
	ArgumentHandlers::Arguments args;

	// Extract file path
	std::string policyFilePath = field("policyFilePath").toString().trimmed().toStdString();

	// If parsing a problem
	if(field("parseProblem").toBool()) {
		args.problem_type = ProblemType::PARSE;

		// Set the dpf to the path given by the user
		char* problemFilePath =
			new char[field("parseProblemFilePath").toString().trimmed().toStdString().length()+1];

		strcpy(problemFilePath,
			   field("parseProblemFilePath").toString().trimmed().toStdString().c_str());

		args.dpf = problemFilePath;
	} else {
		// Get problem type selected
		if(field("aloha").toBool()) {
			args.problem_type = ProblemType::Aloha;
			SetAlohaProblemOptions(args);

		} else if(field("decTiger").toBool()) {
			args.problem_type = ProblemType::DT;

		} else if(field("fireFighting").toBool()) {
			args.problem_type = ProblemType::FF;
			SetFFProblemOptions(args);

		} else if(field("fireFightingFactored").toBool()) {
			args.problem_type = ProblemType::FFF;
			SetFFProblemOptions(args);

		} else if(field("fireFightingGraph").toBool()) {
			args.problem_type = ProblemType::FFG;
			SetFFProblemOptions(args);
		}
	}

	emit PreviousPlan(policyFilePath, args);
}


void PreviousPlanWizard::SetAlohaProblemOptions(ArgumentHandlers::Arguments &args) {
	args.alohaVariation = static_cast<ProblemAloha::AlohaVariation>(
				field("alohaProblemVariation").toInt());
	args.islandConf = static_cast<ProblemAloha::IslandConfiguration>(
				field("alohaProblemIslandConfig").toInt());
	args.maxBacklog = field("alohaProblemMaxBacklog").toInt();
}


void PreviousPlanWizard::SetFFProblemOptions(ArgumentHandlers::Arguments &args) {
	args.nrAgents = field("ffProblemNumAgents").toInt();
	args.extinguishProb = field("ffProblemExtinghishProbability").toDouble();
	args.nrFLs = field("ffProblemFireLevels").toInt();
	args.nrHouses = field("ffProblemNumHouses").toInt();
}


PPProblemSelectionPage::PPProblemSelectionPage(QWidget* parent) : QWizardPage(parent) {
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
	problemBrowseBtn = new QPushButton("Browse", browseFieldWrapper);
	problemFilePath = new QLineEdit(browseFieldWrapper);

	// Setup layouts
	cppProblemBoxLayout->addWidget(alohaProblemRadioBtn);
	cppProblemBoxLayout->addWidget(decTigerProblemRadioBtn);
	cppProblemBoxLayout->addWidget(ffProblemRadioBtn);
	cppProblemBoxLayout->addWidget(fffProblemRadioBtn);
	cppProblemBoxLayout->addWidget(ffgProblemRadioBtn);

	browseFieldLayout->addWidget(problemFilePath);
	browseFieldLayout->addWidget(problemBrowseBtn);

	specifyProblemBoxLayout->addWidget(cppProblemRadioBtn);
	specifyProblemBoxLayout->addWidget(cppClassProblemsGroupBox);
	specifyProblemBoxLayout->addWidget(parseProblemRadioBtn);
	specifyProblemBoxLayout->addWidget(browseFieldWrapper);

	// Disable class based problems as default is to parse
	cppClassProblemsGroupBox->setEnabled(false);
	parseProblemRadioBtn->setChecked(true);

	// Connect browse button to slot and others to
	// alert interface enabling methods
	connect(cppProblemRadioBtn, &QRadioButton::toggled,
			this, &PPProblemSelectionPage::ClassParseRadioBtnToggle);
	connect(parseProblemRadioBtn, &QRadioButton::toggled,
			this, &PPProblemSelectionPage::ClassParseRadioBtnToggle);
	connect(problemBrowseBtn, &QPushButton::clicked,
			this, &PPProblemSelectionPage::ProblemBrowseBtnClicked);
	connect(problemFilePath, &QLineEdit::textChanged,
			this, &PPProblemSelectionPage::completeChanged);

	// Alert complete changed when radio button pressed
	connect(alohaProblemRadioBtn, &QRadioButton::pressed,
			this, &PPProblemSelectionPage::completeChanged);
	connect(decTigerProblemRadioBtn, &QRadioButton::pressed,
			this, &PPProblemSelectionPage::completeChanged);
	connect(ffProblemRadioBtn, &QRadioButton::pressed,
			this, &PPProblemSelectionPage::completeChanged);
	connect(fffProblemRadioBtn, &QRadioButton::pressed,
			this, &PPProblemSelectionPage::completeChanged);
	connect(ffgProblemRadioBtn, &QRadioButton::pressed,
			this, &PPProblemSelectionPage::completeChanged);

	// Add to main layout
	pageLayout->addWidget(specifyProblemGroupBox);
	RegisterFields();

	// Set the layout
	setLayout(pageLayout);
}


void PPProblemSelectionPage::RegisterFields() {
	registerField("cppProblem", cppProblemRadioBtn);
	registerField("parseProblem", parseProblemRadioBtn);
	registerField("aloha", alohaProblemRadioBtn);
	registerField("decTiger", decTigerProblemRadioBtn);
	registerField("fireFighting", ffProblemRadioBtn);
	registerField("fireFightingFactored", fffProblemRadioBtn);
	registerField("fireFightingGraph", ffgProblemRadioBtn);
	registerField("parseProblemFilePath", problemFilePath);
}


int PPProblemSelectionPage::nextId() const {

	// If cpp problem selected
	if(cppProblemRadioBtn->isChecked()) {

		// Check if problem specific options need setting and return those
		if(alohaProblemRadioBtn->isChecked()) {
			return PreviousPlanWizard::AlohaProblemOptions;

		} else if(ffProblemRadioBtn->isChecked() || fffProblemRadioBtn->isChecked()
				  || ffgProblemRadioBtn->isChecked()) {
			return PreviousPlanWizard::FFProblemOptions;
		}
	}

	// If we get here, it's either parse or no problem specific options need setting
	return PreviousPlanWizard::GeneralOptions;
}


bool PPProblemSelectionPage::isComplete() const {

	// If parsing
	if(parseProblemRadioBtn->isChecked()) {

		// Has the user given the file
		return !problemFilePath->text().isEmpty();
	} else {

		// Class based problem, return if one of the radio buttons is checked
		return  (alohaProblemRadioBtn->isChecked() || decTigerProblemRadioBtn->isChecked()
				 || ffProblemRadioBtn->isChecked() || ffgProblemRadioBtn->isChecked()
				 || fffProblemRadioBtn->isChecked());
	}
}


void PPProblemSelectionPage::ClassParseRadioBtnToggle() {
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

	emit completeChanged();
}


void PPProblemSelectionPage::ProblemBrowseBtnClicked() {
	QString directory = QDir::toNativeSeparators(
			QFileDialog::getOpenFileName(this,
										 "Open Problem File",
										 QDir::homePath()));

	problemFilePath->setText(directory);
	emit completeChanged();
}


PPFireFightingProblemOptionsPage::PPFireFightingProblemOptionsPage(QWidget *parent) : QWizardPage(parent) {
	setTitle("Fire Fighting Problem Options");
	setSubTitle("Set the Fire Fighting Problem options here");

	// Get args to set fields to default values
	ArgumentHandlers::Arguments args;

	// Create layout
	QFormLayout* formLayout = new QFormLayout(this);

	// Create items
	ffProblemNumAgentsSpinBox = new QSpinBox();
	ffProblemFireLevelsSpinBox = new QSpinBox();
	ffProblemNumHousesSpinBox = new QSpinBox();

	// Set default values
	ffProblemNumAgentsSpinBox->setValue(args.nrAgents);
	ffProblemFireLevelsSpinBox->setValue(args.nrFLs);
	ffProblemNumHousesSpinBox->setValue(args.nrHouses);

	// Add items
	formLayout->addRow("Number of Agents", ffProblemNumAgentsSpinBox);
	formLayout->addRow("Fire Levels", ffProblemFireLevelsSpinBox);
	formLayout->addRow("Num Houses", ffProblemNumHousesSpinBox);

	// Register the fields
	RegisterFields();
	setLayout(formLayout);
}


int PPFireFightingProblemOptionsPage::nextId() const {
	return PreviousPlanWizard::GeneralOptions;
}


void PPFireFightingProblemOptionsPage::RegisterFields() {
	registerField("ffProblemNumAgents", ffProblemNumAgentsSpinBox);
	registerField("ffProblemFireLevels", ffProblemFireLevelsSpinBox);
	registerField("ffProblemNumHouses", ffProblemNumHousesSpinBox);
}


PPAlohaProblemOptionsPage::PPAlohaProblemOptionsPage(QWidget* parent) : QWizardPage(parent) {
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


int PPAlohaProblemOptionsPage::nextId() const {
	return PreviousPlanWizard::GeneralOptions;
}


void PPAlohaProblemOptionsPage::RegisterFields() {
	registerField("alohaProblemMaxBacklog", alohaProblemMaxBacklogSpinBox);
	registerField("alohaProblemIslandConfig", alohaProblemIslandConfigurationComboBox);
	registerField("alohaProblemVariation", alohaProblemVariationComboBox);
}


PPGeneralOptionsPage::PPGeneralOptionsPage(QWidget* parent) : QWizardPage(parent) {
	QVBoxLayout* pageLayout = new QVBoxLayout();

	QWidget* policyFilePathWrapper = new QWidget();
	QHBoxLayout* policyFilePathLayout = new QHBoxLayout(policyFilePathWrapper);

	policyFilePath = new QLineEdit(policyFilePathWrapper);
	policyBrowseBtn = new QPushButton("Browse", policyFilePathWrapper);

	policyFilePathLayout->addWidget(policyFilePath);
	policyFilePathLayout->addWidget(policyBrowseBtn);

	pageLayout->addWidget(new QLabel("Enter the path of the policy file saved from the MADP Toolbox here",
									 policyFilePathWrapper));
	pageLayout->addWidget(policyFilePathWrapper);

	connect(policyBrowseBtn, &QPushButton::clicked, this, &PPGeneralOptionsPage::PolicyBrowseBtnClicked);
	connect(policyFilePath, &QLineEdit::textChanged, this, &PPGeneralOptionsPage::completeChanged);

	RegisterFields();

	setLayout(pageLayout);
}


int PPGeneralOptionsPage::nextId() const {
	// End of wizard so -1
	return -1;
}


bool PPGeneralOptionsPage::isComplete() const {
	// If empty not complete
	return !policyFilePath->text().isEmpty();
}


void PPGeneralOptionsPage::RegisterFields() {
	registerField("policyFilePath", policyFilePath);
}


void PPGeneralOptionsPage::PolicyBrowseBtnClicked() {
	// Get directory and set text
	QString directory = QDir::toNativeSeparators(
			QFileDialog::getOpenFileName(this,
										 "Open Policy File",
										 QDir::homePath()));

	policyFilePath->setText(directory);
}
