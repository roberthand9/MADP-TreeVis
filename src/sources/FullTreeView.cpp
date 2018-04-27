#include "FullTreeView.h"

FullTreeView::FullTreeView(PlannerManager* man, QWidget* parent) : QWidget(parent) {
	pManager = man;
	SetupUI(parent);
}


FullTreeView::~FullTreeView() {
	// std::cout << "~FullTreeView()" << std::endl;
}

void FullTreeView::SetupUI(QWidget* parent) {
	// Create Graphics View
	graphicsView = new TreeVisGraphicsView(pManager, this);

	// Set size policy
	QSizePolicy graphicsViewSizePol(QSizePolicy::Expanding, QSizePolicy::Expanding);
	graphicsView->setSizePolicy(graphicsViewSizePol);

	// Label container
	buttonLabelContainer = new QWidget(this);
	QSizePolicy labelContainerSizePol(QSizePolicy::Minimum, QSizePolicy::Preferred);
	labelContainerSizePol.setHeightForWidth(buttonLabelContainer->sizePolicy().hasHeightForWidth());
	buttonLabelContainer->setSizePolicy(labelContainerSizePol);
	buttonLabelContainer->setMinimumSize(QSize(0, 10));

	//
	infoHorizontalLayout = new QHBoxLayout(buttonLabelContainer);
	infoHorizontalLayout->setContentsMargins(2, 2, 2, 2);

	// Create decrement button and increment/decrement buttons size policy, add it to the information horizontal layout
	decrementButton = new QPushButton(buttonLabelContainer);
	decrementButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	decrementButton->setMaximumSize(QSize(30, 30));
	infoHorizontalLayout->addWidget(decrementButton);

	// Create new label (this if for between the increment and decrement buttons), add it to the information layout
	policyGenLabel = new QLabel(buttonLabelContainer);
	infoHorizontalLayout->addWidget(policyGenLabel);

	// Create increment button, add it to the information horizontal layout, set size policy to inc/dec size policy
	incrementButton = new QPushButton(buttonLabelContainer);
	incrementButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	incrementButton->setMaximumSize(QSize(30, 30));
	infoHorizontalLayout->addWidget(incrementButton);

	// Create generate button, add it to the information layout
	generateButton = new QPushButton(buttonLabelContainer);
	infoHorizontalLayout->addWidget(generateButton);

	// Hide buttons initially
	buttonLabelContainer->hide();

	// Wait for label
	waitingForPlanLabel = new QLabel("Waiting for Plan", this);

	// Create new label (seen on the far right of the information layout in the ftv screen)
	// create and add the size policy. Add the label to the information pane
	infoLabel = new QLabel(buttonLabelContainer);
	QSizePolicy infoLabelSizePol(QSizePolicy::Preferred, QSizePolicy::Preferred);
	infoLabelSizePol.setHorizontalStretch(5);
	infoLabelSizePol.setHeightForWidth(infoLabel->sizePolicy().hasHeightForWidth());
	infoLabel->setSizePolicy(infoLabelSizePol);
	infoLabel->setTextFormat(Qt::RichText);
	infoLabel->setScaledContents(false);
	infoHorizontalLayout->addWidget(infoLabel, 0, Qt::AlignLeft);

	// Set text to defaults
	incrementButton->setText("+");
	decrementButton->setText("-");
	generateButton->setText("Generate View");
	policyGenLabel->setText("-/-");
	infoLabel->setText("Policy for agent: -/- Currently Shown");

	// Connect slots
	connect(incrementButton, &QPushButton::clicked, this, &FullTreeView::IncrementButtonClicked);
	connect(decrementButton, &QPushButton::clicked, this, &FullTreeView::DecrementButtonClicked);
	connect(generateButton, &QPushButton::clicked, this, &FullTreeView::GenerateButtonClicked);

	// Main vertical layout
	verticalLayout = new QVBoxLayout(parent);

	// Add the info pane to the vertical layout of the widget
	verticalLayout->addWidget(graphicsView);
	verticalLayout->addWidget(waitingForPlanLabel);
	verticalLayout->addWidget(buttonLabelContainer);
}


void FullTreeView::PlanStarting() {
	waitingForPlanLabel->show();
	buttonLabelContainer->hide();

	// Nothing needs displaying
	graphicsView->ChangeScene(NULL);

	// Delete all previous scenes
	for(Index i=0; i<scenes.size(); ++i) {
		scenes[i]->deleteLater();
	}

	// Reset
	scenes.clear();
	fullPolicyToShow = -1;
	currentFullPolicyShown = -1;
}


void FullTreeView::PlanFinished() {
	// Get new vectors
	fullPoliciesGenerated = std::vector<bool>(pManager->GetPlanningUnit()->GetNrAgents(), false);
	scenes = std::vector<TreeVisGraphicsScene*>(pManager->GetPlanningUnit()->GetNrAgents());

	// Create new graphics scenes for all
	for(Index i=0; i<scenes.size(); ++i) {
		scenes[i] = new TreeVisGraphicsScene(graphicsView);

		connect(scenes[i], &TreeVisGraphicsScene::DisplayObservationHistoryForNode,
				graphicsView, &TreeVisGraphicsView::DisplayObservationHistoryForNode);
	}

	// Set text
	infoLabel->setText("Policy for agent: -/- Currently Shown");
	policyGenLabel->setText(QString::number(currentFullPolicyShown+1) +
							"/" + QString::number(pManager->GetPlanningUnit()->GetNrAgents()));

	// Hide waiting label and show the button controls
	waitingForPlanLabel->hide();
	buttonLabelContainer->show();
}


void FullTreeView::SaveGraphicsViewToFile() {
	graphicsView->SaveToFile();
}


void FullTreeView::IncrementButtonClicked() {
	// Wrap as necessary
	fullPolicyToShow = ++fullPolicyToShow >= pManager->GetPlanningUnit()->GetNrAgents() ? 0 : fullPolicyToShow;
	UpdateFullPolicyToShowLabel();
}


void FullTreeView::DecrementButtonClicked() {
	// Wrap as necessary
	fullPolicyToShow = --fullPolicyToShow >= 0 ? fullPolicyToShow : pManager->GetPlanningUnit()->GetNrAgents()-1;
	UpdateFullPolicyToShowLabel();
}


void FullTreeView::GenerateButtonClicked() {
	// If no policy selected
	if(fullPolicyToShow == -1) {

		// Inform user
		emit AppendToInformationText("Choose an agent to generate the policy for first...",
									 MainWindow::Orange);
	} else {
		// Show the policy
		ShowFullPolicyForAgent(fullPolicyToShow);

		// Update full policy shown text
		infoLabel->setText("Policy for agent " +
						   QString::number(currentFullPolicyShown+1) + " Currently shown");
	}
}


void FullTreeView::ShowFullPolicyForAgent(const Index &agentIndex) {

	// If the policy has already been shown and the user is not attempting
	// to show the same policy again, we can just hide the current one and unhide the other one the user wants to see
	if(agentIndex != currentFullPolicyShown) {

		// If the new policy has already been expanded, show it
		if(fullPoliciesGenerated[agentIndex]) {
			emit AppendToInformationText("Policy already generated for agent " +
										 std::to_string(agentIndex+1) + ", showing the policy.", MainWindow::Normal);

		} else {
			// Otherwise we need to generate the full tree for the agent
			GenerateFTForAgent(agentIndex);
		}


		graphicsView->ChangeScene(scenes[agentIndex]);
		currentFullPolicyShown = fullPolicyToShow;
	} else {
		emit AppendToInformationText("Policy for agent " + std::to_string(agentIndex+1) +
									 " already shown", MainWindow::Orange);
	}
}


void FullTreeView::UpdateFullPolicyToShowLabel() {
	policyGenLabel->setText(QString::number(fullPolicyToShow+1) +
							"/" + QString::number(pManager->GetPlanningUnit()->GetNrAgents()));
}


int FullTreeView::GetLongestObservationLengthForAgent(const Index& agentIndex) {
	int longest = 0;

	QFontMetricsF fontMetrics = QFontMetricsF(Edge::GetFont());

	for(Index j=0; j<pManager->GetPlanningUnit()->GetNrObservations(agentIndex); ++j) {
		int width = fontMetrics.boundingRect(QString::fromStdString(
						pManager->GetPlanningUnit()->GetObservation(agentIndex, j)->GetName())).width();

		if(width > longest) {
			longest = width;
		}
	}

	return longest;
}


void FullTreeView::GenerateFTForAgent(const Index &agentIndex) {
	// Otherwise we need to generate the policy, +1 from arrray index
	emit AppendToInformationText("Generating tree for Agent " + std::to_string(agentIndex+1) + "...",
								 MainWindow::Normal);

	// Get the scene for this agent
	TreeVisGraphicsScene* scene = scenes[agentIndex];

	// Get the number of observations and the history index (-1 as starts from 0)
	int nrObservations = pManager->GetPlanningUnit()->GetNrObservations(agentIndex);
	int obvsHistoryIndex = pManager->GetPlanningUnit()->GetNrObservationHistories(agentIndex)-1;

	// Create vector of correct size (+1 as index)
	std::vector<Node*> nodes(obvsHistoryIndex+1);

	// Create first node
	Node* toAdd = new Node(pManager->GetPlanningUnit()->GetAction(agentIndex,
								pManager->GetActionIndex(agentIndex, obvsHistoryIndex))->GetName(),
						   obvsHistoryIndex,
						   agentIndex);

	// Set the position to (0,0), add it to the scene and set the reference in the vector to it
	toAdd->setPos(QPoint(0, 0));
	scene->addItem(toAdd);
	nodes[obvsHistoryIndex] = toAdd;

	// Calculate number of nodes
	int numNodes = pow(nrObservations, pManager->GetPlanningUnit()->GetHorizon()-1);

	// Take off 1 as we have added the first node
	--numNodes;
	--obvsHistoryIndex;

	// Are we still on the last level of the tree
	bool lastLevel = (numNodes != 0);

	// Height multiplier works in reverse to the current depth basically...
	// We want to set this as the min of (-100, -(lengthOfLongestObvsName + toAdd->boundingRect().height()))
	// -100 is a base number to separate at least -100 vertical coordinates between levels of the tree,
	// otherwise we want a larger separation as the observation name dominates the 100
	int heightMultiplier = 0;

	// How much to separate
	int heightSeparation = GetLongestObservationLengthForAgent(agentIndex);
	heightSeparation = std::max(50*nrObservations, (heightSeparation + (int) std::round(toAdd->boundingRect().height())+10));

	// Generate nodes
	for(int currentDepth=pManager->GetPlanningUnit()->GetHorizon(); currentDepth>0; --currentDepth) {

		// Iterate for the number of nodes on this level
		for(int j=0; j<numNodes; ++j) {

			// Create a new node
			toAdd = new Node(pManager->GetPlanningUnit()->GetAction(agentIndex,
							 pManager->GetActionIndex(agentIndex, obvsHistoryIndex))->GetName(),
							 obvsHistoryIndex,
							 agentIndex);

			// If we are on the 'last level' we want to position the next node to the left of the
			// previously placed one by a set number of 'padding'
			if(lastLevel) {

				// Extract node to 'right'
				Node* prev = nodes[obvsHistoryIndex+1];

				// Get the position, set the new node to the left of the previously added node by 20
				// Offset calculated as the two mid points added together to get the correct number
				QPoint point(prev->scenePos().x(), prev->scenePos().y());
				int offset = prev->GetWidth()/2 + toAdd->GetWidth()/2 + paddingBetweenNodes;

				// Set the position
				toAdd->setPos(point.x()-offset, point.y());
			} else {

				// Otherwise we want to position the node in the mid point between it's two furthest children nodes
				int leftMostChildIndex = (nrObservations*obvsHistoryIndex)+1;
				int rightMostChildIndex = leftMostChildIndex + (nrObservations-1);

				// Get the two edge children of the node
				Node* leftMostChild = nodes[leftMostChildIndex];
				Node* rightMostChild = nodes[rightMostChildIndex];

				// Calculate the offset and place the parent node at this offset
				int offset = (leftMostChild->x() - leftMostChild->GetWidth()/2) + (rightMostChild->x() + rightMostChild->GetWidth()/2);
				toAdd->setPos((offset/2), heightMultiplier*-heightSeparation);

				// Add the Edges between the parent and child nodes
				// this loop will naturally iterate over the number of observations the agent has
				for(int q=leftMostChildIndex, obvsIndex = 0; q<=rightMostChildIndex; ++q, ++obvsIndex) {
					// Get the observation name
					std::string obvsName = pManager->GetPlanningUnit()->GetObservation(agentIndex, obvsIndex)->GetName();

					// Create edge and add
					Edge* edge = new Edge(toAdd, nodes[q], obvsName);
					scene->addItem(edge);
				}
			}

			// Add to the scene and vector
			scene->addItem(toAdd);
			nodes[obvsHistoryIndex] = toAdd;

			// To prev index (next node)
			--obvsHistoryIndex;
		}

		// No longer on last level, re calculate number of nodes
		lastLevel = false;
		numNodes = pow(nrObservations, currentDepth-2);
		++heightMultiplier;
	}

	// Policy has been generated
	fullPoliciesGenerated[agentIndex] = true;
}
