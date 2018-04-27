#ifndef Edge_H
#define Edge_H

// TreeVis
#include "Node.h"

// Qt
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

#include <iostream>

class Node;

/// The thickness of the edge
const static qreal edgeThickness = 3.0;

///
/// \brief The Edge class represents an undirected edge
/// between two nodes in a scene. The edge will be drawn
/// directly between the centres of the 'fromNode' and the 'toNode'
///
class Edge : public QGraphicsLineItem {

	public:
		///
		/// \brief Edge Constructor
		/// \param from The node the edge is from
		/// \param to The node the edge will go to
		/// \param labelText The label of the edge
		/// \param obvsProb The observation probability of recieving the
		/// joint observation associated with the edge
		///
		Edge(Node* from, Node* to, const QString &labelText = "", const double& obvsProb = 0);

		/// Alternate constructor that takes a std::string
		Edge(Node* from, Node* to, const std::string& labelText = "", const double& obvsProb = 0);

		///
		~Edge();

		///
		/// \brief Updates the position
		/// between the two nodes, drawing the edge as needed
		///
		void UpdatePosition();

		///
		/// \brief ChangeColourToMatchProbability
		/// Colours the edge to the probability field based on
		/// the given boolean.
		/// \param colourToProbability True if the colour is to match the probability
		/// false otherwise
		///
		void ChangeColourToMatchProbability(const bool &colourToProbability);

		///
		/// \brief SetEdgeColour
		/// Changes the edges colour to the given one
		/// \param newColour The colour to set the edge to
		///
		void SetEdgeColour(const QColor &newColour);

		///
		/// \brief SetTextColour
		/// Changes the text colour to the given one
		/// \param newColour The colour to set the text to
		///
		void SetTextColour(const QColor &newColour);

		///
		/// \brief Sets the default colour of the edge to the given colour
		/// \param newColour The colour to set the default edge colour to
		///
		static void SetDefaultColour(const QColor &newColour);

		///
		/// \brief Sets the default colour of the text on the edge to the given colour
		/// \param newColour The colour to set the default text colour to
		///
		static void SetDefaultTextColour(const QColor &newColour);

		///
		/// \brief Sets the font of all edges to the given font
		/// \param newFont The font to set the font of all edges to
		///
		static void SetFont(const QFont &newFont);

		///
		/// \brief GetDefaultFont Gets the font
		/// \return The font used by all edges
		///
		static QFont GetFont();

		/// Type for qgraphicsitem_cast<>
		enum {
			Type = UserType + 2
		};

		///
		/// \brief Type implementation provided for qgraphicsitem_cast<>
		/// \return UserType
		///
		int type() const {
			return UserType + 2;
		}

	private:
		/// The node the edge goes from
		Node* fromNode;

		/// The node the edge goes to
		Node* toNode;

		/// The text to be displayed in the middle of the edge
		QGraphicsSimpleTextItem* label = nullptr;

		/// The observation probability for this edge
		double observationProbability;

		/// The colour of the edge
		QColor edgeColour = defaultEdgeColour;

		/// The colour of the text associated with the edge
		QColor textColour = defaultTextColour;

		/// Default edge colour, set in settings
		static QColor defaultEdgeColour;

		/// Default text colour, set in settings
		static QColor defaultTextColour;

		/// Font used by all edges, set in settings
		static QFont font;
};

#endif // Edge_H
