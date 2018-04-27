#include "GeneralUtils.h"


QColor ColourUtils::GetColourFromID(const int &id) {

	// Get the colour
	switch(id) {
		case Black:
			return QColor("black");
		case White:
			return QColor("white");
		case Red:
			return QColor("red");
		case Green:
			return QColor("green");
		case Blue:
			return QColor("blue");
		case LightBlue:
			return QColor("cyan");
		case Brown:
			return QColor::fromRgb(139,69,19);
		case Yellow:
			return QColor("yellow");
		case Orange:
			return QColor("orange");
		case Grey:
			return QColor("gray");
		case Custom: {
			// Create colour dialog
			QColorDialog dialog;
			dialog.setAttribute(Qt::WA_DeleteOnClose);
			QColor colour = dialog.getColor();

			// Return colour if chosen
			if(colour.isValid()) {
				return colour;
			} else {
				return QColor::Invalid;
			}
		}
	}

	// Default to black, shouldn't get here
	return QColor("black");
}
