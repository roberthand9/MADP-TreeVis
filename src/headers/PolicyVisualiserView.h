#ifndef POLICYVISUALISERVIEW_H
#define POLICYVISUALISERVIEW_H

// TreeVis
#include "MainWindow.h"
#include "PlannerManager.h"
#include "TreeVisGraphicsView.h"

// Qt
#include <QWidget>
#include <QLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QFormLayout>

///
/// \brief The PolicyVisualiserView class is the full interface and
/// functionality for the Policy Visualiser view component of
/// the application. It creates the interface and provides
/// all further functionality in order to use the component
///
class PolicyVisualiserView : public QWidget {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the interface
		/// \param man The planner manager
		/// \param parent Parent widget
		///
		explicit PolicyVisualiserView(PlannerManager* man, QWidget* parent);
		~PolicyVisualiserView();

	signals:
		///
		/// \brief Signals to (MainWindow::)<AppendToInformationText>
		/// to add text to the info panel
		/// \param toAppend The string to append
		/// \param style The style for the string
		///
		void AppendToInformationText(const std::string& toAppend,
									 const MainWindow::textStyle& style = MainWindow::Normal);

	public slots:
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
		/// \brief Slot called when a JO is recieved from a
		/// JointObservationSelectionDialog
		/// \param joIndex The Index of the chosen
		/// joint observation
		///
		void RecieveJointObservation(int joIndex);

	private slots:
		///
		/// \brief Slot called when the Save Policy Visualiser
		/// Viewer Screen to File action is clicked on the main menu
		///
		void SaveGraphicsViewToFile();

		/// \brief Slot for Randomise Initial State button pressed.
		/// Randomises initial state based on probability
		/// distrubution in the problem
		void RandomiseInitialState();

		/// \brief Slot for Set Initial State button pressed.
		/// Sets initial state to chosen one, starts visualisation
		void SetInitialState();

		/// \brief Slot for Randomise Observations button pressed.
		/// Randomises observations basd on the probability defined in the
		/// problem then supplies this random observations
		void RandomiseObservations();

		/// Slot for Supply Observations button pressed
		void SupplyObservations();

		/// Slot for Restart Visualisation button pressed
		void RestartVisualisation();

		/// Slot for Individual Observation combo box updated
		void JointObservationUpdated();

		/// \brief Slot for View Joint Observations button pressed.
		/// Creates a JointObservationSelectionDialog allowing the user to
		/// choose a jont observation from the list
		void ViewJointObservations();

		/// \brief Slot for Match edge to probability checkbox pressed
		void ChangeEdgeColourProbability(bool checked);

	private:
		///
		/// \brief Creates the UI, sets up all widgets
		/// and interface components
		/// \param parent The parent widget
		///
		void SetupUI(QWidget* parent);

		///
		/// \brief Updates the current state label based
		/// on the information in currentVisualisation
		///
		void UpdateCurrentStateLabel();

		///
		/// \brief Updates the joint action label based
		/// on the information in currentVisualisation
		///
		void UpdateJointActionLabel();

		///
		/// \brief Updates the time step label based
		/// on the information in currentVisualisation
		///
		void UpdateTimeStepLabel();

		///
		/// \brief Updates the probability label based
		/// on the information in currentVisualisation
		///
		void UpdateProbabilityLabel();

		///
		/// \brief Sets the longest action name across all
		/// agents and their respective actions
		///
		void SetLongestActionName();

		///
		/// \brief Sets the height separation
		/// based on the longest observation name over all agents
		///
		void SetHeightSeparation();

		///
		/// \brief Visualisation completed. I.e.
		/// currentTimestep == horizon
		///
		void VisualisationFinished();

		///
		/// \brief Gets the observation probability based on the
		/// information in currentVisualisation
		/// \return
		///
		double GetCurrentJointObservationProbability();

		///
		/// \brief GetIndidividualObservationProbability
		/// \param agentIndex The agent index to get the probability for
		/// \param observationIndex The individual observation index to get the
		/// probability for
		/// \param jointActionIndex The joint action index taken
		/// \param stateIndex The state index
		/// \param succStateIndex The successor state index
		/// \return The probability of the agent recieving the individual
		/// observation based on the
		///
		double GetIndidividualObservationProbability(const Index &agentIndex,
													 const Index &observationIndex,
													 const Index &jointActionIndex,
													 const Index &stateIndex,
													 const Index &succStateIndex);


		///
		/// \brief Gets the possible combinations of observations given the
		/// observation numbers. The given agent index and observation index
		/// ensure the combinations returned contain this individual observation
		/// index for the given agent index. This method is used as a helper method
		/// tfor ghte GetIndividualObservationProbability().
		/// \param observationNumbers The number of possible observations for each agent
		/// \param agentIndex The agent index to set the value of the observation index as
		/// \param observationIndex The individual observation index to leave as the agent index
		/// \return Each combination of observations with the agent indexs' observation
		/// fixed as the observation index given
		///
		std::vector<std::vector<Index>> GetCombinations(const std::vector<size_t> &observationNumbers,
														const int &agentIndex,
														const int &observationIndex);


		///
		/// \brief Computes the individual observation probabilities based on
		/// the data in current visualisation
		/// \return A vector containing a probability for each agent of recieving
		/// their individual observation based on the data in currentVisualisation
		///
		std::vector<double> GetIndividualObservationProbabilities();


		///
		/// \brief Gets a tree widget containing all the Joint Observations
		/// based on the information in currentVisualisation in the format
		/// JOI | JO Name | Probability
		///
		/// \param parent The item to make the parent of the tree widget,
		/// if this is not given the returned pointer must be
		/// reparented with setParent() otherwise it will be parentless
		/// \return The constructed tree widget
		///
		QTreeWidget* GetJointObservationTreeWidget(QWidget* parent = 0);

		///
		/// \brief Adds the next set of nodes to the visualistation
		/// based on the information in currentVisualisation
		/// \param probability The probability of recieving the joint
		/// observation for this set of nodes
		///
		void AddNextNodes(const std::vector<double> &individualProbabilities);

		/// Height to pad from top of scene to nodes
		const int padding = 75;

		/// Edges of node padding
		const qreal betweenNodesPadding = 75;

		/// Holds data about the current visualisation
		struct CurrentVisualisationData {
			/// The time step the visualisation is on
			int timeStep;
			/// The longest action name
			int longestActionName;
			/// How much to separate the nodes on height
			int heightSeparation;
			/// The horizon of the visualisation
			int horizon;
			/// The total reward
			int totalReward;
			/// The total probability for the whole visualisation
			double totalProbability;
			/// The nodes in the current visualisation
			std::vector<Node*> nodes;
			/// Are the problem states named or not
			bool statesNamed;

			/// Current JA index for this time step
			Index currentJAIndex;
			/// \brief The current JO index specified by the selection
			/// of individual observations by the combo boxes
			Index currentJOIndex;
			/// The current joint observation history index
			Index johIndex;
			/// State the problem is in at this time step
			Index currentStateIndex;
			/// Successor state sampled
			Index successorStateIndex;
			/// The number of agents in the visualisation
			Index numAgents;

			// Default values
			CurrentVisualisationData() {
				timeStep = 1;
				horizon = -1;
				statesNamed = false;
				currentJAIndex = 0;
				currentJOIndex = 0;
				johIndex = 0;
				heightSeparation = 100;
				totalReward = 0;
				totalProbability = 0;
				successorStateIndex = -1;
				currentStateIndex = -1;
				numAgents = -1;
			}
		};

		/// Our current visualisation
		CurrentVisualisationData currentVisualisation;

		/// Combo boxes for each agents observation options
		std::vector<QComboBox*> observationSelectionComboBoxes;

		/// \brief The planner manager. Used to access
		/// the planning unit and policies
		PlannerManager* pManager;

		/// The graphics view
		TreeVisGraphicsView* graphicsView;

		// Initial state options
		QComboBox* initialStateSelectionComboBox;
		QGroupBox* initialStateOptionsGroupBox;

		// Buttons
		QPushButton* randomiseInitialStateButton;
		QPushButton* supplyObservationsButton;
		QPushButton* randomiseObservationsButton;
		QPushButton* setInitialStateButton;
		QPushButton* restartVisualisationButton;
		QPushButton* viewJointObservationsButton;
		QCheckBox* colourToObservationProbabilityCheckBox;
		QCheckBox* colourToObservationProbabilityCheckBoxEnd;

		// Labels
		QLabel* currentTimeStepLabel;
		QLabel* probabilityLabel;
		QLabel* currentJointActionLabel;
		QLabel* currentStateLabel;
		QLabel* waitingForPlanLabel;

		// Layout itemss
		QHBoxLayout* horizontalLayout;
		QVBoxLayout* visualisationControlLayout;
		QVBoxLayout* parentVerticalLayout;
		QGridLayout* infoWidgetLayout;
		QGridLayout* visualisationEndLayout;
		QGridLayout* initialStateGridLayout;
		QVBoxLayout* observationSelectionVerticalLayout;
		QScrollArea* scrollArea;
		QSplitter* splitter;

		// End screen widgets
		QTreeWidget* endProbabilityTreeWidget;
		QLabel* endRewardLabel;
		QLabel* endProbabilityLabel;

		// Wrapper widgets
		QWidget* observationSelectionScrollAreaContents;
		QWidget* controlInterfaceParent;
		QWidget* controlInfoWidget;
		QWidget* controlObservationSelectionWidget;
		QWidget* visualisationControlWidget;
		QWidget* visualisationEndWidget;
};

#endif // POLICYVISUALISERVIEW_H
