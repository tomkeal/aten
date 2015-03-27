/*
	*** Primitive Set
	*** src/render/primitiveset.cpp
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

#include "render/primitiveset.h"
#include "base/prefs.h"

ATEN_USING_NAMESPACE

// Constructor
PrimitiveSet::PrimitiveSet()
{
	// Adjustments
	sphereAtomAdjustment_ = 1.0;
	scaledAtomAdjustments_.createEmpty(Elements().nElements());

	// Primitives
	requestedQuality_ = -1;
	currentQuality_ = -1;
	nInstances_ = 0;
}

// Destructor
PrimitiveSet::~PrimitiveSet()
{
}

/*
 * Adjustments for primitives
 */

// Recalculate adjustments
void PrimitiveSet::calculateAdjustments()
{
	// Recalculate adjustments for bond positioning
	double atomradius, bondradius, theta;
	int i;

	// Triangle formed between atom radius (H), bond radius (O), and unknown (A)
	// Determine angle between H and O and calculate adjustment (=H-A)

	// Sphere Style
	atomradius = prefs.atomStyleRadius(Prefs::SphereStyle);
	bondradius = prefs.bondStyleRadius(Prefs::SphereStyle);
	theta = asin(bondradius / atomradius);
	sphereAtomAdjustment_ = atomradius - atomradius*cos(theta);

	// Scaled Style
	theta = asin(bondradius / atomradius);
	sphereAtomAdjustment_ = atomradius - atomradius*cos(theta);
	for (i = 0; i<Elements().nElements(); ++i)
	{
		atomradius = prefs.atomStyleRadius(Prefs::ScaledStyle) * Elements().el[i].atomicRadius;
		theta = asin(bondradius / atomradius);
		scaledAtomAdjustments_[i] = (atomradius - atomradius*cos(theta));
	}
}

// Return current sphereAtomAdjustment
double PrimitiveSet::sphereAtomAdjustment()
{
	return sphereAtomAdjustment_;
}

// Return current sphereAtomAdjustment
double PrimitiveSet::scaledAtomAdjustment(int element)
{
	return scaledAtomAdjustments_[element];
}

/*
 * Primitives
 */

// Return atom primitive
Primitive& PrimitiveSet::atom()
{
	return atom_;
}

// Return selected atom primitive
Primitive& PrimitiveSet::selectedAtom()
{
	return selectedAtom_;
}

// Return bond primitive
Primitive& PrimitiveSet::bond(Prefs::DrawStyle drawStyle, Bond::BondType bondType)
{
	return bonds_[drawStyle][bondType];
}

// Return selected bond primitive
Primitive& PrimitiveSet::selectedBond(Prefs::DrawStyle drawStyle, Bond::BondType bondType)
{
	return selectedBonds_[drawStyle][bondType];
}

// Return line ring primitive
Primitive& PrimitiveSet::lineRing()
{
	return lineRing_;
}

// Return segmented line ring primitive
Primitive& PrimitiveSet::segmentedLineRing()
{
	return segmentedLineRing_;
}

// Return tube ring primitive
Primitive& PrimitiveSet::tubeRing()
{
	return tubeRing_;
}

// Return segmented tube ring primitive
Primitive& PrimitiveSet::segmentedTubeRing()
{
	return segmentedTubeRing_;
}

// Return cube primitive
Primitive& PrimitiveSet::cube()
{
	return cube_;
}

// Return wiret cube primitive
Primitive& PrimitiveSet::wireCube()
{
	return wireCube_;
}

// Return origin cube primitive
Primitive& PrimitiveSet::originCube()
{
	return originCube_;
}

// Return sphere primitive
Primitive& PrimitiveSet::sphere()
{
	return sphere_;
}

// Return cylinder primitive
Primitive& PrimitiveSet::cylinder()
{
	return cylinder_;
}

// Return cone primitive
Primitive& PrimitiveSet::cone()
{
	return cone_;
}

// Return crossed cube primitive
Primitive& PrimitiveSet::crossedCube()
{
	return crossedCube_;
}

// Return cell axes primitive
Primitive& PrimitiveSet::cellAxes()
{
	return cellAxes_;
}

// Return rotation globe primitive
Primitive& PrimitiveSet::rotationGlobe()
{
	return rotationGlobe_;
}

/*
 * Creation / Instantiation
 */

// Set the desired primitive quality
void PrimitiveSet::setQuality(int quality)
{
	requestedQuality_ = quality;
}
	
// (Re)Generate primitives
void PrimitiveSet::recreatePrimitives()
{
	Messenger::enter("PrimitiveSet::recreatePrimitives");

	double radius, aRadius[Prefs::nDrawStyles], bRadius[Prefs::nDrawStyles], selScale;
	int n, m, nStacks, nSlices;
	
	// If current quality is the same as the requested quality, do nothing
	if (requestedQuality_ == currentQuality_)
	{
		Messenger::exit("PrimitiveSet::recreatePrimitives");
		return;
	}

	currentQuality_ = requestedQuality_;

	// Clear old primitives
	atom_.forgetAll();
	selectedAtom_.forgetAll();
	for (n=0; n<Prefs::nDrawStyles; ++n)
	{
		for (m=0; m<Bond::nBondTypes; ++m)
		{
			bonds_[n][m].forgetAll();
			selectedBonds_[n][m].forgetAll();
		}
	}
	tubeRing_.forgetAll();
	segmentedTubeRing_.forgetAll();
	lineRing_.forgetAll();
	segmentedLineRing_.forgetAll();
	sphere_.forgetAll();
	cube_.forgetAll();
	originCube_.forgetAll();
	cylinder_.forgetAll();
	cone_.forgetAll();
	wireCube_.forgetAll();
	crossedCube_.forgetAll();
	cellAxes_.forgetAll();
	rotationGlobe_.forgetAll();

	// To clean up following code, grab radii here
	for (n=0; n<Prefs::nDrawStyles; ++n)
	{
		aRadius[n] = prefs.atomStyleRadius( (Prefs::DrawStyle) n);
		bRadius[n] = prefs.bondStyleRadius( (Prefs::DrawStyle) n);
	}
	selScale = prefs.selectionScale();

	nStacks = std::max(3, (int) (currentQuality_*0.75));
	nSlices = std::max(3, (int) (currentQuality_*1.5));
	
	// Atom Styles (Prefs::StickStyle, Prefs::TubeStyle, and Prefs::SphereStyle)
	atom_.initialise(GL_TRIANGLES, false);
	atom_.plotSphere(1.0, nStacks, nSlices);
	selectedAtom_.initialise(GL_TRIANGLES, false);
	selectedAtom_.plotSphere(selScale, nStacks, nSlices);
	
	// Bond primitive accuracy
	nStacks = std::max(1, (int) (currentQuality_*0.25));
	nSlices = std::max(3, (int) currentQuality_);
	
	// All sphere styles - Single and Aromatic Bonds
	bonds_[Prefs::TubeStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::SphereStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::ScaledStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::TubeStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::TubeStyle], bRadius[Prefs::TubeStyle], nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::SphereStyle], bRadius[Prefs::SphereStyle], nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::ScaledStyle], bRadius[Prefs::ScaledStyle], nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	selectedBonds_[Prefs::SphereStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	selectedBonds_[Prefs::ScaledStyle][Bond::Single].initialise(GL_TRIANGLES, false);
	selectedBonds_[Prefs::TubeStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::TubeStyle]*selScale, bRadius[Prefs::TubeStyle]*selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::SphereStyle]*selScale, bRadius[Prefs::SphereStyle]*selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Single].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::ScaledStyle]*selScale, bRadius[Prefs::ScaledStyle]*selScale, nStacks, nSlices);
	bonds_[Prefs::TubeStyle][Bond::Aromatic].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::SphereStyle][Bond::Aromatic].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::ScaledStyle][Bond::Aromatic].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::TubeStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::TubeStyle], bRadius[Prefs::TubeStyle], nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::SphereStyle], bRadius[Prefs::SphereStyle], nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::ScaledStyle], bRadius[Prefs::ScaledStyle], nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Aromatic].initialise(GL_LINES, false);
	selectedBonds_[Prefs::SphereStyle][Bond::Aromatic].initialise(GL_LINES, false);
	selectedBonds_[Prefs::ScaledStyle][Bond::Aromatic].initialise(GL_LINES, false);
	selectedBonds_[Prefs::TubeStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::TubeStyle]*selScale, bRadius[Prefs::TubeStyle]*selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::SphereStyle]*selScale, bRadius[Prefs::SphereStyle]*selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Aromatic].plotCylinder(0,0,0,0,0,1, bRadius[Prefs::ScaledStyle]*selScale, bRadius[Prefs::ScaledStyle]*selScale, nStacks, nSlices);
	
	// All styles - Double Bond
	bonds_[Prefs::TubeStyle][Bond::Double].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::SphereStyle][Bond::Double].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::ScaledStyle][Bond::Double].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::TubeStyle][Bond::Double].plotCylinder(-bRadius[Prefs::TubeStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.5, bRadius[Prefs::TubeStyle]*0.5, nStacks, nSlices);
	bonds_[Prefs::TubeStyle][Bond::Double].plotCylinder( bRadius[Prefs::TubeStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.5, bRadius[Prefs::TubeStyle]*0.5, nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Double].plotCylinder(-bRadius[Prefs::SphereStyle]*0.50,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.5, bRadius[Prefs::SphereStyle]*0.5, nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Double].plotCylinder( bRadius[Prefs::SphereStyle]*0.50,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.5, bRadius[Prefs::SphereStyle]*0.5, nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Double].plotCylinder(-bRadius[Prefs::ScaledStyle]*0.50,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.5, bRadius[Prefs::ScaledStyle]*0.5, nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Double].plotCylinder( bRadius[Prefs::ScaledStyle]*0.50,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.5, bRadius[Prefs::ScaledStyle]*0.5, nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Double].initialise(GL_LINES, false);
	selectedBonds_[Prefs::SphereStyle][Bond::Double].initialise(GL_LINES, false);
	selectedBonds_[Prefs::ScaledStyle][Bond::Double].initialise(GL_LINES, false);
	selectedBonds_[Prefs::TubeStyle][Bond::Double].plotCylinder(-bRadius[Prefs::TubeStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.5* selScale, bRadius[Prefs::TubeStyle]*0.5* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Double].plotCylinder( bRadius[Prefs::TubeStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.5* selScale, bRadius[Prefs::TubeStyle]*0.5* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Double].plotCylinder(-bRadius[Prefs::SphereStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.5* selScale, bRadius[Prefs::SphereStyle]*0.5* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Double].plotCylinder( bRadius[Prefs::SphereStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.5* selScale, bRadius[Prefs::SphereStyle]*0.5* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Double].plotCylinder(-bRadius[Prefs::ScaledStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.5* selScale, bRadius[Prefs::ScaledStyle]*0.5* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Double].plotCylinder( bRadius[Prefs::ScaledStyle]*0.5,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.5* selScale, bRadius[Prefs::ScaledStyle]*0.5* selScale, nStacks, nSlices);
	
	// All styles - Triple Bond
	bonds_[Prefs::TubeStyle][Bond::Triple].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::SphereStyle][Bond::Triple].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::ScaledStyle][Bond::Triple].initialise(GL_TRIANGLES, false);
	bonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::TubeStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33, bRadius[Prefs::TubeStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33, bRadius[Prefs::TubeStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder( bRadius[Prefs::TubeStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33, bRadius[Prefs::TubeStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::SphereStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33, bRadius[Prefs::SphereStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33, bRadius[Prefs::SphereStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder( bRadius[Prefs::SphereStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33, bRadius[Prefs::SphereStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::ScaledStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33, bRadius[Prefs::ScaledStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33, bRadius[Prefs::ScaledStyle]*0.33, nStacks, nSlices);
	bonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder( bRadius[Prefs::ScaledStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33, bRadius[Prefs::ScaledStyle]*0.33, nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Triple].initialise(GL_LINES, false);
	selectedBonds_[Prefs::SphereStyle][Bond::Triple].initialise(GL_LINES, false);
	selectedBonds_[Prefs::ScaledStyle][Bond::Triple].initialise(GL_LINES, false);
	selectedBonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::TubeStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33* selScale, bRadius[Prefs::TubeStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33* selScale, bRadius[Prefs::TubeStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::TubeStyle][Bond::Triple].plotCylinder( bRadius[Prefs::TubeStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::TubeStyle]*0.33* selScale, bRadius[Prefs::TubeStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::SphereStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33* selScale, bRadius[Prefs::SphereStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33* selScale, bRadius[Prefs::SphereStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::SphereStyle][Bond::Triple].plotCylinder( bRadius[Prefs::SphereStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::SphereStyle]*0.33* selScale, bRadius[Prefs::SphereStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder(-bRadius[Prefs::ScaledStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33* selScale, bRadius[Prefs::ScaledStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder(0.0,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33* selScale, bRadius[Prefs::ScaledStyle]*0.33* selScale, nStacks, nSlices);
	selectedBonds_[Prefs::ScaledStyle][Bond::Triple].plotCylinder( bRadius[Prefs::ScaledStyle]*0.66,0.0,0.0,0.0, 0.0, 1.0, bRadius[Prefs::ScaledStyle]*0.33* selScale, bRadius[Prefs::ScaledStyle]*0.33* selScale, nStacks, nSlices);
	
	// Other primitives
	nStacks = std::max(3, (int) (currentQuality_*0.75));
	nSlices = std::max(3, (int) (currentQuality_));
	cube_.initialise(GL_TRIANGLES, false);
	cube_.plotCube(1.0, 2, -0.5, -0.5, -0.5);
	originCube_.initialise(GL_TRIANGLES, false);
	originCube_.plotCube(1.0, 2, 0.0, 0.0, 0.0);

	sphere_.initialise(GL_TRIANGLES, false);
	sphere_.plotSphere(1.0, nStacks, nSlices);
	cylinder_.initialise(GL_TRIANGLES, false);
	cylinder_.plotCylinder(0,0,0,0,0,1,1.0,1.0, nStacks, nSlices);
	cone_.initialise(GL_TRIANGLES, false);
	cone_.plotCylinder(0,0,0,0,0,1,1.0,0.0, nStacks, nSlices);
	tubeRing_.initialise(GL_TRIANGLES, false);
	tubeRing_.plotRing(1.0, 0.1, 10, nSlices, 5);
	segmentedTubeRing_.initialise(GL_TRIANGLES, false);
	segmentedTubeRing_.plotRing(1.0, 0.1, 20, nSlices, 5, TRUE);
	lineRing_.initialise(GL_LINES, false);
	lineRing_.plotCircle(1.0, 10, 5);
	segmentedLineRing_.initialise(GL_LINES, false);
	segmentedLineRing_.plotCircle(1.0, 20, 5, TRUE);
	wireCube_.initialise(GL_LINES, false);
	wireCube_.plotCube(1.0, 2, 0.0, 0.0, 0.0);
	crossedCube_.initialise(GL_LINES, false);
	crossedCube_.plotCrossedCube(1.0, 2, 0.0, 0.0, 0.0);
	cellAxes_.initialise(GL_TRIANGLES, false);
	cellAxes_.plotCylinder(0.0f, 0.0f, 0.0f, 0.65f, 0.0f, 0.0f, 0.1f, 0.1f, nStacks, nSlices, TRUE, FALSE);
	cellAxes_.plotCylinder(0.65f, 0.0f, 0.0f, 0.35f, 0.0f, 0.0f, 0.2f, 0.0f, nStacks, nSlices, TRUE, FALSE);
	cellAxes_.plotCylinder(0.0f, 0.0f, 0.0f, 0.0f, 0.65f, 0.0f, 0.1f, 0.1f, nStacks, nSlices, TRUE, FALSE);
	cellAxes_.plotCylinder(0.0f, 0.65f, 0.0f, 0.0f, 0.35f, 0.0f, 0.2f, 0.0f, nStacks, nSlices, TRUE, FALSE);
	cellAxes_.plotCylinder(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.65f, 0.1f, 0.1f, nStacks, nSlices, TRUE, FALSE);
	cellAxes_.plotCylinder(0.0f, 0.0f, 0.65f, 0.0f, 0.0f, 0.35f, 0.2f, 0.0f, nStacks, nSlices, TRUE, FALSE);
	rotationGlobe_.initialise(GL_TRIANGLES, false);
	rotationGlobe_.plotSphere(0.75,10,13);
	rotationGlobe_.plotCylinder(0.7f, 0.0f, 0.0f, 0.3f, 0.0f, 0.0f, 0.2f, 0.0f, 8, 10);
	rotationGlobe_.plotCylinder(0.0f, 0.7f, 0.0f, 0.0f, 0.3f, 0.0f, 0.2f, 0.0f, 8, 10);
	rotationGlobe_.plotCylinder(0.0f, 0.0f, 0.7f, 0.0f, 0.0f, 0.3f, 0.2f, 0.0f, 8, 10);
	
	Messenger::exit("PrimitiveSet::recreatePrimitives");
}

// Create instance for primitives
void PrimitiveSet::pushInstance(const QGLContext* context, GLExtensions* extensions)
{
	Messenger::enter("PrimitiveSet::pushInstance");

	// Push instances
	atom_.pushInstance(context, extensions);
	selectedAtom_.pushInstance(context, extensions);
	for (int n=0; n<Prefs::nDrawStyles; ++n)
	{
		for (int m=0; m<Bond::nBondTypes; ++m)
		{
			bonds_[n][m].pushInstance(context, extensions);
			selectedBonds_[n][m].pushInstance(context, extensions);
		}
	}
	tubeRing_.pushInstance(context, extensions);
	segmentedTubeRing_.pushInstance(context, extensions);
	lineRing_.pushInstance(context, extensions);
	segmentedLineRing_.pushInstance(context, extensions);
	sphere_.pushInstance(context, extensions);
	cube_.pushInstance(context, extensions);
	originCube_.pushInstance(context, extensions);
	cylinder_.pushInstance(context, extensions);
	cone_.pushInstance(context, extensions);
	wireCube_.pushInstance(context, extensions);
	crossedCube_.pushInstance(context, extensions);
	cellAxes_.pushInstance(context, extensions);
	rotationGlobe_.pushInstance(context, extensions);

	++nInstances_;

	Messenger::exit("PrimitiveSet::pushInstance");
}

// Pop topmost instance for primitives
void PrimitiveSet::popInstance(const QGLContext *context, GLExtensions* extensions)
{
	Messenger::enter("PrimitiveSet::popInstance");

	atom_.popInstance(context, extensions);
	selectedAtom_.popInstance(context, extensions);
	for (int n=0; n<Prefs::nDrawStyles; ++n)
	{
		for (int m=0; m<Bond::nBondTypes; ++m)
		{
			bonds_[n][m].popInstance(context, extensions);
			selectedBonds_[n][m].popInstance(context, extensions);
		}
	}
	tubeRing_.popInstance(context, extensions);
	segmentedTubeRing_.popInstance(context, extensions);
	lineRing_.popInstance(context, extensions);
	segmentedLineRing_.popInstance(context, extensions);
	sphere_.popInstance(context, extensions);
	cube_.popInstance(context, extensions);
	originCube_.popInstance(context, extensions);
	cylinder_.popInstance(context, extensions);
	cone_.popInstance(context, extensions);
	wireCube_.popInstance(context, extensions);
	crossedCube_.popInstance(context, extensions);
	cellAxes_.popInstance(context, extensions);
	rotationGlobe_.popInstance(context, extensions);
	
	--nInstances_;

	Messenger::exit("PrimitiveSet::popInstance");
}

// Return number of instances currently pushed
int PrimitiveSet::nInstances()
{
	return nInstances_;
}