#include "NumSortTreeWidgetItem.h"


NumSortTreeWidgetItem::NumSortTreeWidgetItem(QTreeWidget *parent,
											 QStringList list) :
											 QTreeWidgetItem(parent, list) {}


NumSortTreeWidgetItem::~NumSortTreeWidgetItem() {
	//std::cout << "~NumSortTreeWidgetItem()" << std::endl;
}


bool NumSortTreeWidgetItem::operator<(const QTreeWidgetItem &compare) const {
	// Get sort column requested
	int col = treeWidget()->sortColumn();

	// Get the strings to be sorted on
	QString first(text(col));
	QString second(compare.text(col));

	// Strip out % symbols
	first.replace("%", "");
	second.replace("%", "");

	// Return sort order based on double values
	// rather than lexical
	return (first.toDouble() < second.toDouble());
}
