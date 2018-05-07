#include "PolicyVisualiserView.h"

// TreeVis files
#include "NumSortTreeWidgetItem.h"
#include "JointObservationSelectionDialog.h"


PolicyVisualiserView::PolicyVisualiserView(PlannerManager* man, QWidget* parent) : QWidget(parent) {
	pManager = man;
	SetupUI(parent);
}


PolicyVisualiserView::~PolicyVisualiserView() {
	//std::cout << "~PolicyVisualiserView()" << std::endl;
}


void PolicyVisualiserView::SetupUI(QWidget* parent) {
	// Create horizontal layout, set sizes
	horizontalLayout = new QHBoxLayout(parent);

	// Create splitter to go down between the graphics view and the table view
	splitter = new QSplitter(parent);

	// Create graphics view, add it to the splitter
	graphicsView = new TreeVisGraphicsView(pManager, splitter);
	graphicsView->setAlignment(Qt::AlignTop);
	splitter->addWidget(graphicsView);

	// Create observations widgets
	controlInterfaceParent = new QWidget(splitter);

	// Widget and layout for the visualisation controller
	visualisationControlWidget = new QWidget(controlInterfaceParent);
	visualisationControlLayout = new QVBoxLayout(visualisationControlWidget);

	// Sub items, the info widget and observation combo boxes widget
	controlInfoWidget = new QWidget(visualisationControlWidget);
	controlObservationSelectionWidget = new QWidget(visualisationControlWidget);

	// Vertical layout for the control widget (where observations are chosen via combo boxes)
	observationSelectionVerticalLayout = new QVBoxLayout(controlObservationSelectionWidget);

	// Create a scroll area in the event that there are many agents (avoids squashing)
	scrollArea = new QScrollArea(controlObservationSelectionWidget);
	observationSelectionScrollAreaContents = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);

	// Add the widget to the layout
	observationSelectionVerticalLayout->addWidget(scrollArea);

	// Grid layout for controller buttons and info widget
	infoWidgetLayout = new QGridLayout(controlInfoWidget);

	// Create buttons to use for visualisation and connect
	supplyObservationsButton = new QPushButton("Supply Observations", controlInfoWidget);
	randomiseObservationsButton = new QPushButton("Supply Rand Obvs", controlInfoWidget);
	viewJointObservationsButton = new QPushButton("View all Joint Observations", controlInfoWidget);

	connect(supplyObservationsButton, &QPushButton::clicked, this, &PolicyVisualiserView::SupplyObservations);
	connect(randomiseObservationsButton, &QPushButton::clicked, this, &PolicyVisualiserView::RandomiseObservations);
	connect(viewJointObservationsButton, &QPushButton::clicked, this, &PolicyVisualiserView::ViewJointObservations);

	// Create information labels and their corresponding output labels, setup in layout
	infoWidgetLayout->addWidget(new QLabel("Current Timestep:", controlInfoWidget), 0, 0);
	currentTimeStepLabel = new QLabel("----", controlInfoWidget);
	infoWidgetLayout->addWidget(currentTimeStepLabel, 0, 1);

	infoWidgetLayout->addWidget(new QLabel("Current JA:", controlInfoWidget), 1, 0);
	currentJointActionLabel = new QLabel("----", controlInfoWidget);
	infoWidgetLayout->addWidget(currentJointActionLabel, 1, 1);

	infoWidgetLayout->addWidget(new QLabel("Current State:", controlInfoWidget), 2, 0);
	currentStateLabel = new QLabel("----", controlInfoWidget);
	infoWidgetLayout->addWidget(currentStateLabel, 2, 1);

	infoWidgetLayout->addWidget(new QLabel("Probability:", controlInfoWidget), 3, 0);
	probabilityLabel = new QLabel("----", controlInfoWidget);
	infoWidgetLayout->addWidget(probabilityLabel, 3, 1);

	// Colour to probability check box
	infoWidgetLayout->addWidget(new QLabel("Colour to Prob:", controlInfoWidget), 4, 0);
	colourToObservationProbabilityCheckBox = new QCheckBox(controlInfoWidget);
	infoWidgetLayout->addWidget(colourToObservationProbabilityCheckBox, 4, 1);
	// Connect check box
	connect(colourToObservationProbabilityCheckBox, &QCheckBox::toggled,
			this, &PolicyVisualiserView::ChangeEdgeColourProbability);

	infoWidgetLayout->addWidget(supplyObservationsButton, 5, 0);
	infoWidgetLayout->addWidget(randomiseObservationsButton, 5, 1);


	// Create inital state options interface
	initialStateOptionsGroupBox = new QGroupBox("Initial State Options", controlInterfaceParent);
	initialStateGridLayout = new QGridLayout(initialStateOptionsGroupBox);
	initialStateSelectionComboBox = new QComboBox(controlInfoWidget);

	// Initial state buttons and connect
	randomiseInitialStateButton = new QPushButton("Randomise Initial State", controlInfoWidget);
	setInitialStateButton = new QPushButton("Set Initial State");

	connect(randomiseInitialStateButton, &QPushButton::clicked,
			this, &PolicyVisualiserView::RandomiseInitialState);

	connect(setInitialStateButton, &QPushButton::clicked,
			this, &PolicyVisualiserView::SetInitialState);

	// Setup initial state layout
	initialStateGridLayout->addWidget(initialStateSelectionComboBox, 0, 0, 1, 2);
	initialStateGridLayout->addWidget(randomiseInitialStateButton, 1, 0, 1, 1);
	initialStateGridLayout->addWidget(setInitialStateButton, 1, 1, 1, 1);


	// Create end of visualisation interface
	visualisationEndWidget = new QWidget(controlInterfaceParent);
	visualisationEndLayout = new QGridLayout(visualisationEndWidget);

	// Labels
	visualisationEndLayout->addWidget(new QLabel("Overall Probability:", visualisationEndWidget), 1, 0);
	endProbabilityLabel = new QLabel("----", visualisationEndWidget);
	visualisationEndLayout->addWidget(endProbabilityLabel, 1, 1);

	visualisationEndLayout->addWidget(new QLabel("Reward for Visualisation:", visualisationEndWidget), 2, 0);
	endRewardLabel = new QLabel("----", visualisationEndWidget);
	visualisationEndLayout->addWidget(endRewardLabel, 2, 1);

	visualisationEndLayout->addWidget(new QLabel("Colour to Prob:", visualisationEndWidget), 3, 0);
	colourToObservationProbabilityCheckBoxEnd = new QCheckBox(visualisationEndWidget);

	// Connect check box
	connect(colourToObservationProbabilityCheckBoxEnd, &QCheckBox::toggled,
			this, &PolicyVisualiserView::ChangeEdgeColourProbability);

	visualisationEndLayout->addWidget(colourToObservationProbabilityCheckBoxEnd, 3, 1);


	// Vertical layout to hold the widgets
	parentVerticalLayout = new QVBoxLayout(controlInterfaceParent);
	parentVerticalLayout->setAlignment(Qt::AlignCenter);

	// Add the control widgets to their layout
	visualisationControlLayout->addWidget(controlObservationSelectionWidget);
	visualisationControlLayout->addWidget(viewJointObservationsButton);
	visualisationControlLayout->addWidget(controlInfoWidget);

	// Remove default padding
	controlInfoWidget->setContentsMargins(-9, -9, -9, -9);
	visualisationControlWidget->setContentsMargins(-9, -9, -9, -9);
	controlObservationSelectionWidget->setContentsMargins(-9, -9, -9, -9);
	visualisationEndWidget->setContentsMargins(-9, -9, -9, -9);

	// Hide the control widget and end of visualisation widget
	visualisationControlWidget->hide();
	initialStateOptionsGroupBox->hide();
	visualisationEndWidget->hide();

	// Basic size pol
	QSizePolicy full(QSizePolicy::Expanding, QSizePolicy::Minimum);
	visualisationControlWidget->setSizePolicy(full);
	initialStateOptionsGroupBox->setSizePolicy(full);
	visualisationEndWidget->setSizePolicy(full);

	// Add widgets to the parent layout
	parentVerticalLayout->addWidget(visualisationControlWidget);
	parentVerticalLayout->addWidget(initialStateOptionsGroupBox);
	parentVerticalLayout->addWidget(visualisationEndWidget);

	// Restart button
	restartVisualisationButton = new QPushButton("Restart Visualisation", visualisationControlWidget);
	parentVerticalLayout->addWidget(restartVisualisationButton);

	connect(restartVisualisationButton, &QPushButton::clicked,
			this, &PolicyVisualiserView::RestartVisualisation);
	restartVisualisationButton->hide();

	// Create wait label
	waitingForPlanLabel = new QLabel("Waiting for Plan", controlInterfaceParent);
	parentVerticalLayout->addWidget(waitingForPlanLabel);

	// Initally null as we need to wait for an end of a plan
	endProbabilityTreeWidget = nullptr;

	// Add the control parent widget to the splitter
	splitter->addWidget(controlInterfaceParent);

	// Setup splitter
	splitter->setOrientation(Qt::Horizontal);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 0);

	// Add the splitter to the layout
	horizontalLayout->addWidget(splitter);
	srand(time(NULL));
}


void PolicyVisualiserView::ChangeEdgeColourProbability(bool checked) {

	// Get all items in the scene
	QList<QGraphicsItem*> items = graphicsView->scene()->items();

	for(QGraphicsItem* item : items) {
		Edge* edge = qgraphicsitem_cast<Edge*>(item);

		// If we got an edge
		if(edge) {
			edge->ChangeColourToMatchProbability(!checked);
		}
	}
}


double PolicyVisualiserView::GetIndidividualObservationProbability(
											const Index &agentIndex,
											const Index &observationIndex,
											const Index &jointActionIndex,
											const Index &stateIndex,
											const Index &succStateIndex) {
	double toReturn = 0.0;

	// Get num observations vector
	std::vector<size_t> numObservations =
			pManager->GetPlanningUnit()->GetNrObservations();

	// Get possible combinations
	std::vector<std::vector<Index>> combinations = GetCombinations(numObservations, agentIndex, observationIndex);

	for(std::vector<Index> vect : combinations) {
		// Get joint observation index for this vector
		Index jointObservationIndex = pManager->GetPlanningUnit()->IndividualToJointObservationIndices(vect);

		// Add probability
		toReturn += pManager->GetPlanningUnit()->GetProblem()->
				GetObservationProbability(stateIndex, jointActionIndex, succStateIndex, jointObservationIndex);
	}

	return toReturn;
}


std::vector<std::vector<Index>> PolicyVisualiserView::GetCombinations(
									const std::vector<size_t> &observationNumbers,
									const int &agentIndex,
									const int &observationIndex) {
	int combinations = 1;

	// Calculate combinations
	for(unsigned int i=0; i<observationNumbers.size(); ++i) {

		// Skip agents index
		if(i != agentIndex) {
			combinations *= observationNumbers[i];
		}
	}

	// Combinations to return
	std::vector<std::vector<Index>> toReturn(combinations);

	// Start off with 0s
	std::vector<Index> current(observationNumbers.size(), 0);

	// Set the agent index we leave unchanged to the observation index
	current[agentIndex] = observationIndex;
	toReturn[0] = current;

	// Current index of incrementation is last
	int currentIndex = observationNumbers.size()-1;

	for(int j=1; j<combinations; ++j) {

		// If we can add one, do it
		if((current[currentIndex] < observationNumbers[currentIndex]-1)
		   && currentIndex != agentIndex) {

			current[currentIndex]++;

			// Add vector to set
			toReturn[j] = current;
		} else {

			// While we don't have space to the 'left' or we're skipping due to agent index, decrement
			while((observationNumbers[currentIndex]-1 == current[currentIndex])
					|| (currentIndex == agentIndex)) {
				--currentIndex;
			}

			// Reset all to right back to 0
			for(int i=currentIndex+1; i<observationNumbers.size(); ++i) {
				if(i != agentIndex) {
					current[i] = 0;
				}
			}

			// Increment current and set back to end
			current[currentIndex]++;
			currentIndex = observationNumbers.size()-1;

			// Add vector to set
			toReturn[j] = current;
		}
	}

	return toReturn;
}



std::vector<double> PolicyVisualiserView::GetIndividualObservationProbabilities() {
	// Create vector of size
	std::vector<double> individualObservationProbabilities(currentVisualisation.numAgents);

	// Get values based on data in currentVisualisation
	for(Index i=0; i<currentVisualisation.numAgents; ++i) {
		individualObservationProbabilities[i] = GetIndidividualObservationProbability(
					i,
					observationSelectionComboBoxes[i]->currentIndex(),
					currentVisualisation.currentJAIndex,
					currentVisualisation.currentStateIndex,
					currentVisualisation.successorStateIndex);
	}

	return individualObservationProbabilities;
}


void PolicyVisualiserView::RecieveJointObservation(int joIndex) {
	// Get corresponding individual observation indexes
	std::vector<Index> individualObvservationIndexes =
			pManager->GetPlanningUnit()->JointToIndividualObservationIndices(joIndex);

	// Set the indexs
	for(unsigned int i=0; i<individualObvservationIndexes.size(); ++i) {
		observationSelectionComboBoxes[i]->setCurrentIndex(individualObvservationIndexes[i]);
	}
}


void PolicyVisualiserView::ViewJointObservations() {
	// Create dialog passing a new tree widget
	JointObservationSelectionDialog* dialog =
			new JointObservationSelectionDialog(GetJointObservationTreeWidget(), this);

	// Don't want multiple instances, user can just cancel
	dialog->setModal(true);
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	// Connect so we can recieve and show
	connect(dialog, &JointObservationSelectionDialog::SendJointObservation,
			this, &PolicyVisualiserView::RecieveJointObservation);

	dialog->show();
}


QTreeWidget* PolicyVisualiserView::GetJointObservationTreeWidget(QWidget* parent) {
	QTreeWidget* treeWidget = new QTreeWidget(parent);

	// Set headers
	QTreeWidgetItem* treeWidgetHeader = treeWidget->headerItem();
	treeWidgetHeader->setText(0, "Index");
	treeWidgetHeader->setText(1, "Name");
	treeWidgetHeader->setText(2, "Probability");

	// Configure tree widget
	treeWidget->setSortingEnabled(true);
	treeWidget->setRootIsDecorated(false);
	treeWidget->sortByColumn(2, Qt::DescendingOrder);
	treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	PlanningUnitDecPOMDPDiscrete* pUnit = pManager->GetPlanningUnit();

	// Fill the tree widget
	for(unsigned int i=0; i<pUnit->GetNrJointObservations(); ++i) {

		double obvsProbability = pUnit->GetProblem()->GetObservationProbability(
					currentVisualisation.currentStateIndex,
					currentVisualisation.currentJAIndex,
					currentVisualisation.successorStateIndex,
					i)*100;

		// Only add if possible, otherwise skip
		if(obvsProbability > 0) {
			QStringList list;

			// JO Index | Name | Probability
			list << QString::number(i);
			list << QString::fromStdString(pUnit->GetJointObservation(i)->SoftPrint());
			list << QString::number(obvsProbability) + "%";

			// Create item and add
			NumSortTreeWidgetItem* item = new NumSortTreeWidgetItem(treeWidget, list);
			treeWidget->addTopLevelItem(item);
		}
	}

	// Resize
	treeWidget->setCurrentItem(treeWidget->topLevelItem(0));
	treeWidget->resizeColumnToContents(0);
	treeWidget->resizeColumnToContents(1);
	treeWidget->resizeColumnToContents(2);

	return treeWidget;
}


void PolicyVisualiserView::RandomiseInitialState() {

	// Sample random initial state
	Index stateIndex = pManager->GetPlanningUnit()->GetProblem()->SampleInitialState();

	// Iterate over the combo box items
	for(int i=0; i<initialStateSelectionComboBox->count(); ++i) {

		// If the state has been found in the combo box list, set it to this
		if(initialStateSelectionComboBox->itemData(i).toUInt() == stateIndex) {
			initialStateSelectionComboBox->setCurrentIndex(i);
			return;
		}
	}
}


void PolicyVisualiserView::RestartVisualisation() {
	AppendToInformationText("Restarting Visualisation");
	// Reset by simulating plan start + finish
	PlanStarting();
	PlanFinished();
}


void PolicyVisualiserView::SaveGraphicsViewToFile() {
	// Save to file on graphics view
	graphicsView->SaveToFile();
}


void PolicyVisualiserView::SupplyObservations() {
	// Get planning unit
	PlanningUnitDecPOMDPDiscrete* pUnit = pManager->GetPlanningUnit();

	// Next time step
	++currentVisualisation.timeStep;

	// Get joint observation probability and individual observationb probabilities,
	// do this here before sampling the successor JOHI
	double probability = GetCurrentJointObservationProbability();
	std::vector<double> individualProbabilities = GetIndividualObservationProbabilities();

	// Set probability based on first time step or not
	if(currentVisualisation.timeStep == 2) {
		currentVisualisation.totalProbability = probability;
	} else {
		currentVisualisation.totalProbability *= probability;
	}

	// Get successor johi based on the previous johIndex and the one given by the user
	currentVisualisation.johIndex = pUnit->GetSuccessorJOHI(currentVisualisation.johIndex,
															currentVisualisation.currentJOIndex);

	// Get next joint action
	currentVisualisation.currentJAIndex = pManager->GetJointActionIndex(currentVisualisation.johIndex);

	// Add the next set of nodes
	AddNextNodes(individualProbabilities);

	// Set old successor state to current state and sample new successor state
	currentVisualisation.currentStateIndex = currentVisualisation.successorStateIndex;
	currentVisualisation.successorStateIndex = pUnit->GetProblem()->SampleSuccessorState(
				currentVisualisation.currentStateIndex, currentVisualisation.currentJAIndex);

	// Update labels
	UpdateTimeStepLabel();
	UpdateJointActionLabel();
	UpdateCurrentStateLabel();
	UpdateProbabilityLabel();

	// Update reward
	currentVisualisation.totalReward += pUnit->GetReward(currentVisualisation.currentStateIndex,
													currentVisualisation.currentJAIndex);

	// If TS == Horizon, finished
	if(currentVisualisation.timeStep == currentVisualisation.horizon) {
		VisualisationFinished();
	}

	JointObservationUpdated();
}


void PolicyVisualiserView::VisualisationFinished() {
	// Hide visualisation controls, show the end widget
	visualisationControlWidget->hide();

	// Set this colour to, to the current value
	colourToObservationProbabilityCheckBoxEnd->setChecked(
				colourToObservationProbabilityCheckBox->isChecked());

	AppendToInformationText("Visualisation Finished");

	endRewardLabel->setText(QString::number(currentVisualisation.totalReward));
	endProbabilityLabel->setText(QString::number(currentVisualisation.totalProbability*100) + "%");	

	// If previously created, mark for deletion
	if(endProbabilityTreeWidget) {
		endProbabilityTreeWidget->deleteLater();
	}

	// Get new tree widget & add to layout
	endProbabilityTreeWidget = GetJointObservationTreeWidget(visualisationEndWidget);
	visualisationEndLayout->addWidget(endProbabilityTreeWidget, 0, 0, 1, 2);

	// Show the pane
	visualisationEndWidget->show();
}


void PolicyVisualiserView::SetInitialState() {	
	PlanningUnitDecPOMDPDiscrete* pUnit = pManager->GetPlanningUnit();
	restartVisualisationButton->show();

	// Extract state index user chose as initial state and set on class
	currentVisualisation.currentStateIndex = initialStateSelectionComboBox->currentData().toInt();

	// Get successor state based on JA and state chosen
	currentVisualisation.successorStateIndex = pUnit->GetProblem()->SampleSuccessorState(
				currentVisualisation.currentStateIndex , currentVisualisation.currentJAIndex);

	// Show the controller, hide the initial state selection
	visualisationControlWidget->show();
	initialStateOptionsGroupBox->hide();

	// Set the joint action for the first time step
	currentVisualisation.currentJAIndex = pManager->GetJointActionIndex(currentVisualisation.johIndex);


	// Update reward
	currentVisualisation.totalReward += pUnit->GetReward(currentVisualisation.currentStateIndex,
													currentVisualisation.currentJAIndex);

	// Update the labels
	UpdateTimeStepLabel();
	UpdateJointActionLabel();
	UpdateCurrentStateLabel();
	UpdateProbabilityLabel();

	// Add the first set of action nodes
	AddNextNodes(GetIndividualObservationProbabilities());

	// If only planned for one time step, nothing to do
	if(currentVisualisation.horizon == 1) {
		VisualisationFinished();
	}

	// Disable button if needed
	JointObservationUpdated();
}


void PolicyVisualiserView::UpdateTimeStepLabel() {
	currentTimeStepLabel->setText(QString::number(currentVisualisation.timeStep) + "/" + QString::number(currentVisualisation.horizon));
}


void PolicyVisualiserView::UpdateJointActionLabel() {
	currentJointActionLabel->setText(QString::fromStdString(pManager->GetPlanningUnit()->GetJointAction(currentVisualisation.currentJAIndex)->SoftPrint()));
}


void PolicyVisualiserView::AddNextNodes(const std::vector<double> &individualProbabilities) {
	PlanningUnitDecPOMDPDiscrete* pUnit = pManager->GetPlanningUnit();

	// Get individual actions based on the JA index
	const std::vector<Index> individualActions =
			pUnit->JointToIndividualActionIndices(currentVisualisation.currentJAIndex);

	const std::vector<Index> individualObvsHistoryIndexes =
			pUnit->JointToIndividualObservationHistoryIndices(currentVisualisation.johIndex);

	// Create the fresh set of nodes
	for(unsigned int i=0; i<currentVisualisation.numAgents; ++i) {
		Node* node = new Node(pUnit->GetAction(i, individualActions[i])->GetName(), individualObvsHistoryIndexes[i], i);

		qreal xPos = (i*currentVisualisation.longestActionName) + (i*betweenNodesPadding);
		qreal yPos = (currentVisualisation.timeStep-1)*currentVisualisation.heightSeparation;

		// Set the position, adjust to the centre of the node on the x axis
		node->setPos(xPos, yPos);

		//node->pos().rx() -= node->boundingRect().width()/2;
		graphicsView->scene()->addItem(node);
		currentVisualisation.nodes[((currentVisualisation.timeStep-1)*currentVisualisation.numAgents)+i] = node;
	}

	// If edges need adding (i.e. not the first time step)
	if(currentVisualisation.timeStep > 1) {
		for(unsigned int i=0; i<currentVisualisation.numAgents; ++i) {
			// Child nodes are the ones that were just added
			Node* parent = currentVisualisation.nodes[((currentVisualisation.timeStep-2)*currentVisualisation.numAgents)+i];
			Node* child = currentVisualisation.nodes[((currentVisualisation.timeStep-1)*currentVisualisation.numAgents)+i];

			// Create edge with observation name, add the edge
			Edge* edge = new Edge(parent,
								  child,
								  pUnit->GetObservation(i, observationSelectionComboBoxes[i]->currentIndex())->GetName(),
								  individualProbabilities[i]);

			edge->ChangeColourToMatchProbability(!colourToObservationProbabilityCheckBox->isChecked());
			graphicsView->scene()->addItem(edge);
		}
	}
}


void PolicyVisualiserView::SetHeightSeparation() {
	int longest = 0;

	// Get currently used font on edges
	QFontMetricsF fontMetrics = QFontMetricsF(Edge::GetFont());

	// Iterate over all agents, all observations
	for(unsigned int i=0; i<pManager->GetPlanningUnit()->GetNrAgents(); ++i) {
		for(unsigned int j=0; j<pManager->GetPlanningUnit()->GetNrObservations(i); ++j) {

			// Max of current longest and new one
			longest = std::max(longest, (int) (fontMetrics.boundingRect(
				QString::fromStdString(pManager->GetPlanningUnit()->GetObservation(i, j)->GetName())).width()));
		}
	}

	// Add approximate height for a node + a constant value of 30. Ensure at least separated by 120
	currentVisualisation.heightSeparation = std::max(120, (int) (longest + fontMetrics.boundingRect("A").height() + 50));
}


void PolicyVisualiserView::RandomiseObservations() {
	// Sample observation
	Index sampledObservation = pManager->GetPlanningUnit()->GetProblem()->
			SampleJointObservation(
				currentVisualisation.currentStateIndex,
				currentVisualisation.currentJAIndex,
				currentVisualisation.successorStateIndex);

	// Get individual indexes
	std::vector<Index> individualObvs = pManager->GetPlanningUnit()->
			JointToIndividualObservationIndices(sampledObservation);

	// Set each box
	for(Index i=0; i<observationSelectionComboBoxes.size(); ++i) {
		observationSelectionComboBoxes[i]->setCurrentIndex(individualObvs[i]);
	}

	// Supply
	SupplyObservations();
}


void PolicyVisualiserView::SetLongestActionName() {
	int longest = 0;

	// Get current font
	QFontMetricsF fontMetrics = QFontMetricsF(Node::GetFont());

	for(unsigned int i=0; i<pManager->GetPlanningUnit()->GetNrAgents(); ++i) {
		for(unsigned int j=0; j<pManager->GetPlanningUnit()->GetNrActions(i); ++j) {

			int length = fontMetrics.boundingRect(QString::fromStdString(
										 pManager->GetPlanningUnit()->GetAction(i, j)->GetName())).width();

			// If a longer width
			if(length > longest) {
				longest = length;
			}
		}
	}

	currentVisualisation.longestActionName = longest;
}


void PolicyVisualiserView::PlanStarting() {
	// Clear anything off the scene & clear vector
	observationSelectionComboBoxes.clear();

	// Reset graphics view (new scene) & add padding so items will align to this
	graphicsView->Reset();
	graphicsView->scene()->addRect(0, -padding, 0, 0);

	// Ensure all other widgets are hidden
	visualisationControlWidget->hide();
	visualisationEndWidget->hide();
	restartVisualisationButton->hide();
	waitingForPlanLabel->show();
	restartVisualisationButton->hide();
	initialStateOptionsGroupBox->hide();

	// Fix alignment
	parentVerticalLayout->setAlignment(Qt::AlignCenter);

	// Clear combo box options
	initialStateSelectionComboBox->clear();

	// Delete the old observation selection widget if
	// it has been created before
	if(observationSelectionScrollAreaContents) {
	   observationSelectionScrollAreaContents->deleteLater();

	   // Qt has own pointer to delete this
	   observationSelectionScrollAreaContents = nullptr;
	}
}


void PolicyVisualiserView::PlanFinished() {
	PlanningUnitDecPOMDPDiscrete* pUnit = pManager->GetPlanningUnit();

	// New instance of data for default values
	currentVisualisation = CurrentVisualisationData();
	SetLongestActionName();
	SetHeightSeparation();

	// Set options specifc to this
	currentVisualisation.horizon = pUnit->GetHorizon();
	currentVisualisation.statesNamed = pUnit->GetState(0)->GetName().compare("undefined") != 0;
	currentVisualisation.numAgents = pUnit->GetNrAgents();
	currentVisualisation.nodes = std::vector<Node*>(currentVisualisation.horizon*currentVisualisation.numAgents);

	// Fix alignment
	parentVerticalLayout->setAlignment(Qt::AlignCenter|Qt::AlignTop);
	waitingForPlanLabel->hide();
	initialStateOptionsGroupBox->show();

	// Fill initial state selection combo box
	for(unsigned int i=0; i<pUnit->GetNrStates(); ++i) {
		double prob = pUnit->GetInitialStateProbability(i);

		// If the state is choosable as the initial state
		if(prob > 0) {

			// Construct name based on if the states are named
			QString name("");
			name += currentVisualisation.statesNamed ? QString::fromStdString(pUnit->GetState(i)->GetName()) : "State " + QString::number(i);
			name +=  " - " + QString::number(prob*100) + "%";

			// Add it to the combo box, use the QVariant to keep track of the actual state index
			initialStateSelectionComboBox->addItem(name, QVariant((int) i));
		}
	}

	// Create fresh scroll area
	observationSelectionScrollAreaContents = new QWidget(controlInfoWidget);

	// Create form layout for the Labels & Combo Boxes
	QFormLayout* formLayout = new QFormLayout(observationSelectionScrollAreaContents);
	scrollArea->setWidget(observationSelectionScrollAreaContents);

	// Create vector big enough for combo boxes for each agent
	size_t numAgents = pManager->GetPlanningUnit()->GetNrAgents();
	observationSelectionComboBoxes = std::vector<QComboBox*>(numAgents);

	// Create a combo box for each agent
	for(unsigned int i=0; i<numAgents; ++i) {
		observationSelectionComboBoxes[i] = new QComboBox(controlObservationSelectionWidget);

		// Fill the combo box with the observation options for the agent
		for(unsigned int j=0; j<pManager->GetPlanningUnit()->GetNrObservations(i); ++j) {
			observationSelectionComboBoxes[i]->insertItem(j, QString::fromStdString(pManager->GetPlanningUnit()->GetObservation(i, j)->GetName()));
		}

		formLayout->addRow("Agent " + QString::number(i+1), observationSelectionComboBoxes[i]);
	}

	// Connect when a combo box is changed it updates the probability label
	// do this here as if we connect in above loop the program will crash (not all initalised)
	for(unsigned int i=0; i<numAgents; ++i) {

		// Static cast to overload - http://wiki.qt.io/New_Signal_Slot_Syntax#Overload
		connect(observationSelectionComboBoxes[i], static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
				this, &PolicyVisualiserView::JointObservationUpdated);
	}
}


void PolicyVisualiserView::UpdateCurrentStateLabel() {
	QString newLabel;

	// If states named, set it to the name, otherwise the index
	if(currentVisualisation.statesNamed) {
		newLabel = QString::fromStdString(pManager->GetPlanningUnit()->GetState(currentVisualisation.currentStateIndex)->GetName());
	} else {
		newLabel = "State " + QString::number(currentVisualisation.currentStateIndex);
	}

	// Set the label
	currentStateLabel->setText(newLabel);
}


void PolicyVisualiserView::UpdateProbabilityLabel() {
	double probability = 100 * GetCurrentJointObservationProbability();
	probabilityLabel->setText(QString::number(probability) + "%");
}


void PolicyVisualiserView::JointObservationUpdated() {
	std::vector<Index> individualObservations(currentVisualisation.numAgents);

	// Fill observations array based on users options
	for(unsigned int i=0; i<currentVisualisation.numAgents; ++i) {
		individualObservations[i] = observationSelectionComboBoxes[i]->currentIndex();
	}

	currentVisualisation.currentJOIndex =
			pManager->GetPlanningUnit()->IndividualToJointObservationIndices(individualObservations);

	// Update the label with the new one
	UpdateProbabilityLabel();

	// Enable if possible to supply the observation
	supplyObservationsButton->setEnabled(GetCurrentJointObservationProbability() > 0);
}


double PolicyVisualiserView::GetCurrentJointObservationProbability() {
	// Get the probability based on the data in currentVisualisation
	return pManager->GetPlanningUnit()->GetProblem()->
				GetObservationProbability(
					currentVisualisation.currentStateIndex,
					currentVisualisation.currentJAIndex,
					currentVisualisation.successorStateIndex,
					currentVisualisation.currentJOIndex);
}
