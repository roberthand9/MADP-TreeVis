#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// TreeVis
#include "Node.h"
#include "Edge.h"

// Qt
#include <QDialog>
#include <QSettings>
#include <QGraphicsView>
#include <QComboBox>
#include <QFontComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>

///
/// \brief The SettingsDialog class provides a dialog window
/// to the user in order to set default settings about the
/// look of the program. Namely the style of the edges and
/// nodes including the colour and fonts. These are saved
/// using QSettings in order to be persistent
/// across restarts of the application
///
class SettingsDialog : public QDialog {

	public:
		///
		/// \param parent The parent widget
		///
		SettingsDialog(QWidget* parent);
		~SettingsDialog();

	private slots:
		///
		/// \brief Slot called when the node fill colour is updated
		/// \param id The id of the colour selected
		///
		void UpdateNodeFillColour(const int &id);

		///
		/// \brief Slot called when node outline colour is updated
		/// \param id The id of the colour selected
		///
		void UpdateNodeOutlineColour(const int &id);

		///
		/// \brief Slot called when the node text colour is updated
		/// \param id The id of the colour selected
		///
		void UpdateNodeTextColour(const int &id);

		///
		/// \brief Slot called when edge colour is updated
		/// \param id The id of the colour selected
		///
		void UpdateEdgeColour(const int &id);

		///
		/// \brief Slot called when edge text colour is updated
		/// \param id The id of the colour selected
		///
		void UpdateEdgeTextColour(const int & id);

		/// Slot called when the font is updated
		void UpdateFont();

		/// \brief Slot called when accept button clicked, saves
		/// settings to persistent storage
		void Accept();

		/// Slot called when cancel button pressed
		void Reject();

	private:
		/// \brief Loads current settings to the window
		/// or uses defaults if not set
		void LoadSettings();

		/// Updates the colours in the preview
		void UpdateColours();

		// Accept & Cancel buttons
		QDialogButtonBox* buttonBox;

		// Live preview
		QGraphicsView* graphicsView;

		// Selection combo boxes
		QComboBox* nodeFillColourComboBox;
		QComboBox* nodeOutlineColourComboBox;
		QComboBox* nodeTextColourComboBox;

		QComboBox* edgeColourComboBox;
		QComboBox* edgeTextColourComboBox;

		QFontComboBox* fontComboBox;
		QSpinBox* fontSizeSpinBox;

		// Live preview items
		Node* nodeOne;
		Node* nodeTwo;
		Edge* edge = nullptr;

		// Currently selected colours in the boxes
		QColor selectedNodeFillColour;
		QColor selectedNodeOutlineColour;
		QColor selectedNodeTextColour;

		QColor selectedEdgeColour;
		QColor selectedEdgeTextColour;
};

#endif // SETTINGSDIALOG_H
