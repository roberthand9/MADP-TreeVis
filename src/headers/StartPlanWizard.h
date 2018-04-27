#ifndef STARTPLANWIZARD_H
#define STARTPLANWIZARD_H

// TreeVis
#include "PlannerManager.h"

// Qt
#include <QWidget>
#include <QWizard>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLayout>
#include <QFormLayout>
#include <QGroupBox>


///
/// \brief A wizard that allows the user to start a plan via
/// a GUI interface that can then be directly used by
/// the visualisers. The functionality is provided in the
/// signal StartPlan that emits giving the planner type
/// chosen and the aruments of the problem to plan for
///
class StartPlanWizard : public QWizard {
	Q_OBJECT

	public:
		///
		/// \brief Creates pages for the wizard, sets the start page.
		/// \param parent The parent of the wizard
		///
		StartPlanWizard(QWidget* parent = 0);
		~StartPlanWizard();

		/// The page IDs in the wizard
		enum pages {
			PlannerSelection,
			GMAAPlannerOptions,
			JESPPlannerOptions,
			DICEPlannerOptions,
			ProblemSelection,
			FFProblemOptions,
			AlohaProblemOptions,
			GeneralProblemOptions
		};

	signals:
		///
		/// \brief Signal emitted when all options have
		/// been specified. Should be connected
		/// to by the creator to recieve
		/// \param type The planner type to use to plan
		/// \param args The arguments of the problem to plan for
		///
		void StartPlan(PlannerManager::PlannerType type, ArgumentHandlers::Arguments args);

	private slots:
		/// \brief Finish button clicked on the wizard,
		/// emit the stat plan signal as finished
		void FinishButtonClicked();

	private:
		///
		/// \brief Gets the planner type chosen by the user
		/// \return The type of planner chosen by the user
		///
		PlannerManager::PlannerType GetPlannerType();

		///
		/// \brief Handles parsing and setting the problem type based on the wizard supplied values.
		/// \param args The reference of the arguments struct to set the problem type on.
		///
		void SetProblemType(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the problem options including problem specific options if the user has
		/// chosen a class based problem where specific options are applicable.
		/// \param args The reference of the arguments struct to set the problem options on.
		///
		void SetProblemOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the parameters on the given arguments for the C++ class based Aloha Problem.
		/// \param args The reference of the arguments struct to set the Aloha options on.
		///
		void SetAlohaProblemOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the parameters on the given arguments for the C++ class based Fire Fighting Problems.
		/// \param args The reference of the arguments struct to set the Fire Fighting options on.
		///
		void SetFFProblemOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the GMAA Specific planner options, for example, the BnB Options.
		/// \param args The reference of the arguments struct to set the GMAA Options on.
		///
		void SetGMAAOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the JESP Solver type.
		/// \param args The reference of the arguments struct to set the JESP type on.
		///
		void SetDICEPSOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets the DICE Planners specific CE Options.
		/// \param args The reference of the arguments struct to set the DICE Options on.
		///
		void SetJESPOptions(ArgumentHandlers::Arguments &args);
};

///
/// \brief The PlannerSelectionPage class
/// represents the page of the wizard that
/// allows the user to select the planner to use
///
class SPPlannerSelectionPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPPlannerSelectionPage(QWidget* parent = 0);

		///
		/// \return The next page id of the wizard. This
		/// will be based on the users chosen planner
		///
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Elements
		QRadioButton* bfsRadioBtn;
		QRadioButton* jespRadioBtn;
		QRadioButton* gmaaRadioBtn;
		QRadioButton* diceRadioBtn;
};

///
/// \brief This page allows the user to choose options that are
/// present for every problem, for example, the horizon.
///
class SPGeneralProblemOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPGeneralProblemOptionsPage(QWidget* parent = 0);

		///
		/// \return The next page id of the wizard. Final
		/// page so will return -1
		///
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Elements
		QSpinBox* horizonSpinBox;
		QSpinBox* restartsSpinBox;
		QDoubleSpinBox* discountSpinBox;
		QCheckBox* cacheFlatModelsCheckBox;
		QCheckBox* sparseCheckBox;
};

///
/// \brief Allows the user to select the problem either as
/// a class based one or specify that the problem should
/// be parsed from a file
///
class SPProblemSelectionPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPProblemSelectionPage(QWidget* parent = 0);

		///
		/// \return The next page id of the wizard based on
		/// if the user has chosen a class based
		/// problem or to parse
		///
		int nextId() const override;

		///
		/// \return True if the page is complete and the
		/// wizard can proceed, false otherwize
		///
		bool isComplete() const override;

	private slots:
		///
		/// \brief Slot called when either the class based problem
		/// or parse radio buttons are pressed
		///
		void ClassParseRadioBtnToggle();

		/// Browse button for problem path clicked
		void BrowseBtnClicked();

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Buttons and fields
		QRadioButton* cppProblemRadioBtn;
		QRadioButton* parseProblemRadioBtn;
		QRadioButton* alohaProblemRadioBtn;
		QRadioButton* decTigerProblemRadioBtn;
		QRadioButton* ffProblemRadioBtn;
		QRadioButton* fffProblemRadioBtn;
		QRadioButton* ffgProblemRadioBtn;
		QPushButton* browseBtn;
		QLineEdit* problemFilePath;

		// Class based so we can disable them
		QGroupBox* cppClassProblemsGroupBox;
		QWidget* browseFieldWrapper;
};

///
/// \brief Allows the user to set the GMAA Planner specific options
///
class SPGMAAPlannerOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPGMAAPlannerOptionsPage(QWidget* parent = 0);

		/// \return The ID of the problem selection page
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// BGIP Solver options
		QComboBox* bgipSolverTypeComboBox;

		// Branch and Bound Options
		QCheckBox* bnbCIHeuristicCheckBox;
		QCheckBox* bnbKeepAllCheckBox;
		QComboBox* bnbTypeComboBox;

		// Q Heuristic Options
		QSpinBox* qHurAcceleratedPruningThresholdSpinBox;
		QComboBox* qHurFirstTHeuristicComboBox;
		QComboBox* qHurLastTSHeuristicComboBox;
		QSpinBox* qHurHorLastTSpinBox;
		QCheckBox* qHurPruneAfterCrossSumCheckBox;
		QCheckBox* qHurPruneAfterUnionCheckBox;
		QComboBox* qHurComboBox;
		QCheckBox* qHurUseVectorCacheCheckBox;

		// GMAA Cluster Options
		QComboBox* clusterAlgorithmComboBox;
		QCheckBox* clusterUseBGClustering;
		QDoubleSpinBox* clusterJBThresholdSpinBox;
		QDoubleSpinBox* clusterJAOHThresholdSpinBox;

		// GMAA Options
		QSpinBox* gmaaRestartsSpinBox;
		QComboBox* gmaaTypeComboBox;
		QCheckBox* gmaaApproxInferenceCheckBox;
		QSpinBox* gmaaKSpinBox;

		// Max Plus Options
		QDoubleSpinBox* maxPlusDampingSpinBox;
		QSpinBox* maxPlusIterationsSpinBox;
		QSpinBox* maxPlusRestartsSpinBox;
		QComboBox* maxPlusUpdateComboBox;

		// CE Options
		QDoubleSpinBox* ceAlphaSpinBox;
		QSpinBox* ceRestartsSpinBox;
		QSpinBox* ceIterationsSpinBox;
		QSpinBox* ceSamplesSpinBox;
		QSpinBox* ceUpdateSamplesSpinBox;
		QCheckBox* ceNotStrictlyImprovingCheckBox;
};

///
/// \brief Allows the user to set the JESP Planner specific options
///
class SPJESPPlannerOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPJESPPlannerOptionsPage(QWidget* parent = 0);

		/// \return The ID of the problem selection page
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Components
		QRadioButton* jespExhaustiveRadioButton;
		QRadioButton* jespDynamicProgrammingRadioButton;
};

///
/// \brief Allows the user to set the DICEPS Planner specific options
///
class SPDICEPlannerOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPDICEPlannerOptionsPage(QWidget* parent = 0);

		/// \return The ID of the problem selection page
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// CE Options
		QDoubleSpinBox* ceAlphaSpinBox;
		QSpinBox* ceRestartsSpinBox;
		QSpinBox* ceIterationsSpinBox;
		QSpinBox* ceSamplesSpinBox;
		QCheckBox* ceNotStrictlyImprovingCheckBox;
		QSpinBox* ceUpdateSamplesSpinBox;
};

///
/// \brief Allows the user to configure the fire fighting options
/// if the class based problem is chosen
///
class SPFireFightingProblemOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPFireFightingProblemOptionsPage(QWidget* parent = 0);

		/// \return The ID of the general problem options page
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Components
		QSpinBox* ffProblemNumAgentsSpinBox;
		QDoubleSpinBox* ffProblemExtinguishProbSpinBox;
		QSpinBox* ffProblemFireLevelsSpinBox;
		QSpinBox* ffProblemNumHousesSpinBox;
};

///
/// \brief Allows the user to configure the aloha options
/// if the class based problem is chosen
///
class SPAlohaProblemOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		SPAlohaProblemOptionsPage(QWidget* parent = 0);

		/// \return The ID of the general problem options page
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();
		
		// Components
		QSpinBox* alohaProblemMaxBacklogSpinBox;
		QComboBox* alohaProblemIslandConfigurationComboBox;
		QComboBox* alohaProblemVariationComboBox;
};

#endif // STARTPLANWIZARD_H
