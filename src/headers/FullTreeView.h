#ifndef FULLTREEVIEW_H
#define FULLTREEVIEW_H

// TreeVis
#include "MainWindow.h"
#include "PlannerManager.h"
#include "TreeVisGraphicsView.h"
#include "TreeVisGraphicsScene.h"
#include "Node.h"

// Qt
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLabel>

///
/// \brief The FullTreeView class is the full interface for
/// the full tree view component of the application.
/// Provides all of the functionality for displaying a policy
/// in a full tree representation.
///
class FullTreeView : public QWidget {
	Q_OBJECT

	public:
		///
		/// \brief Constructor
		/// \param parent The parent widget
		/// \param man The planner manager this widget will make use of
		///
		explicit FullTreeView(PlannerManager*, QWidget* parent = 0);
		~FullTreeView();

	signals:
		///
		/// \brief Signals to (MainWindow::)<AppendToInformationText>
		/// to add text to the info panel
		/// \param toAppend The string to append
		/// \param style The style for the string
		///
		void AppendToInformationText(const std::string& toAppend,
									 const MainWindow::textStyle &style = MainWindow::Normal);

	// Private slots can be connected to any signal from anywhere
	// but not called as normal functions outside of the class
	private slots:
		///
		/// \brief Slot called whenever a plan is starting
		/// or a previous plan is being loaded
		///
		void PlanStarting();

		///
		/// \brief Slot called whenever a plan has finished
		/// or a previous plan has finished loading
		///
		void PlanFinished();

		///
		/// \brief Slot called when the Save Full Tree
		/// Viewer Screen to File action is clicked on the main menu
		///
		void SaveGraphicsViewToFile();

		/// \brief Slot for increment policy button on interface
		void IncrementButtonClicked();

		/// \brief Slot for decrement policy button on interface
		void DecrementButtonClicked();

		/// \brief Slot for generate button on interface
		void GenerateButtonClicked();

	private:
		///
		/// \brief Creates the UI for the full tree view
		/// \param parent The parent widget
		///
		void SetupUI(QWidget* parent);

		///
		/// \brief Shows the full policy for the given
		/// agents index on the full tree viewer.
		/// \param agentIndex The zero indexed agent index
		///
		void ShowFullPolicyForAgent(const Index &agentIndex);

		/// \brief Updates the policy to show label on the interface
		void UpdateFullPolicyToShowLabel();

		///
		/// \brief Generates the full policy visualisation for the given agent index
		/// \param agentIndex The zero indexed agent index
		///
		void GenerateFTForAgent(const Index &agentIndex);

		///
		/// \brief Gets the longest width of the bounding rect of observation labels for the given agent.
		/// \param agentIndex The agents index to get the longest observation for
		/// \return The length of the longest observations bounding rect for the agent
		///
		int GetLongestObservationLengthForAgent(const Index &agentIndex);

		///
		/// \brief The agent index of the current policy being shown in the view.
		/// If this is -1 then no policy is being shown
		///
		int currentFullPolicyShown = -1;

		///
		/// \brief The full policy to show based on the increment/decrement buttons.
		/// This is displayed in the label between the buttons.
		///
		int fullPolicyToShow = -1;

		/// One scene per agent for every view
		std::vector<TreeVisGraphicsScene*> scenes;

		/// Which scenes have already been populated with the view
		std::vector<bool> fullPoliciesGenerated;

		/// The graphics view to display the policies in
		TreeVisGraphicsView* graphicsView;

		/// Gives access to the policies and planning unit
		PlannerManager* pManager;

		/// The space between nodes on the bottom level
		const int paddingBetweenNodes = 20;

		// UI Items
		// Push Buttons
		QPushButton* incrementButton;
		QPushButton* decrementButton;
		QPushButton* generateButton;

		// Labels
		QLabel* infoLabel;
		QLabel* policyGenLabel;
		QLabel* waitingForPlanLabel;

		// Other
		QWidget* buttonLabelContainer;
		QHBoxLayout* infoHorizontalLayout;
		QVBoxLayout* verticalLayout;
};

#endif // FULLTREEVIEW_H
