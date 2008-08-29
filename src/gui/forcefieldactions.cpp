/*
	*** Qt forcefield action functions
	*** src/gui/forcefieldactions.cpp
	Copyright T. Youngs 2007,2008

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

#include "gui/mainwindow.h"
#include "gui/forcefields.h"
#include "gui/gui.h"
#include "gui/minimiser.h"
#include "aten/aten.h"
#include "model/model.h"
#include "ff/forcefield.h"

// Local variables
bool updating_ = FALSE;

void AtenForm::on_actionMinimise_triggered(bool on)
{
	// Activate the slot for the 'Minimise' button on the minimiser window
	gui.minimiserWindow->doMinimisation();
}

void AtenForm::on_actionCalculateEnergy_triggered(bool on)
{
	Model *m = aten.current.rs;
	// Create expression for the current model
	if (!m->createExpression()) return;
	// Calculate total energy
	double energy = m->totalEnergy(m);
	// Print energy
	m->energy.print();
}

void AtenForm::on_actionCalculateForces_triggered(bool on)
{
	Model *m = aten.current.rs;
	// Create expression for the current model
	if (!m->createExpression()) return;
	// Calculate atomic forces
	m->calculateForces(m);
}

void AtenForm::refreshForcefieldCombo()
{
	if (forcefieldCombo_ == NULL) return;
	updating_ = TRUE;
	QStringList slist;
	int def = -1, n = 0;
	slist << "<No Forcefield>";
	for (Forcefield *ff = aten.forcefields(); ff != NULL; ff = ff->next)
	{
		n++;
		if (ff == aten.defaultForcefield()) def = n;
		slist << ff->name();
	}
	forcefieldCombo_->clear();
	forcefieldCombo_->addItems(slist);
	forcefieldCombo_->setEnabled( n == 0 ? FALSE : TRUE );
	// Select whichever forcefield is marked as the default
	if (def != -1) forcefieldCombo_->setCurrentIndex(def);
	else forcefieldCombo_->setCurrentIndex(0);
	updating_ = FALSE;
}

void AtenForm::forcefieldCombo_currentIndexChanged(int i)
{
	if (updating_) return;
	// Set the new default forcefield in the master and refresh the forcefields page
	Forcefield *ff = (i == 0 ? NULL : aten.forcefield(i-1));
	aten.setDefaultForcefield(ff);
	gui.forcefieldsWindow->refresh();
}
