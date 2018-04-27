#include "JointObservationSelectionDialog.h"
#include "NumSortTreeWidgetItem.h"


JointObservationSelectionDialog::JointObservationSelectionDialog(QTreeWidget* widget, QWidget *parent) : QDialog(parent) {
	// Setup UI
	verticalLayout = new QVBoxLayout(this);
	treeWidget = widget;
	treeWidget->setParent(this);

	//treeWidget = new QTreeWidget(this);
	verticalLayout->addWidget(treeWidget);
	resize(500, 300);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	// Add the buttons
	verticalLayout->addWidget(buttonBox);

	// Connect the slots
	connect(buttonBox, &QDialogButtonBox::accepted, this, &JointObservationSelectionDialog::Accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &JointObservationSelectionDialog::Reject);

	// Double click acts as accept
	connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(Accept()));
	treeWidget->setCurrentItem(treeWidget->topLevelItem(0));
	treeWidget->resizeColumnToContents(1);
}


void JointObservationSelectionDialog::Accept() {
	// We can only have one selected, emit the JO Index
	emit SendJointObservation(treeWidget->selectedItems().first()->text(0).toInt());
	close();
}


void JointObservationSelectionDialog::Reject() {
	close();
}


JointObservationSelectionDialog::~JointObservationSelectionDialog() {
	//std::cout << "~JointObservationSelectionDialog()" << std::endl;
}
