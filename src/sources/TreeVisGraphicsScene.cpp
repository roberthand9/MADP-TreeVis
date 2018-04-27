#include "TreeVisGraphicsScene.h"

// Qt
#include <QGraphicsSceneContextMenuEvent>
#include <QColorDialog>
#include <iostream>

// TreeVis
#include "GeneralUtils.h"
#include "Node.h"
#include "Edge.h"


TreeVisGraphicsScene::TreeVisGraphicsScene(QWidget* parent) : QGraphicsScene(parent) {
	// Create menus
	itemColourPickMenu = new QMenu("Change Item Colour", parent);
	textColourPickMenu = new QMenu("Change Text Colour", parent);
	nodeOutlineColourPickMenu = new QMenu("Change Outline Colour", parent);

	// Populate menus with colours, connect to appropritate slots
	for(int i=0; i< ColourUtils::colourList.size(); ++i) {
		QAction* itemColourAction = itemColourPickMenu->addAction(ColourUtils::colourList[i]);
		connect(itemColourAction, &QAction::triggered, this, [this, i] {ChangeItemColour(i);});

		QAction* textColourAction = textColourPickMenu->addAction(ColourUtils::colourList[i]);
		connect(textColourAction, &QAction::triggered, this, [this, i] {ChangeItemTextColour(i);});

		QAction* nodeOutlineColourAction = nodeOutlineColourPickMenu->addAction(ColourUtils::colourList[i]);
		connect(nodeOutlineColourAction, &QAction::triggered, this, [this, i] {ChangeNodeOutlineColour(i);});
	}

	// Create actions
	changeItemColourAction = new QAction("Change Colour", this);
	changeItemTextColourAction = new QAction("Change Text Colour", this);
	changeNodeOutlineColourAction = new QAction("Change outline colour", this);
	viewNodeOHAction = new QAction("View Observation History", this);

	// Connect to slots
	connect(changeItemColourAction, &QAction::triggered,
			this, &TreeVisGraphicsScene::ChangeItemColour);

	connect(changeItemTextColourAction, &QAction::triggered,
			this, &TreeVisGraphicsScene::ChangeItemTextColour);

	connect(changeNodeOutlineColourAction, &QAction::triggered,
			this, &TreeVisGraphicsScene::ChangeNodeOutlineColour);

	connect(viewNodeOHAction, &QAction::triggered,
			this, &TreeVisGraphicsScene::ViewNodeObservationHistory);
}


TreeVisGraphicsScene::~TreeVisGraphicsScene() {
	//std::cout << "~TreeVisGraphicsScene()" << std::endl;
}


void TreeVisGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
	// Try extract an item
	selectedNode = qgraphicsitem_cast<Node*>(itemAt(event->scenePos(), QTransform()));
	selectedEdge = qgraphicsitem_cast<Edge*>(itemAt(event->scenePos(), QTransform()));

	// Create menu, add global actions
	QMenu menu;
	menu.addMenu(itemColourPickMenu);
	menu.addMenu(textColourPickMenu);

	// If we got a node, add extras
	if(selectedNode) {
		menu.addMenu(nodeOutlineColourPickMenu);
		menu.addAction(viewNodeOHAction);

		// Display menu
		menu.exec(event->screenPos());
	} else if(selectedEdge) {

		// Otherwise edge was selected
		menu.exec(event->screenPos());
	}

	// Nothing selected so no need to display
}


void TreeVisGraphicsScene::ChangeItemColour(const int &id) {
	QColor selectedColour = ColourUtils::GetColourFromID(id);

	// In case of colour dialog closing
	if(selectedColour != QColor::Invalid) {

		// If node selected, call on node
		if(selectedNode) {
			selectedNode->SetFillColour(selectedColour);
		} else {
			selectedEdge->SetEdgeColour(selectedColour);
		}
	}
}


void TreeVisGraphicsScene::ChangeItemTextColour(const int &id) {
	QColor selectedColour = ColourUtils::GetColourFromID(id);

	// In case of colour dialog closing
	if(selectedColour != QColor::Invalid) {

		// If node selected, call on node
		if(selectedNode) {
			selectedNode->SetTextColour(selectedColour);
		} else {
			selectedEdge->SetTextColour(selectedColour);
		}
	}
}


void TreeVisGraphicsScene::ChangeNodeOutlineColour(const int &id) {
	QColor selectedColour = ColourUtils::GetColourFromID(id);

	// In case of colour dialog closing
	if(selectedColour != QColor::Invalid) {
		selectedNode->SetOutlineColour(selectedColour);
	}
}


void TreeVisGraphicsScene::ViewNodeObservationHistory() {
	emit DisplayObservationHistoryForNode(selectedNode);
}
