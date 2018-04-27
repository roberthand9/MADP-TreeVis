	#include "Edge.h"

// Qt
#include <QPen>
#include <math.h>

const qreal labelPaddingAdjust = 3;

QColor Edge::defaultEdgeColour;
QColor Edge::defaultTextColour;
QFont Edge::font;


Edge::Edge(Node* from, Node* to, const QString &labelText, const double& obvsProb) {
	// Set nodes values
	fromNode = from;
	toNode = to;
	observationProbability = obvsProb;

	// Hide behind the nodes
	setZValue(-1);

	// If a label has been supplied
	if(labelText != "") {

		// Create new label with the text, passing the line as the parent of the label
		label = new QGraphicsSimpleTextItem(labelText, this);
	}

	UpdatePosition();
}


// Call constructor with converted QString
Edge::Edge(Node* from, Node* to, const std::string& labelText, const double& obvsProb) :
	Edge(from, to, QString::fromStdString(labelText), obvsProb) {}


void Edge::UpdatePosition() {
	// Set the start position of the edge to be from nodes position
	setPos(fromNode->pos());

	// Set the position of the edge, set the pen to the defaults
	setLine(QLineF(mapFromScene(pos()), mapFromScene(toNode->pos())));
	setPen(QPen(edgeColour, edgeThickness));

	// If a label has been created
	if(label) {

		// Brush to text colour
		label->setFont(font);
		label->setBrush(textColour);

		// Centre on line and get angle
		label->setPos(line().center());
		qreal angle = line().angle();

		// Offset to position on centre of line rather than top left corner
		qreal xOffset = label->boundingRect().width()/2;
		qreal yOffset = label->boundingRect().height();

		// Edge is going left if angle is < 270, calculate
		// angle as a percentage between 0-90
		bool leftSide = (angle < 270);
		qreal mult = (double) ((int) angle % 90) / 90;

		// If left side adjust appropriately
		if(leftSide) {
			xOffset += mult*labelPaddingAdjust;
			yOffset += (1-mult)*labelPaddingAdjust;
			if(angle != 0) angle += 180; // Flip label if not horizontal i.e. -
		} else {

			// Otherwise opposite direction
			xOffset -= (1-mult)*labelPaddingAdjust;
			yOffset += mult*labelPaddingAdjust;
		}

		// Move by the offset, set the origin point to the bottom left of the label
		label->moveBy(-xOffset, -yOffset);
		label->setTransformOriginPoint(label->boundingRect().width()/2,
									   label->boundingRect().height());

		// Rotate
		label->setRotation(360-angle);
	}
}


QFont Edge::GetFont() {
	return font;
}


void Edge::SetDefaultColour(const QColor &newColour) {
	defaultEdgeColour = newColour;
}


void Edge::SetDefaultTextColour(const QColor &newColour) {
	defaultTextColour = newColour;
}


void Edge::SetFont(const QFont &newFont) {
	font = newFont;
}


void Edge::ChangeColourToMatchProbability(const bool &colourToProbability) {
	// Change to default colour
	if(colourToProbability) {
		setPen(QPen(edgeColour, edgeThickness));

	} else {
		// HSL Colour, 120 is code for all green so we take a
		// percentage of the number based on the observation probability.
		// Full saturation at 255 & 255/2 = ~127 for lightness
		QColor colour = QColor::fromHsl((120 * observationProbability), 255, 127);
		setPen(QPen(colour, edgeThickness));
	}
}


void Edge::SetEdgeColour(const QColor &newColour) {
	edgeColour = newColour;
	setPen(QPen(edgeColour, edgeThickness));
}


void Edge::SetTextColour(const QColor &newColour) {
	// If there is a label associated with the edge
	if(label) {
		textColour = newColour;
		label->setBrush(textColour);
	}
}

Edge::~Edge()  {
	// std::cout << "~Edge()" << std::endl;
}
