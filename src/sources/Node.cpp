#include "Node.h"
#include "Edge.h"

#include <iostream>

#include <QFontMetrics>
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QAction>

QColor Node::defaultFillColour;
QColor Node::defaultOutlineColour;
QColor Node::defaultTextColour;
QFontMetricsF Node::fontMetrics = QFontMetricsF(font);
QFont Node::font;


Node::Node(const QString &text, const Index &ohi, const Index &ai) {
	nodeText = text;
	ohIndex = ohi;
	agentIndex = ai;
}


Node::Node(const std::string &text, const Index &ohi, const Index &ai) :
	Node(QString::fromStdString(text), ohi, ai) {}


Node::~Node() {
	//std::cout << "~Node()" << std::endl;
}


void Node::SetDefaultFillColour(const QColor &newColour) {
	defaultFillColour = newColour;
}


void Node::SetDefaultOutlineColour(const QColor &newColour) {
	defaultOutlineColour = newColour;
}


void Node::SetDefaultTextColour(const QColor &newColor) {
	defaultTextColour = newColor;
}


void Node::SetFont(const QFont &newFont) {
	font = newFont;

	// Update metrics
	fontMetrics = QFontMetricsF(newFont);
}


QFont Node::GetFont() {
	return font;
}


Index Node::GetOHIndex() const {
	return ohIndex;
}


Index Node::GetAgentIndex() const {
	return agentIndex;
}


int Node::GetWidth() {
	return boundingRect().width();
}


int Node::GetHeight() {
	return boundingRect().height();
}


void Node::SetFillColour(const QColor &newColour) {
	fillColour = newColour;
	update(); // Repaint as needed
}


void Node::SetTextColour(const QColor &newColour) {
	textColour = newColour;
	update(); // Repaint as needed
}


void Node::SetOutlineColour(const QColor &newColour) {
	outlineColour = newColour;
	update(); // Repaint as needed
}


QRectF Node::OutlineRect() const {
	// Get rect based on text
	QRectF rect = fontMetrics.boundingRect(nodeText);

	// Add padding and centre
	rect.adjust(-padding, -padding, +padding, +padding);
	rect.translate(-rect.center());
	return rect;
}


QRectF Node::boundingRect() const {
	// 1 margin for bounding
	const int margin = 1;
	return OutlineRect().adjusted(-margin, -margin, +margin, +margin);
}


QPainterPath Node::shape() const {
	QRectF rect = OutlineRect();
	QPainterPath path;

	// Add to path with roundness
	path.addRoundedRect(rect, roundness, roundness);
	return path;
}


void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
	// Set pen to outline colour, brush to fill
	painter->setPen(QPen(outlineColour, 2));
	painter->setBrush(fillColour);

	QRectF rect = OutlineRect();
	painter->drawRoundRect(rect, roundness, roundness);

	painter->setFont(font);

	// Change pen colour then paint text
	painter->setPen(textColour);
	painter->drawText(rect, Qt::AlignCenter, nodeText);
}
