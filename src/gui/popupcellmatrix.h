/*
	*** Popup Widget - Cell Matrix
	*** src/gui/popupcellmatrix.h
	Copyright T. Youngs 2007-2015

	This file is part of Aten.

	Aten is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Aten is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Aten.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ATEN_CELLMATRIXPOPUP_H
#define ATEN_CELLMATRIXPOPUP_H

#include "gui/ui_popupcellmatrix.h"
#include "gui/tmenubutton.hui"

// Forward Declarations (Qt)
class AtenWindow;

// Popup Widget - Cell Matrix
class CellMatrixPopup : public TMenuButtonPopupWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Reference to main window
	AtenWindow& parent_;

	public:
	// Constructor / Destructor
	CellMatrixPopup(AtenWindow& parent, TMenuButton* buttonParent);
	// Main form declaration
	Ui::CellMatrixPopup ui;
	// Show popup, updating any controls as necessary beforehand
	void popup();
	// Call named method associated to popup
	bool callMethod(QString methodName);


	/*
	 * Reimplementations
	 */
	protected:
	void hideEvent(QHideEvent* event) { TMenuButtonPopupWidget::hideEvent(event); }


	/*
	 * Widget Functions
	 */
	private:
	// Apply current matrix to current model
	void setCurrentMatrix();

	private slots:
	void on_AxisAXSpin_valueChanged(double value);
	void on_AxisAYSpin_valueChanged(double value);
	void on_AxisAZSpin_valueChanged(double value);
	void on_AxisBXSpin_valueChanged(double value);
	void on_AxisBYSpin_valueChanged(double value);
	void on_AxisBZSpin_valueChanged(double value);
	void on_AxisCXSpin_valueChanged(double value);
	void on_AxisCYSpin_valueChanged(double value);
	void on_AxisCZSpin_valueChanged(double value);
};

#endif
