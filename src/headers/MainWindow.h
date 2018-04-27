#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Treevis files
#include "PlannerManager.h"

// Qt
#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QFutureWatcher>

namespace Ui {
	class MainWindow;
}

///
/// \brief MainWindow is the applications way of being
/// created and used. It holds references to all of
/// the needed interface components and owns the
/// planner manager instance needed to make use of the toolbox.
///
class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		///
		/// \brief Constructor sets up the UI.
		/// \param parent
		///
		explicit MainWindow(QWidget* parent = 0);

		/// Deletes the UI
		~MainWindow();

		///
		/// \brief Colours of text for the text area output
		/// at the bottom of the application
		///
		enum textStyle {
			Red,
			Green,
			Orange,
			Normal
		};

	signals:
		///
		/// \brief Signal emitted when a live plan is starting
		/// or a previous plan is being read in
		///
		void PlanStarting();

		///
		/// \brief Signal emitted when a live plan has finished
		/// or a previous plan has been read in
		///
		void PlanFinished();

	public slots:
		///
		/// \brief Appends the string to the information text area
		/// at the bottom of the screen
		/// \param text The string to append
		/// \param style The MainWindow::textStyle style of the text
		///
		void AppendToInformationText(const std::string& text,
									 const MainWindow::textStyle& style = Normal);

		///
		/// \brief Slot that allows the user to specify a previous plan to read in
		/// \param policyFilePath The path to the policy
		/// file ouputted from a previous plan
		/// \param args The arguments given, used to instantiate a null planner
		///
		void PreviousPlan(std::string policyFilePath,
						  ArgumentHandlers::Arguments args);

		///
		/// \brief Starts a plan with the given args using the planner manager.
		/// This slot will be called from StartPlanWizard that constructs
		/// the arguments from the users options
		/// \param type The planner type to use
		/// \param args The arguments from the wizard to pass to the planner
		///
		void StartPlan(PlannerManager::PlannerType type,
					   ArgumentHandlers::Arguments args);

		/// Slot called when new plan action on menu bar clicked
		void ActionNewPlan();

		///
		/// \brief Slot called when load save policy action on menu
		/// bar clicked.
		///
		void ActionLoadSavedPolicy();

		/// Slot called when settings action on menu bar clicked
		void ActionSetSettings();

		///
		/// \brief PlanEnded Slot called when a live plan has ended
		/// \param success True if the plan was successful, false if failed
		/// \param errorMessage Error message if the plan failed
		///
		void PlanEnded(bool success, QString errorMessage);

		///
		/// \brief PlanEnded Slot called when a
		/// previous policy finished being read in
		/// \param success True if reading in was successful, false if failed
		/// \param errorMessage Error message if the
		/// policy could not be read in failed
		///
		void PreviousPlanEnded(bool success, QString errorMessage);

	private:
		/// The UI
		Ui::MainWindow* ui;

		/// Messagebox used to inform the user a plan is taking place
		QMessageBox* informationMessageBox;

		/// The Planner Manager used to plan and used by the viewers
		std::unique_ptr<PlannerManager> pManager = 0;
};

#endif // MAINWINDOW_H
