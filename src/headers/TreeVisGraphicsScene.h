#ifndef TREEVISGRAPHICSSCENE_H
#define TREEVISGRAPHICSSCENE_H

// TreeVis
#include "Node.h"
#include "Edge.h"

// Qt
#include <QGraphicsScene>
#include <QAction>
#include <QMenu>

#include <iostream>


///
/// \brief The TreeVisGraphicsScene class provides extra functionality
/// for context menus for edges and node object types.
///
class TreeVisGraphicsScene : public QGraphicsScene {
	Q_OBJECT

	public:
		///
		/// \brief Constructor. Creates context menu actions and
		/// connects slots.
		/// \param parent The parent to the scene
		///
		TreeVisGraphicsScene(QWidget* parent = 0);
		~TreeVisGraphicsScene();

	protected:
		///
		/// \brief Context menu event on the scene, a right click.
		/// \param event The event object
		///
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);


	signals:
		///
		/// \brief Signals a dialog needs opening containing the nodes
		/// observation history
		/// \param node The selected node to get the history for
		///
		void DisplayObservationHistoryForNode(Node* node);

	private slots:
		///
		/// \brief Changes the currently selected item (either edge or node)
		/// to the given colour. Called from the context menu
		/// \param id The id of the colour chosen in the menu
		///
		void ChangeItemColour(const int& id);

		///
		/// \brief Changes the currently selected items text (either edge or node)
		/// to the given colour. Called from the context menu
		/// \param id The id of the colour chosen in the menu
		///
		void ChangeItemTextColour(const int& id);

		///
		/// \brief Changes the currently selected nodes outline
		/// colour to to the given colour. Called from the context menu
		/// \param id The id of the colour chosen in the menu
		///
		void ChangeNodeOutlineColour(const int& id);

		///
		/// \brief Opens a dialog containing the observation
		/// history for the node. Called from the context menu
		///
		void ViewNodeObservationHistory();

	private:
		/// Selected node when context menu shown
		Node* selectedNode;
		/// Selected edge when context menu shown
		Edge* selectedEdge;

		// Context menus
		QMenu* itemColourPickMenu;
		QMenu* textColourPickMenu;
		QMenu* nodeOutlineColourPickMenu;

		// Actions for context menu
		QAction* changeItemColourAction;
		QAction* changeItemTextColourAction;
		QAction* changeNodeOutlineColourAction;
		QAction* viewNodeOHAction;
};

#endif // TREEVISGRAPHICSSCENE_H
