#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

// TreeVis
#include "MainWindow.h"
#include "PlannerManager.h"
#include "FullTreeView.h"
#include "PolicyVisualiserView.h"

// Qt
#include <QApplication>
#include <QMenuBar>
#include <QAction>

QT_BEGIN_NAMESPACE

///
/// \brief The Ui_MainWindow class contains all of the UI elements
/// for the main window. This includes menu actions, layouts for the
/// top level layout of the application. It also connects
/// various slots to the MainWindow allowing them to communicate.
///
class Ui_MainWindow : public QObject {
	Q_OBJECT

	public:
		/// Main Widget
		QWidget* main;

		/// The applications full tree viewer tool
		FullTreeView* fullTreeViewer;

		/// The applications policy visualiser tool
		PolicyVisualiserView* policyVisualiserView;

		/// Main menu bar
		QMenuBar* menuBar;

		/// The file menu on the main meny bar
		QMenu* fileMenu;

		/// Action for a new plan, connected to main window
		QAction* actionNewPlan;

		/// Action to load a saved policy from a file, connected to main window
		QAction* actionLoadSavedPolicy;

		/// Action to save the full tree viewer to a file, connected to main window
		QAction* actionSaveFullTreeViewerScreenToImage;

		/// Action to save the policy visualiser viewer to a file, connected to main window
		QAction* actionSavePolicyVisualiserScreenToImage;

		/// Action to set default settings
		QAction* actionSetSettings;

		/// Layout to expand all
		QHBoxLayout* horizontalLayout;

		/// Splitter between tab widget and text output area
		QSplitter* mainWindowSplitter;

		/// Tab widget to hold the two tabs
		QTabWidget* tabWidget;

		/// Holds the full tree viewer widget
		QWidget* FTVTab;

		/// Holds the policy visualiser widgert
		QWidget* PVTab;

		/// Text area at bottom of application used for output
		QTextEdit* informationTextOutput;

		///
		/// \brief Sets up the main application UI including creating both the PV and FTV
		/// components.
		/// \param MainWindow
		/// \param pManager
		///
		void SetupUi(QMainWindow* MainWindow, PlannerManager* pManager) {
			MainWindow->resize(1000, 1000);
			MainWindow->setWindowTitle(QApplication::applicationName());

			// Create container widget
			main = new QWidget(MainWindow);
			main->setMinimumSize(QSize(500, 500));

			// Create main horizontal layout
			horizontalLayout = new QHBoxLayout(main);

			// Splits the main window between the tab widget and the information text area
			mainWindowSplitter = new QSplitter(main);
			mainWindowSplitter->setOrientation(Qt::Vertical);

			// Create tab widget and size policy
			tabWidget = new QTabWidget(mainWindowSplitter);

			QSizePolicy tabWidgetSizePol(QSizePolicy::Expanding, QSizePolicy::Expanding);
			tabWidgetSizePol.setVerticalStretch(1);
			tabWidget->setSizePolicy(tabWidgetSizePol);
			tabWidget->setStyleSheet(QLatin1String("QTabWidget::tab-bar {width: 9999px;}"));

			// Create tab for full tree viewer
			FTVTab = new QWidget();
			fullTreeViewer = new FullTreeView(pManager, FTVTab);
			tabWidget->addTab(FTVTab, "Full Tree Viewer");

			// Create the policy visualiser tab
			PVTab = new QWidget();
			policyVisualiserView = new PolicyVisualiserView(pManager, PVTab);
			tabWidget->addTab(PVTab, "Policy Visualiser");

			// Add the tab widget to the main window splitter
			mainWindowSplitter->addWidget(tabWidget);

			// Create the information text output, set the size policy
			informationTextOutput = new QTextEdit(mainWindowSplitter);
			informationTextOutput->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
			informationTextOutput->setReadOnly(true);

			// Add the widget to the splitter, add the horizontal layout to the main window
			// then set the main windows main widget to be our main widget
			mainWindowSplitter->addWidget(informationTextOutput);
			horizontalLayout->addWidget(mainWindowSplitter);
			MainWindow->setCentralWidget(main);

			// Create a new menu bar
			menuBar = new QMenuBar(MainWindow);

			// Create menu actions
			actionNewPlan = new QAction("New Plan", MainWindow);
			actionLoadSavedPolicy = new QAction("Load Saved Policy", MainWindow);
			actionSaveFullTreeViewerScreenToImage = new QAction("Save Full Tree Viewer Screen to file", MainWindow);
			actionSavePolicyVisualiserScreenToImage = new QAction("Save Policy Visualiser Screen to file", MainWindow);
			actionSetSettings = new QAction("Settings", MainWindow);

			// New file menu in menu bar
			fileMenu = new QMenu("File", menuBar);
			MainWindow->setMenuBar(menuBar);

			// Add actions to the menu bar
			menuBar->addAction(fileMenu->menuAction());
			fileMenu->addAction(actionNewPlan);
			fileMenu->addAction(actionLoadSavedPolicy);
			fileMenu->addAction(actionSaveFullTreeViewerScreenToImage);
			fileMenu->addAction(actionSavePolicyVisualiserScreenToImage);
			fileMenu->addAction(actionSetSettings);

			// Connect the slots up
			ConnectSlots(MainWindow);
		}

	private:
		/// Connects the main signals for the widgets onto the main windows slots
		void ConnectSlots(QMainWindow* MainWindow) {
			// Connect menu actions
			connect(actionNewPlan, SIGNAL(triggered(bool)), MainWindow, SLOT(ActionNewPlan()));
			connect(actionLoadSavedPolicy, SIGNAL(triggered(bool)), MainWindow, SLOT(ActionLoadSavedPolicy()));
			connect(actionSaveFullTreeViewerScreenToImage, SIGNAL(triggered(bool)), fullTreeViewer, SLOT(SaveGraphicsViewToFile()));
			connect(actionSavePolicyVisualiserScreenToImage, SIGNAL(triggered(bool)), policyVisualiserView, SLOT(SaveGraphicsViewToFile()));
			connect(actionSetSettings, SIGNAL(triggered(bool)), MainWindow, SLOT(ActionSetSettings()));

			// Connect slot to allow the components to output to the info text field
			connect(fullTreeViewer, SIGNAL(AppendToInformationText(std::string, MainWindow::textStyle)), MainWindow, SLOT(AppendToInformationText(std::string, MainWindow::textStyle)));
			connect(policyVisualiserView, SIGNAL(AppendToInformationText(std::string, MainWindow::textStyle)), MainWindow, SLOT(AppendToInformationText(std::string, MainWindow::textStyle)));

			// Connect slots to inform viewers when plans have started/finished
			connect(MainWindow, SIGNAL(PlanStarting()), fullTreeViewer, SLOT(PlanStarting()));
			connect(MainWindow, SIGNAL(PlanFinished()), fullTreeViewer, SLOT(PlanFinished()));
			connect(MainWindow, SIGNAL(PlanStarting()), policyVisualiserView, SLOT(PlanStarting()));
			connect(MainWindow, SIGNAL(PlanFinished()), policyVisualiserView, SLOT(PlanFinished()));
		}
};

namespace Ui {
	class MainWindow: public Ui_MainWindow {};
}

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
