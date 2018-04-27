#ifndef NODE_H
#define NODE_H

// TreeVis
#include "Edge.h"

// Qt
#include <QGraphicsItem>
#include <QFontMetrics>
#include <QSet>


/// To match MADP
typedef unsigned int Index;

class Edge;

///
/// \brief Node class is to be used in a QGraphicsScene.
/// Text passed will be drawn at the centre of the node
/// and functionality is provided to customise the look
/// of the node in the scene.
///
class Node : public QGraphicsItem {

	public:
		///
		/// \brief Constructor creates node sets text
		/// \param text Text to go in centre of node
		/// \param ohi The observation history index of the node
		/// \param ai The Action Index of the node
		/// in an individual policy
		///
		Node(const QString &text, const Index& ohi = -1, const Index& ai = -1);

		/// Alternate constructor that takes a std::string
		Node(const std::string &text, const Index& ohi = -1, const Index& ai = -1);

		/// Destructor
		~Node();

		///
		/// \brief Bounding rect implementation for QGraphicsItem
		/// \return The bounding rect of the node
		///
		QRectF boundingRect() const;

		///
		/// \brief Shape implementation for QGraphicsItem
		/// \return The path of the rect for the node
		///
		QPainterPath shape() const;

		///
		/// \brief Paints the node on a QGraphicsScene,
		/// implementation of QGraphicsItem paint
		/// \param painter Painter provided by Qt
		/// \param option Unused
		/// \param widget Unused
		///
		void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget);

		///
		/// \brief Sets the filled colour of the node
		/// to the given colour
		/// \param newColour The colour to set the node to
		///
		void SetFillColour(const QColor &newColour);

		///
		/// \brief Sets the text in the node
		/// to the given colour
		/// \param newColour The colour to set the node to
		///
		void SetTextColour(const QColor &newColour);

		///
		/// \brief Sets the outline colour of the node
		/// to the given colour
		/// \param newColour The colour to set the node to
		///
		void SetOutlineColour(const QColor &newColour);

		///
		/// \brief Gets the height of the node
		/// \return boundingRect().height()
		///
		int GetHeight();

		///
		/// \brief Gets the width of the node
		/// \return boundingRect().width()
		///
		int GetWidth();

		///
		/// \brief Gets the observation history index of the node.
		/// If this is not set it will return -1
		/// \return The OHI of the node
		///
		Index GetOHIndex() const;

		///
		/// \brief Gets the agent index of the node.
		/// If this is not set it will return -1
		/// \return The agent index of the node
		///
		Index GetAgentIndex() const;

		///
		/// \brief Sets the default colour of future nodes to the given colour
		/// \param newColour The colour to set the default fill colour to
		///
		static void SetDefaultFillColour(const QColor &newColour);

		///
		/// \brief Sets the default outline colour of future nodes to the given colour
		/// \param newColour The colour to set the default outline colour to
		///
		static void SetDefaultOutlineColour(const QColor &newColour);

		///
		/// \brief Sets the default text colour of future nodes to the given colour
		/// \param newColour The colour to set the default text colour to
		///
		static void SetDefaultTextColour(const QColor &newColour);

		///
		/// \brief Sets the font of future nodes to the given font
		/// \param newFont The font to set the font for all nodes to
		///
		static void SetFont(const QFont &newFont);

		///
		/// \brief GetDefaultFont Gets the font
		/// \return The font used by all nodes
		///
		static QFont GetFont();

		/// Type for qgraphicsitem_cast<>
		enum {
			Type = UserType + 1
		};

		///
		/// \brief Type implementation provided for qgraphicsitem_cast<>
		/// \return UserType
		///
		int type() const {
			return Type;
		}

	private:
		/// \brief The Observation history index for the node in the agents individual policy.
		/// -1 if not set in constructor.
		Index ohIndex;

		/// The Agent Index for the node. -1 if not set in constructor.
		Index agentIndex;

		/// Text to go in centre of the node
		QString nodeText;

		/// The fill colour of the node
		QColor fillColour = defaultFillColour;

		/// The outline colour of the node
		QColor outlineColour = defaultOutlineColour;

		/// The text colour of the node
		QColor textColour = defaultTextColour;

		/// \return A padded node
		QRectF OutlineRect() const;

		/// Padding for nodes
		static const int padding = 8;

		/// Roundness of nodes
		static const int roundness = 60;

		/// Default node fill colour, set in settings
		static QColor defaultFillColour;

		/// Default node outline colour, set in settings
		static QColor defaultOutlineColour;

		/// Default node text colour, set in settings
		static QColor defaultTextColour;

		/// Font for all nodes, set in settings
		static QFont font;

		/// Used to calculate node size based on text
		static QFontMetricsF fontMetrics;
};

#endif // NODE_H
