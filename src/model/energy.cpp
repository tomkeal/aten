/*
	*** Model energy and force calculation
	*** src/model/energy.cpp
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

#include "model/model.h"
#include "classes/pattern.h"
#include "classes/energystore.h"
#include "classes/forcefield.h"
#include "classes/fourier.h"
#include "base/prefs.h"
#include "base/elements.h"

// Calculate total energy of model (from supplied coordinates)
double Model::totalEnergy(Model *srcmodel)
{
	dbgBegin(Debug::Calls,"Model::totalEnergy");
	// Check the expression validity
	if (!isExpressionValid())
	{
		msg(Debug::None,"Model::totalEnergy - No valid energy expression defined for model.\n");
		dbgEnd(Debug::Calls,"Model::totalEnergy");
		return 0.0;
	}
	// Clear the energy store
	energy.clear();
	// Cycle through patterns, calculating the contributions from each
	Pattern *p, *p2;
	p = patterns_.first();
	// Calculate VDW correction
	if (prefs.calculateVdw() && (cell_.type() != CT_NONE)) p->vdwCorrectEnergy(&cell_, &energy);
	// Prepare Ewald (if necessary)
	ElecMethod emodel = prefs.electrostaticsMethod();
	if (prefs.calculateElec())
	{
		if (emodel == EM_EWALDAUTO) prefs.estimateEwaldParameters(&srcmodel->cell_);
		// Create the fourier space for use in the Ewald sum
		if (emodel != EM_COULOMB) fourier.prepare(srcmodel,prefs.ewaldKvec());
	}
	while (p != NULL)
	{
		// Intramolecular Interactions
		if (prefs.calculateIntra())
		{
			p->bondEnergy(srcmodel, &energy);
			p->angleEnergy(srcmodel, &energy);
			p->torsionEnergy(srcmodel, &energy);
		}
		// Van der Waals Interactions
		if (prefs.calculateVdw())
		{
			p->vdwIntraPatternEnergy(srcmodel, &energy);
			for (p2 = p; p2 != NULL; p2 = p2->next)
				p->vdwInterPatternEnergy(srcmodel, p2, &energy);
		}
		// Electrostatic Interactions
		if (prefs.calculateElec())
		{
			switch (emodel)
			{
				case (EM_OFF):
					msg(Debug::None,"Electrostatics requested but no method of calculation chosen!\n");
					break;
				case (EM_COULOMB):
					p->coulombIntraPatternEnergy(srcmodel,&energy);
					p2 = p;
					while (p2 != NULL)
					{
						p->coulombInterPatternEnergy(srcmodel,p2,&energy);
						p2 = p2->next;
					}
					break;
				default: // Ewald
					p->ewaldRealIntraPatternEnergy(srcmodel,&energy);
					p->ewaldCorrectEnergy(srcmodel,&energy);
					p2 = p;
					while (p2 != NULL)
					{
						p->ewaldRealInterPatternEnergy(srcmodel,p2,&energy);
						p2 = p2->next;
					}
					// Calculate reciprocal space part (called once from first pattern only)
					if (p == patterns_.first())
						p->ewaldReciprocalEnergy(srcmodel,p,patterns_.nItems(),&energy);
					break;
			}
		}
		p = p->next;
	}
	energy.totalise();
	dbgEnd(Debug::Calls,"Model::totalEnergy");
	return energy.total();
}

// Calculate total interaction energy of specified molecule with remainder of model
double Model::moleculeEnergy(Model *srcmodel, Pattern *molpattern, int molecule)
{
	dbgBegin(Debug::Calls,"Model::moleculeEnergy");
	// Check the expression validity
	if (!isExpressionValid())
	{
		msg(Debug::None,"Model::moleculeEnergy - No valid energy expression defined for model.\n");
		dbgEnd(Debug::Calls,"Model::moleculeEnergy");
		return 0.0;
	}
	// Clear the energy store
	energy.clear();
	Pattern *p;
	// Prepare Ewald (if necessary)
	ElecMethod emodel = prefs.electrostaticsMethod();
	if (prefs.calculateElec())
	{
		if (emodel == EM_EWALDAUTO) prefs.estimateEwaldParameters(&srcmodel->cell_);
		// Create the fourier space for use in the Ewald sum
		if (emodel != EM_COULOMB) fourier.prepare(srcmodel,prefs.ewaldKvec());
	}
	// Calculate VDW interactions between 'molecule' in pattern 'molpattern' and molecules in it and other's patterns
	for (p = patterns_.first(); p != NULL; p = p->next)
		molpattern->vdwInterPatternEnergy(srcmodel, p, &energy, molecule);
	// Electrostatic Interactions between 'molecule' in pattern 'molpattern' and molecules in it and other's patterns
	if (prefs.calculateElec())
	{
		switch (emodel)
		{
			case (EM_OFF):
				msg(Debug::None,"Electrostatics requested but no method of calculation chosen!\n");
				break;
			case (EM_COULOMB):
				for (p = patterns_.first(); p != NULL; p = p->next) molpattern->coulombInterPatternEnergy(srcmodel,p,&energy);
				break;
			default: // Ewald
				for (p = patterns_.first(); p != NULL; p = p->next) p->ewaldRealInterPatternEnergy(srcmodel,p,&energy);
				// Calculate reciprocal space part (called once from first pattern only)
				if (p == patterns_.first())
					p->ewaldReciprocalEnergy(srcmodel,p,patterns_.nItems(),&energy);
				break;
		}
	}
	energy.totalise();
	dbgEnd(Debug::Calls,"Model::moleculeEnergy");
	return energy.total();
}

// Calculate forces from specified config
void Model::calculateForces(Model *srcmodel)
{
	// Calculate the forces for the atoms of 'srcmodel' from the expression defined in the *this model
	dbgBegin(Debug::Calls,"Model::calculateForces");
	// Check the expression validity
	if (!isExpressionValid())
	{
		msg(Debug::None,"calculateForces : No valid energy expression defined for model.\n");
		dbgEnd(Debug::Calls,"Model::calculateForces");
		return;
	}
	srcmodel->zeroForces();
	// Cycle through patterns, calculate the intrapattern forces for each
	Pattern *p, *p2;
	p = patterns_.first();
	// Prepare Ewald (if necessary)
	ElecMethod emodel = prefs.electrostaticsMethod();
	if (prefs.calculateElec())
	{
		if (emodel == EM_EWALDAUTO) prefs.estimateEwaldParameters(&srcmodel->cell_);
		// Create the fourier space for use in the Ewald sum
		if (emodel != EM_COULOMB) fourier.prepare(srcmodel,prefs.ewaldKvec());
	}
	while (p != NULL)
	{
		// Bonded Interactions
		if (prefs.calculateIntra())
		{
			p->bondForces(srcmodel);
			p->angleForces(srcmodel);
			p->torsionForces(srcmodel);
		}
		// VDW
		if (prefs.calculateVdw())
		{
			p->vdwIntraPatternForces(srcmodel);
			Pattern *p2 = p;
			while (p2 != NULL)
			{
				p->vdwInterPatternForces(srcmodel,p2);
				p2 = p2->next;
			}
		}
		// Electrostatics
		if (prefs.calculateElec())
		{
			switch (emodel)
			{
				case (EM_OFF):
					msg(Debug::None,"Electrostatics requested but no method of calculation chosen!\n");
					break;
				case (EM_COULOMB):
					//p->coulombInterPatternForces(xxcfg);
					//p2 = p;
					//while (p2 != NULL)
				//	{
				//		p->coulomb_intrapatternEnergy(xcfg,p2);
				//		p2 = p2->next;
				//	}
					break;
				default: // Ewald
					p->ewaldRealIntraPatternForces(srcmodel);
					p->ewaldCorrectForces(srcmodel);
					p2 = p;
					while (p2 != NULL)
					{
						p->ewaldRealInterPatternForces(srcmodel,p2);
						p2 = p2->next;
					}
					// Calculate reciprocal space part (called once from first pattern only)
					if (p == patterns_.first()) p->ewaldReciprocalForces(srcmodel);
					break;
			}
		}
		p = p->next;
	}
	dbgEnd(Debug::Calls,"Model::calculateForces");
}
