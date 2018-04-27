#include "SettingsDialog.h"

// TreeVis
#include "GeneralUtils.h"

// Qt
#include <QHBoxLayout>
#include <QFormLayout>
#include <QColorDialog>


SettingsDialog::~SettingsDialog() {
	//std::cout << "~SettingsDialog()" << std::endl;
}


SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
	// Geometry in middle of screen
	setGeometry(parent->geometry().x() + parent->geometry().width()/2,
				parent->geometry().y() + parent->geometry().y()/2,
				800, 300);

	// Global layout
	QVBoxLayout* dialogLayout = new QVBoxLayout(this);

	// Wrap and layouts for items
	QWidget* sceneAndOptionsWrap = new QWidget(this);
	QHBoxLayout* sceneAndOptionsLayout = new QHBoxLayout(sceneAndOptionsWrap);
	QWidget* controlWrap = new QWidget(sceneAndOptionsWrap);
	QFormLayout* formLayout = new QFormLayout(controlWrap);

	graphicsView = new QGraphicsView(sceneAndOptionsWrap);
	graphicsView->setScene(new QGraphicsScene(graphicsView));
	graphicsView->setRenderHint(QPainter::Antialiasing);

	// Create live preview items
	nodeOne = new Node(QString("Node One"));
	nodeTwo = new Node(QString("Node Two"));
	edge = new Edge(nodeOne, nodeTwo, QString("Edge Text"));

	graphicsView->scene()->addItem(nodeOne);
	graphicsView->scene()->addItem(nodeTwo);
	graphicsView->scene()->addItem(edge);

	// Node combo boxes
	nodeFillColourComboBox = new QComboBox(controlWrap);
	nodeOutlineColourComboBox = new QComboBox(controlWrap);
	nodeTextColourComboBox = new QComboBox(controlWrap);

	// Add colour list to boxes
	nodeFillColourComboBox->addItems(ColourUtils::colourList);
	nodeOutlineColourComboBox->addItems(ColourUtils::colourList);;
	nodeTextColourComboBox->addItems(ColourUtils::colourList);;

	// Edge combo boxes
	edgeColourComboBox = new QComboBox(controlWrap);
	edgeTextColourComboBox = new QComboBox(controlWrap);

	// Static cast to overload - http://wiki.qt.io/New_Signal_Slot_Syntax#Overload
	connect(nodeFillColourComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									this, &SettingsDialog::UpdateNodeFillColour);

	connect(nodeOutlineColourComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									this, &SettingsDialog::UpdateNodeOutlineColour);

	connect(nodeTextColourComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									this, &SettingsDialog::UpdateNodeTextColour);

	// Add colour list to boxes
	edgeColourComboBox->addItems(ColourUtils::colourList);
	edgeTextColourComboBox->addItems(ColourUtils::colourList);

	connect(edgeColourComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									this, &SettingsDialog::UpdateEdgeColour);

	connect(edgeTextColourComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									this, &SettingsDialog::UpdateEdgeTextColour);

	fontSizeSpinBox = new QSpinBox(controlWrap);
	fontSizeSpinBox->setRange(1, 200); // Max size 200

	// Font combo box
	fontComboBox = new QFontComboBox(controlWrap);
	fontComboBox->setFontFilters(QFontComboBox::ScalableFonts);

	// Connect font related
	connect(fontComboBox, &QFontComboBox::currentFontChanged,
			this, &SettingsDialog::UpdateFont);

	connect(fontSizeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
			this, &SettingsDialog::UpdateFont);


	// Add all to form layout
	formLayout->addRow("Node Fill Colour:", nodeFillColourComboBox);
	formLayout->addRow("Node Outline Colour:", nodeOutlineColourComboBox);
	formLayout->addRow("Node Text Colour:", nodeTextColourComboBox);

	formLayout->addRow("Edge Colour:", edgeColourComboBox);
	formLayout->addRow("Edge Text Colour:", edgeTextColourComboBox);

	formLayout->addRow("Font:", fontComboBox);
	formLayout->addRow("Font Size:", fontSizeSpinBox);


	// Button box for dialog
	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	// Connect the accept and reject buttons
	connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::Accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::Reject);

	// Add scene and control widgets
	sceneAndOptionsLayout->addWidget(graphicsView);
	sceneAndOptionsLayout->addWidget(controlWrap);

	// Add widgets to dialogs
	dialogLayout->addWidget(sceneAndOptionsWrap);
	dialogLayout->addWidget(buttonBox);

	// Set the layout of the dialog
	setLayout(dialogLayout);

	// Load the default settings
	LoadSettings();
}


void SettingsDialog::LoadSettings() {
	QSettings settings;

	// If no settings set, use hard coded default values
	if(!settings.contains("node/fillColour")) {
		nodeFillColourComboBox->setCurrentIndex(1); // White
		selectedNodeFillColour = QColor("white");

		nodeOutlineColourComboBox->setCurrentIndex(0); // Black
		selectedNodeOutlineColour = QColor("black");

		nodeTextColourComboBox->setCurrentIndex(0);
		selectedNodeTextColour = QColor("black");

		edgeColourComboBox->setCurrentIndex(0);
		selectedEdgeColour = QColor("black");

		edgeTextColourComboBox->setCurrentIndex(0);
		selectedEdgeTextColour = QColor("black");

		fontComboBox->setCurrentFont(QFont("Helvetica"));
		fontSizeSpinBox->setValue(12);

		UpdateColours();
		UpdateFont();
		return;
	}

	// Temp array of combo boxes
	QComboBox* comboBoxes[5];
	comboBoxes[0] = nodeFillColourComboBox;
	comboBoxes[1] = nodeOutlineColourComboBox;
	comboBoxes[2] = nodeTextColourComboBox;
	comboBoxes[3] = edgeColourComboBox;
	comboBoxes[4] = edgeTextColourComboBox;


	// Disable signals for now
	for(QComboBox* box : comboBoxes) {
		box->blockSignals(true);
	}

	// Fetch colours in settings file
	QColor colours[5];
	selectedNodeFillColour		= colours[0] = settings.value("node/fillColour").value<QColor>();
	selectedNodeOutlineColour	= colours[1] = settings.value("node/outlineColour").value<QColor>();
	selectedNodeTextColour		= colours[2] = settings.value("node/textColour").value<QColor>();
	selectedEdgeColour			= colours[3] = settings.value("edge/colour").value<QColor>();
	selectedEdgeTextColour		= colours[4] = settings.value("edge/textColour").value<QColor>();

	// Iterate over all saved colours
	for(int i=0; i<5; ++i) {

		// Colour not set
		bool set = false;

		// While not set
		for(int j=0; j<10 && !set; ++j) {

			// Get the colour in the list
			QColor compare = ColourUtils::GetColourFromID(j);

			// If they are equal, found
			if(compare == colours[i]) {
				comboBoxes[i]->setCurrentIndex(j);
				set = true;
			}
		}

		// If not set, must be custom
		if(!set) {
			comboBoxes[i]->setCurrentIndex(10);
		}
	}

	// Re-enable
	for(QComboBox* box : comboBoxes) {
		box->blockSignals(false);
	}

	// Set spin box to value
	fontSizeSpinBox->setValue(settings.value("font/size").toInt());

	// Set font
	QFont font = settings.value("font/font").value<QFont>();
	fontComboBox->setCurrentFont(font);

	// Update preview based on new values
	UpdateColours();
	UpdateFont();
}


void SettingsDialog::Accept() {
	// Update preferences
	QSettings settings;

	// Set all values based on current ones
	settings.setValue("node/fillColour", selectedNodeFillColour);
	settings.setValue("node/outlineColour", selectedNodeOutlineColour);
	settings.setValue("node/textColour", selectedNodeTextColour);

	settings.setValue("edge/colour", selectedEdgeColour);
	settings.setValue("edge/textColour", selectedEdgeTextColour);

	settings.setValue("font/size", fontSizeSpinBox->value());
	settings.setValue("font/font", fontComboBox->currentFont());


	// Set current values for this instance of the application
	// (otherwise restart required)

	// Set node colours
	Node::SetDefaultFillColour(selectedNodeFillColour);
	Node::SetDefaultOutlineColour(selectedNodeOutlineColour);
	Node::SetDefaultTextColour(selectedNodeTextColour);

	// Edge colours
	Edge::SetDefaultColour(selectedEdgeColour);
	Edge::SetDefaultTextColour(selectedEdgeTextColour);

	// Update both fonts
	QFont font = fontComboBox->currentFont();
	font.setPointSize(fontSizeSpinBox->value());

	Node::SetFont(font);
	Edge::SetFont(font);

	// Commit to file
	settings.sync();
	QDialog::accept();
}


void SettingsDialog::Reject() {
	QDialog::reject();
}


void SettingsDialog::UpdateFont() {
	// Get selected font
	QFont newFont = fontComboBox->currentFont();
	newFont.setPointSize(fontSizeSpinBox->value());

	graphicsView->scene()->removeItem(nodeOne);
	graphicsView->scene()->removeItem(nodeTwo);

	// Set new fonts
	Node::SetFont(newFont);
	Edge::SetFont(newFont);

	// Update nodes
	nodeOne->update();
	nodeTwo->update();

	// Reset positions
	nodeOne->setPos(0, 0);
	nodeTwo->setPos(0, 0);

	// Set font and colours
	edge->SetFont(newFont);
	edge->SetTextColour(selectedEdgeTextColour);
	edge->SetEdgeColour(selectedEdgeColour);

	// Set the position of node two to allow enough room
	// for the text + padding of 30. Child bounding rect will give us
	// label width as the edge will be 0 (nodes are both at (0,0))
	nodeTwo->setPos(nodeOne->boundingRect().width() + edge->childrenBoundingRect().width() + 30, 0);

	// Add items
	graphicsView->scene()->addItem(nodeOne);
	graphicsView->scene()->addItem(nodeTwo);
	// Re do edge position
	edge->UpdatePosition();
}


void SettingsDialog::UpdateColours() {
	// Set all colours to current values
	nodeOne->SetFillColour(selectedNodeFillColour);
	nodeTwo->SetFillColour(selectedNodeFillColour);

	nodeOne->SetOutlineColour(selectedNodeOutlineColour);
	nodeTwo->SetOutlineColour(selectedNodeOutlineColour);

	nodeOne->SetTextColour(selectedNodeTextColour);
	nodeTwo->SetTextColour(selectedNodeTextColour);

	edge->SetEdgeColour(selectedEdgeColour);
	edge->SetTextColour(selectedEdgeTextColour);
}


void SettingsDialog::UpdateNodeFillColour(const int &id) {
	selectedNodeFillColour = ColourUtils::GetColourFromID(id);
	UpdateColours();
}


void SettingsDialog::UpdateNodeOutlineColour(const int &id) {
	selectedNodeOutlineColour = ColourUtils::GetColourFromID(id);
	UpdateColours();
}


void SettingsDialog::UpdateNodeTextColour(const int &id) {
	selectedNodeTextColour = ColourUtils::GetColourFromID(id);
	UpdateColours();
}


void SettingsDialog::UpdateEdgeColour(const int &id) {
	selectedEdgeColour = ColourUtils::GetColourFromID(id);
	UpdateColours();
}


void SettingsDialog::UpdateEdgeTextColour(const int &id) {
	selectedEdgeTextColour = ColourUtils::GetColourFromID(id);
	UpdateColours();
}
