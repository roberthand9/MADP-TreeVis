#ifndef JOINTOBSERVATIONSELECTIONDIALOG_H
#define JOINTOBSERVATIONSELECTIONDIALOG_H

// Qt
#include <QDialog>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QDialogButtonBox>

///
/// \brief The JointObservationSelectionDialog class allows a
/// dialog to be created that can be used to display the a
/// Tree Widget of joint observationsobservations and provides a
/// signal to connect to so that one can be selected.
/// Primarily used in the policy visualiser tool.
///
class JointObservationSelectionDialog : public QDialog {
	Q_OBJECT

	public:
		///
		/// \brief Constructor takes the given tree widget and displays it in the dialog.
		/// \param treeWidget The populated tree widget to display in the dialog.
		/// This is in the form JA Index | JA Name | Probability
		/// \param parent The parent of the dialog
		///
		explicit JointObservationSelectionDialog(QTreeWidget* treeWidget, QWidget *parent = 0);
		~JointObservationSelectionDialog();

	signals:
		///
		/// \brief Signal that can be used to get the selected JOI from the user.
		/// \param joIndex The index chosen by the user
		///
		void SendJointObservation(int joIndex);

	public slots:
		///
		/// \brief Default dialog accept slot when the user clicks OK.
		/// Also called when user double clicks an item
		///
		void Accept();

		///
		/// \brief Cancel button pressed, exit the dialog - no signal emitted.
		///
		void Reject();

	private:
		// Layout items
		QVBoxLayout* verticalLayout;
		QDialogButtonBox* buttonBox;

		/// The tree widget that is popular with the joint observations
		QTreeWidget* treeWidget;

};

#endif // JOINTOBSERVATIONSELECTIONDIALOG_H
