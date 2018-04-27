#include "TreeVisGraphicsView.h"

// TreeVis
#include "Node.h"
#include "TreeVisGraphicsScene.h"

// Qt
#include <QSvgGenerator>
#include <QMessageBox>
#include <QFileDialog>
#include <QWheelEvent>
#include <QtMath>
#include "qgraphicsitem.h"

// MADP
#include "Observation.h"

TreeVisGraphicsView::TreeVisGraphicsView(PlannerManager* man, QWidget* parent) : QGraphicsView(parent) {
	pManager = man;
	setRenderHint(QPainter::Antialiasing);
	setDragMode(QGraphicsView::ScrollHandDrag);
}


TreeVisGraphicsView::~TreeVisGraphicsView()  {
	//std::cout << "~TreeVisGraphicsView()" << std::endl;
}


void TreeVisGraphicsView::Reset() {
	// Delete old scene if not first run
	if(scene()) {
		scene()->deleteLater();
	}

	// Reset zoom
	resetMatrix();

	// Make new scene and set
	TreeVisGraphicsScene* newScene = new TreeVisGraphicsScene(this);
	ChangeScene(newScene);

	connect(newScene, &TreeVisGraphicsScene::DisplayObservationHistoryForNode,
			this, &TreeVisGraphicsView::DisplayObservationHistoryForNode);
}


void TreeVisGraphicsView::ChangeScene(QGraphicsScene* newScene) {
	resetMatrix();
	setScene(newScene);
}


void TreeVisGraphicsView::SaveToFile() {

	// If no items display warning
	if(!scene() || scene()->items().size() == 0) {
		QMessageBox::warning(this, "MADP Tree Vis", "Nothing in view, nothing to save.");

	} else {

		// Get a save file path
		QString imagePath = QFileDialog::getSaveFileName(
				this, "Save File", QDir::homePath(), "SVG (*.svg)");

		// If we got an image path, save the scene to the path
		if(imagePath != "") {
			QSvgGenerator generator;
			generator.setFileName(imagePath);

			int width = qCeil(qreal(scene()->sceneRect().size().width()/1.25));
			int height = qCeil(qreal(scene()->sceneRect().size().height()/1.25));

			generator.setSize(QSize(width, height));
			generator.setViewBox(QRect(0, 0, width, height));

			generator.setTitle("MADP TreeVis Output");

			// Paint the image
			QPainter painter;
			painter.begin(&generator);
			scene()->render(&painter);
			painter.end();
		}
	}
}


void TreeVisGraphicsView::wheelEvent(QWheelEvent* event) {

	// Anchor on position of the mouse
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	// If no control key, allow scroll and return
	if(!(event->modifiers() & Qt::ControlModifier)) {
		QGraphicsView::wheelEvent(event);
		return;
	}

	// Control key must be presed - prevent zoom with no items
	if(scene() && scene()->items().count() > 0) {

		// If we scrolled forwards zoom in
		if(event->delta() > 0) {
			scale(factor, factor);
		} else {

			// Otherwise zoom out
			scale(1/factor, 1/factor);
		}
	}
}


void TreeVisGraphicsView::DisplayObservationHistoryForNode(const Node* node) {

	// Get agent and oh index from the node
	Index agentIndex = node->GetAgentIndex();
	Index ohIndex = node->GetOHIndex();

	// Fetch timestep, fill array with observation indexes
	Index timeStep = pManager->GetPlanningUnit()->GetTimeStepForOHI(agentIndex, ohIndex);

	// Must make dynamic as variable used for initialisation
	Index* observationsIndexArray = new Index[timeStep];

	pManager->GetPlanningUnit()->GetObservationHistoryArrays(
				agentIndex,
				ohIndex,
				timeStep,
				observationsIndexArray);

	// Create the history as needed
	std::stringstream output;

	// If actually any history
	if(timeStep > 0) {

		//Index currentOHI = 0;

		for(Index timeS=0; timeS<timeStep; ++timeS) {
			// Fetch current obvs, add to output
			std::string currentObvs = pManager->GetPlanningUnit()->GetObservation(node->GetAgentIndex(), observationsIndexArray[timeS])->GetName();

			//std::string currentAction = pManager->GetPlanningUnit()->GetAction(agentIndex, pManager->GetActionIndex(agentIndex, currentOHI))->GetName();
			//currentOHI = pManager->GetPlanningUnit()->GetSuccessorOHI(agentIndex, currentOHI, observationsIndexArray[timeS]);

			output << timeS+1 << ": " << currentObvs /*<< " : " << currentAction*/ << std::endl;
		}

	} else {
		output << "No observation history as time step 0!";
	}

	// Display in message box
	QMessageBox messageBox(QMessageBox::Information,
						   "Obvs History for Index " + QString::number(ohIndex),
						   QString::fromStdString(output.str()),
						   QMessageBox::NoButton,
						   this, Qt::Dialog);


	messageBox.exec();

	// Delete array
	delete[] observationsIndexArray;
}
