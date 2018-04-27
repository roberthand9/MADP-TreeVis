#ifndef NUMSORTTREEWIDGETITEM_H
#define NUMSORTTREEWIDGETITEM_H

// Qt
#include <QTreeWidgetItem>

#include <iostream>

///
/// \brief Sorts tree widgets based on their numbers,
/// as opposed to the default lexical sort
///
class NumSortTreeWidgetItem : public QTreeWidgetItem {

	public:
		///
		/// \brief Constructor calls super class of QTreeWidgetItem
		/// \param parent The parent of the tree widger
		/// \param list The list of items, each of which will form a column.
		///
		NumSortTreeWidgetItem(QTreeWidget* parent, QStringList list);

		/// Destructor
		~NumSortTreeWidgetItem();

	private:
		///
		/// \brief Override Operator < to compare as doubles rather than lexical
		/// \param compare Item to compare the text to, this will sort them as numbers
		/// rather than text.
		/// \return The correct placement
		///
		bool operator<(const QTreeWidgetItem &compare) const;
};

#endif // NUMSORTTREEWIDGETITEM_H
