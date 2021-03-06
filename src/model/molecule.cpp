/*
	*** Model molecule functions
	*** src/model/molecule.cpp
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

#include "model/model.h"
#include "base/pattern.h"

ATEN_USING_NAMESPACE

// Position molecule at specified coordinates
void Model::positionMolecule(Pattern* p, int mol, const Vec3<double>& v)
{
	Messenger::enter("Model::positionMolecule");
	static Vec3<double> newpos, cog;
	static int pnatoms, offset, n;
	Atom** modelatoms = atomArray();
	pnatoms = p->nAtoms();
	offset = p->startAtom() + pnatoms * mol;
	Messenger::print(Messenger::Verbose, "Model::positionMolecule : Moving %i atoms starting at %i (config can hold %i atoms)", pnatoms, offset, atoms_.nItems());
	if (offset < atoms_.nItems())
	{
		cog = p->calculateCog(mol,this);
		for (n=offset; n<offset+pnatoms; n++)
		{
			// Get local coordinates of atom - mim with and then subtract centre of geometry
			//newpos = modelatoms[n];
			newpos = cell_.mim(modelatoms[n]->r(),cog);
			newpos -= cog;
			// Re-position
			newpos += v;
			// Store new positions
			modelatoms[n]->r() = newpos;
		}
	}
	else printf("Model::positionMolecule : Requested a molecule past end of config contents. (%s %i)\n", qPrintable(p->name()), mol); 
	Messenger::exit("Model::positionMolecule");
}

// Translate molecule along vector
void Model::translateMolecule(Pattern* p, int mol, const Vec3<double>& v)
{
	// Vector 'v' should be normalised before passing
	Messenger::enter("Model::translateMolecule");
	static int pnatoms, offset, n;
	Atom** modelatoms = atomArray();
	pnatoms = p->nAtoms();
	offset = p->startAtom() + pnatoms * mol;
	Messenger::print(Messenger::Verbose, "Model::translateMolecule : Moving %i atoms starting at %i (%i atoms currently in model)", pnatoms, offset, atoms_.nItems());
	if (offset < atoms_.nItems()) for (n=offset; n<offset+pnatoms; n++) modelatoms[n]->r() += v;
	else printf("Model::translateMolecule : Requested a molecule past end of model contents. (%s %i)\n", qPrintable(p->name()), mol);  
	Messenger::exit("Model::translateMolecule");
}

void Model::rotateMolecule(Pattern* p, int mol, double rotx, double roty)
{
	// Rotate the coordinates of the atoms in pattern p, molecule mol, about their centre of geometry.
	// rotx and roty are the rotations about the x and y axes respectively, in degrees
	Messenger::enter("Model::rotateMolecule");
	static Matrix rotmat;
	static Vec3<double> delta, newpos, cog;
	static int pnatoms, offset, n;
	Atom** modelatoms = atomArray();
	rotmat.createRotationXY(rotx,roty);
	pnatoms = p->nAtoms();
	offset = p->startAtom() + pnatoms * mol;
	// Calculate COG before we start
	cog = p->calculateCog(mol,this);
	//printf("rotateMolecule : Moving %i atoms starting at %i (%i atoms currently in config)\n",pnatoms,offset,natoms);
	if (offset < atoms_.nItems())
	{
		for (n=offset; n<offset+pnatoms; n++)
		{
			// Get local coordinates of atom, i.e. subtract centre of geometry
			//newpos = cell_.mim(modelatoms[n]->r(), cog) - cog;
			//newpos = rotmat * (modelatoms[n]->r() - cog);
			newpos = rotmat * (cell_.mim(modelatoms[n]->r(), cog) - cog);
			newpos += cog;
			// Store the new position
			modelatoms[n]->r() = newpos;
		}
	}
	else printf("Model::rotateMolecule : Requested a molecule past end of model contents. (%s %i)\n", qPrintable(p->name()), mol); 
	Messenger::exit("Model::rotateMolecule");
}

// Set the hidden flag on atoms of the specified molecule
void Model::hideMolecule(Pattern* p, int mol, bool visible)
{
	Messenger::enter("Model::hideMolecule");
	static int pnatoms, offset, n;
	Atom** modelatoms = atomArray();
	pnatoms = p->nAtoms();
	offset = p->startAtom() + pnatoms * mol;
	if (offset < atoms_.nItems()) for (n=offset; n<offset+pnatoms; n++) modelatoms[n]->setHidden(visible);
	else printf("Model::hideMolecule : Requested a molecule past end of model contents. (%s %i)\n", qPrintable(p->name()), mol); 
	Messenger::exit("Model::hideMolecule");
}
