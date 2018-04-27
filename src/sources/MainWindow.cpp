#include "MainWindow.h"

// Include TreeVis Files
#include "UIMainWindow.h"
#include "StartPlanWizard.h"
#include "PreviousPlanWizard.h"
#include "SettingsDialog.h"

// Smart pointers
#include <memory>

// Include Qt Filess
#include <QFileDialog>
#include <QUrl>
#include <QLineEdit>
#include <QDir>
#include <QThread>
#include <QSettings>
#include <QtConcurrent/QtConcurrentRun>

// Include other files
#include <iostream>

// Create new UI When instantiated
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	// Create a new planner manager
	pManager = std::unique_ptr<PlannerManager>(new PlannerManager());

	// Connect slots for finished events
	connect(pManager.get(), &PlannerManager::PlanEnded, this, &MainWindow::PlanEnded); // SIGNAL(PlanSucceeded()), this, SLOT(PlanSucceeded()));
	connect(pManager.get(), &PlannerManager::PreviousPlanEnded, this, &MainWindow::PreviousPlanEnded);

	// Setup the UI
	ui->SetupUi(this, pManager.get());

	// Create message box for use when planning
	informationMessageBox = new QMessageBox(this);
	informationMessageBox->setStandardButtons(0);
	informationMessageBox->setWindowFlags(Qt::Dialog);

	QSettings settings;

	// If settings have been created
	if(settings.contains("node/fillColour")) {
		Node::SetDefaultFillColour(settings.value("node/fillColour").value<QColor>());
		Node::SetDefaultOutlineColour(settings.value("node/outlineColour").value<QColor>());
		Node::SetDefaultTextColour(settings.value("node/textColour").value<QColor>());

		Edge::SetDefaultColour(settings.value("edge/colour").value<QColor>());
		Edge::SetDefaultTextColour(settings.value("edge/textColour").value<QColor>());

		QFont font = settings.value("font/font").value<QFont>();
		font.setPointSize(settings.value("font/size").toInt());

		Node::SetFont(font);
		Edge::SetFont(font);
	} else {
		// Otherwise use hard coded defaults
		Node::SetDefaultFillColour(QColor("white"));
		Node::SetDefaultOutlineColour(QColor("black"));
		Node::SetDefaultTextColour(QColor("black"));

		Edge::SetDefaultColour(QColor("black"));
		Edge::SetDefaultTextColour(QColor("black"));

		QFont font = QFont("Helvetica", 12);
		Node::SetFont(font);
		Edge::SetFont(font);
	}
}


MainWindow::~MainWindow() {
	// std::cout << "~MainWindow()" << std::endl;
	delete ui;
}


void MainWindow::ActionNewPlan() {
	// Show wizard, connecting signal to this
	StartPlanWizard* wizard = new StartPlanWizard(this);
	connect(wizard, &StartPlanWizard::StartPlan, this, &MainWindow::StartPlan);
	wizard->setModal(true);
	wizard->setAttribute(Qt::WA_DeleteOnClose);
	wizard->show();
}


void MainWindow::ActionLoadSavedPolicy() {
	// Show wizard, connecting signal to this
	PreviousPlanWizard* wizard = new PreviousPlanWizard(this);
	connect(wizard, &PreviousPlanWizard::PreviousPlan, this, &MainWindow::PreviousPlan);
	wizard->setModal(true);
	wizard->setAttribute(Qt::WA_DeleteOnClose);
	wizard->show();
}


void MainWindow::ActionSetSettings() {
	SettingsDialog* dialog = new SettingsDialog(this);
	dialog->setModal(true);
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	// Get result
	int result = dialog->exec();

	// If new settings, reset visualiser tools to use new values
	if(result == QDialog::Accepted) {

		// If planned reset
		if(pManager->HasPlanned()) {
			emit PlanStarting();
			emit PlanFinished();
		}

		AppendToInformationText("Settings saved", Green);
	}
}


void MainWindow::StartPlan(PlannerManager::PlannerType type, ArgumentHandlers::Arguments args) {
	// Will reset everything
	emit PlanStarting();

	// Start the plan
	AppendToInformationText("Starting new plan with given arguments");

	informationMessageBox->setWindowTitle("Planning");
	informationMessageBox->setText("The plan is currently running...");
	informationMessageBox->show();

	// Start the computation
	QtConcurrent::run(pManager.get(), &PlannerManager::Plan, type, args);
}


void MainWindow::PlanEnded(bool success, QString errorMessage) {
	informationMessageBox->hide();
	QApplication::processEvents(); // Ensure hidden

	// If the plan was a success
	if(success) {
		AppendToInformationText("Plan finished", Green);
		emit PlanFinished();
	} else {

		// Otherwise an error was thrown
		AppendToInformationText("Plan did not succeed: " + errorMessage.toStdString(), Red);
		QMessageBox::critical(this, "MADP Tree Vis", errorMessage);
	}
}


void MainWindow::PreviousPlan(std::string policyFilePath, ArgumentHandlers::Arguments args) {
	// Will reset everything
	emit PlanStarting();

	informationMessageBox->setWindowTitle("Loading Policy");
	informationMessageBox->setText("The policy is currently being loaded...");
	informationMessageBox->show();

	AppendToInformationText("Attempting to load policy from " + policyFilePath + " with given arguments");
	QApplication::processEvents();

	// Load policy in separate thread
	QtConcurrent::run(pManager.get(), &PlannerManager::PreviousPlan, policyFilePath, args);
}


void MainWindow::PreviousPlanEnded(bool success, QString errorMessage) {
	informationMessageBox->hide();
	QApplication::processEvents(); // Ensure hidden

	// If reading in the previous plan succeeded
	if(success) {
		emit PlanFinished();
		AppendToInformationText("Previous plan loaded", Green);
	} else {

		// Otherwise it failed
		AppendToInformationText("Failed to load previous plan: " + errorMessage.toStdString(), Red);
		QMessageBox::critical(this, "MADP Tree Vis", errorMessage);
	}
}


void MainWindow::AppendToInformationText(const std::string& text, const textStyle& style) {
	// Move to the top
	ui->informationTextOutput->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
	QString toAppend = "<span style=";

	// Add the colour if necessary
	switch(style) {

		case Red:
			toAppend += "\"color:red\">";
			break;
		case Orange:
			toAppend += "\"color:orange\">";
			break;
		case Green:
			toAppend += "\"color:darkgreen\">";
			break;
		default:
			toAppend += "\"\">";
			break;
	}

	// Add the text and close the span
	toAppend += QString::fromStdString(text);
	toAppend += "</span><br>";

	// Insert (at the top)
	ui->informationTextOutput->insertHtml(toAppend);
}
