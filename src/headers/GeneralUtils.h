#ifndef GENERALUTILS_H
#define GENERALUTILS_H

#include <QStringList>
#include <QColor>
#include <QColorDialog>

///
/// \brief Provide utilities relating to colours.
///
namespace ColourUtils {

	/// Available colours to use for new colour slots
	enum Colours {
		Black,
		White,
		Red,
		Green,
		Blue,
		LightBlue,
		Brown,
		Yellow,
		Orange,
		Grey,
		Custom
	};

	/// \brief Colours available. These should match
	/// the Colours enum
	const QStringList colourList =
			QStringList() << "Black"
						  << "White"
						  << "Red"
						  << "Green"
						  << "Blue"
						  << "Light Blue"

						  << "Brown"
						  << "Yellow"
						  << "Orange"
						  << "Grey"

						  << "Custom";

	///
	/// \brief GetColourFromID Gets the colour from the
	/// index ID associated with it in the colours enum
	/// \param id The index (id) of the colour to get
	/// \return A QColour of the colour
	///
	QColor GetColourFromID(const int &id);
}


#endif // GENERALUTILS_H
