#ifndef PREVIOUSPLANWIZARD_H
#define PREVIOUSPLANWIZARD_H

// Qt
#include <QWizard>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QComboBox>

// MADP Files
#include "ProblemType.h"
#include "argumentHandlers.h"

///
/// \brief A wizard allowing the user to read in a previous
/// plan generated earlier by the toolbox. The user needs to specify
/// the problem the plan is for, the plan file outputted by the
/// toolbox using the ExportJointPolicy() method defined in
/// JointPolicyPureVector and the horizon the plan is for.
///
class PreviousPlanWizard : public QWizard {
	Q_OBJECT

	public:
		///
		/// \brief Constructor creates UI and sets up the pages
		/// for the wizard
		/// \param parent The parent of the wizard
		///
		PreviousPlanWizard(QWidget* parent = 0);
		~PreviousPlanWizard();

		/// The page IDs in the wizard
		enum pages {
			ProblemSelection,
			FFProblemOptions,
			AlohaProblemOptions,
			GeneralOptions
		};


	signals:
		///
		/// \brief Signal emitted when all options have
		/// been specified. Should be connected
		/// to by the creator to recieve
		/// \param policyFilePath The file path to the policy
		/// \param args The arguments of the problem the policy is for
		///
		void PreviousPlan(std::string policyFilePath,
						  ArgumentHandlers::Arguments args);

	private slots:
		/// \brief Finish button clicked on the wizard,
		/// emit the previous plan signal as finished
		void FinishButtonClicked();

	private:
		///
		/// \brief Sets aloha specific problem options for the cpp problem.
		/// \param args Reference to the arguments to set on
		///
		void SetAlohaProblemOptions(ArgumentHandlers::Arguments &args);

		///
		/// \brief Sets firefighting specific problem options for the cpp problem.
		/// \param args Reference to the arguments to set on
		///
		void SetFFProblemOptions(ArgumentHandlers::Arguments& args);
};

///
/// \brief Allows the user to select the problem either as
/// a class based one or specify that the problem should
/// be parsed from a file
///
class PPProblemSelectionPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		PPProblemSelectionPage(QWidget* parent = 0);

		///
		/// \return True if the page is complete and the
		/// wizard can proceed, false otherwize
		///
		bool isComplete() const override;

		///
		/// \return The next page id of the wizard based on
		/// if the user has chosen a class based
		/// problem or to parse
		///
		int nextId() const override;

	private slots:
		///
		/// \brief Slot called when either the class based problem
		/// or parse radio buttons are pressed
		///
		void ClassParseRadioBtnToggle();

		/// Browse button for problem path clicked
		void ProblemBrowseBtnClicked();

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		/// The problem file path if parsing
		QLineEdit* problemFilePath;

		// Layout items
		QGroupBox* cppClassProblemsGroupBox;
		QWidget* browseFieldWrapper;

		// Components
		QRadioButton* cppProblemRadioBtn;
		QRadioButton* parseProblemRadioBtn;
		QRadioButton* alohaProblemRadioBtn;
		QRadioButton* decTigerProblemRadioBtn;
		QRadioButton* ffProblemRadioBtn;
		QRadioButton* fffProblemRadioBtn;
		QRadioButton* ffgProblemRadioBtn;
		QPushButton* problemBrowseBtn;
};

///
/// \brief Allows the user to configure the fire fighting options
/// if the class based problem is chosen
///
class PPFireFightingProblemOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		PPFireFightingProblemOptionsPage(QWidget* parent = 0);

		/// \return The next page of the wizard
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Components
		QSpinBox* ffProblemNumAgentsSpinBox;
		QSpinBox* ffProblemFireLevelsSpinBox;
		QSpinBox* ffProblemNumHousesSpinBox;
};

///
/// \brief Allows the user to configure the aloha options
/// if the class based problem is chosen
///
class PPAlohaProblemOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		PPAlohaProblemOptionsPage(QWidget* parent = 0);

		/// \return The next page of of the wizard
		int nextId() const override;

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Components
		QSpinBox* alohaProblemMaxBacklogSpinBox;
		QComboBox* alohaProblemIslandConfigurationComboBox;
		QComboBox* alohaProblemVariationComboBox;
};

///
/// \brief Allows the user to set the general options
/// namely give the policy file and the horizon
/// the policy file is for
///
class PPGeneralOptionsPage : public QWizardPage {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the layout
		/// \param parent Parent to the page
		///
		PPGeneralOptionsPage(QWidget* parent = 0);

		///
		/// \return True if the page is complete and the
		/// wizard can proceed (can be finished), false otherwize
		///
		bool isComplete() const override;

		/// \return The next page if of the wizard
		int nextId() const override;

	private slots:
		/// Browse button for policy path clicked
		void PolicyBrowseBtnClicked();

	private:
		/// Register the pages fields on the wizard
		void RegisterFields();

		// Components
		QLineEdit* policyFilePath;
		QPushButton* policyBrowseBtn;
};

#endif // PREVIOUSPLANWIZARD_H
