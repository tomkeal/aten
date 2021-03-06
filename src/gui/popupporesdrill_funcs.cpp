/*
	*** Popup Widget - Pores Drill
	*** src/gui/popupporesdrill_funcs.cpp
	Copyright T. Youngs 2007-2016

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

#include "gui/popupporesdrill.h"
#include "main/aten.h"
#include "gui/mainwindow.h"
#include "base/namespace.h"

ATEN_USING_NAMESPACE

// Constructor
PoresDrillPopup::PoresDrillPopup(AtenWindow& parent, TMenuButton* buttonParent) : TPopupWidget(buttonParent), parent_(parent)
{
	// Set up interface
	ui.setupUi(this);
}

// Update controls (before show()) (virtual)
void PoresDrillPopup::updateControls()
{
	refreshing_ = true;

	// Disable buttons if there is no unit cell in the current model
	Model* currentModel = parent_.aten().currentModelOrFrame();
	ui.PoreSelectAndCutButton->setEnabled(currentModel);
	ui.PoreSelectButton->setEnabled(currentModel);

	refreshing_ = false;
}

// Call named method associated to popup
bool PoresDrillPopup::callMethod(QString methodName, ReturnValue& rv)
{
	bool result = true;
	if (methodName == "TEST") return true;
	else if (methodName == "hideEvent")
	{
		return true;
	}
	else
	{
		printf("No method called '%s' is available in this popup.\n", qPrintable(methodName));
		result = false;
	}
	return result;
}

/*
 * Widget Functions
 */

void PoresDrillPopup::on_PoreSelectButton_clicked(bool checked)
{
	// First check - does the current model have a unit cell?
	Model* currentModel = parent_.aten().currentModelOrFrame();
	if (currentModel->cell().type() == UnitCell::NoCell)
	{
		Messenger::print("Can't drill pores in a non-periodic model.");

		// Update the main window
		parent_.updateWidgets();

		return;
	}

	// Grab some values so we are ready to run the command
	int nx = ui.ArrayXSpin->value(), ny = ui.ArrayYSpin->value();
	int face = ui.OriginPlaneCombo->currentIndex()+1;
	Vec3<double> v(ui.PoreVectorX->value(), ui.PoreVectorY->value(), ui.PoreVectorZ->value());
	QString geometry = ui.PoreGeometryCombo->currentText();
	double sizeParam = ui.PoreSizeSpin->value();

	// Run the command
	CommandNode::run(Commands::SelectPores, "cdiiiddd", qPrintable(geometry), sizeParam, nx, ny, face, v.x, v.y, v.z);

	// Update the main window
	parent_.updateWidgets();
}

void PoresDrillPopup::on_PoreSelectAndCutButton_clicked(bool checked)
{
	// First check - does the current model have a unit cell?
	Model* currentModel = parent_.aten().currentModelOrFrame();
	if (currentModel->cell().type() == UnitCell::NoCell)
	{
		Messenger::print("Can't drill pores in a non-periodic model.");

		// Update the main window
		parent_.updateWidgets();

		return;
	}

	// Grab some values so we are ready to run the command
	int nx = ui.ArrayXSpin->value(), ny = ui.ArrayYSpin->value();
	int face = ui.OriginPlaneCombo->currentIndex()+1;
	Vec3<double> v(ui.PoreVectorX->value(), ui.PoreVectorY->value(), ui.PoreVectorZ->value());
	QString geometry = ui.PoreGeometryCombo->currentText();
	double sizeParam = ui.PoreSizeSpin->value();

	// Run the command
	CommandNode::run(Commands::DrillPores, "cdiiiddd", qPrintable(geometry), sizeParam, nx, ny, face, v.x, v.y, v.z);

	// Update the main window
	parent_.updateWidgets();
}
