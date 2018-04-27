#ifndef TREEVISGRAPHICSVIEW_H
#define TREEVISGRAPHICSVIEW_H

// TreeVis
#include "Node.h"
#include "PlannerManager.h"

// Qt
#include <QGraphicsView>

///
/// \brief Custom QGraphicsView to support zooming
/// via the mouse wheel.
///
class TreeVisGraphicsView : public QGraphicsView {
	Q_OBJECT

	public:
		///
		/// \brief Calls the super constructor of the QGraphicsView the class inherits from.
		/// Sets display rendering properties
		///
		TreeVisGraphicsView(PlannerManager* man, QWidget* parent = 0);
		~TreeVisGraphicsView();

		/// Resets the scene to a new one
		void Reset();

		/// \brief Prompts the user to save the currently
		/// rendered scene to a file of their choice
		void SaveToFile();

		/// \brief Resets the zoom level and changes the scene to
		/// the new scene
		/// \param newScene The scene to change to
		void ChangeScene(QGraphicsScene* newScene);


	protected:
		/// Mouse Wheel event on the view
		virtual void wheelEvent(QWheelEvent* event);

	public slots:
		///
		/// \brief Slot called from QGraphicsScene via the
		/// context menu of a node. Pops up a dialog
		/// with the observaion history for the given node
		/// \param node The node to get the observation history from
		///
		void DisplayObservationHistoryForNode(const Node* node);

	private:
		/// Zoom factor
		const double factor = 1.15;

		/// Planner manager to access observation histories
		PlannerManager* pManager;
};

#endif // TREEVISGRAPHICSVIEW_H
