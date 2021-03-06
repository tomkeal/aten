/*
	*** Aten's session
	*** src/main/session.cpp
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

#include "main/aten.h"
#include "ff/forcefield.h"
#include "version.h"

ATEN_USING_NAMESPACE

// Return name of current session file, if any
QString Aten::sessionFilename()
{
	return sessionFilename_;
}

// Clear current session (remove all user data)
void Aten::clearSession()
{
	visibleModels_.clear();
	models_.clear();
	forcefields_.clear();
}

// Load session from filename specified
bool Aten::loadSession(QString filename)
{
	// Attempt to create a script from the file
	Program sessionScript;
	sessionFilename_ = filename;
	if (sessionScript.generateFromFile(sessionFilename_, "SessionScript"))
	{
		// Clear all current information
		clearSession();

		// Execute the script
		ReturnValue rv;
		if (!sessionScript.execute(rv))
		{
			sessionFilename_.clear();

			return false;
		}

		// If there are no models, we must add one
		if (models_.nItems() == 0) addModel();
	}
	else return false;

	return true;
}

// Save session under specified filename
bool Aten::saveSession(QString filename)
{
	sessionFilename_ = filename;

	LineParser parser;
	if (!parser.openOutput(sessionFilename_, true)) return false;

	// Write header information
	parser.writeLineF("// Session file saved from Aten version %s on %s", ATENVERSION, qPrintable(QDateTime::currentDateTime().toString()));

	// Preferences
	dumpPreferences(parser);

	// Element information
	dumpElementInfo(parser);

	// Non-model related settings / aspects
	// TODO Forcefields etc.

	// Loop over all loaded models. Save original file source information as a comment only, since a filter may have loaded in multiple models, we can never be sure.
	int modelId = 0, gridId = 0;
	for (Model* m = models_.first(); m != NULL; m = m->next, ++modelId)
	{
		// Create variable pointer name
		QString modelVar = QString("model%1").arg(modelId, 3, 10, QChar('0'));
	
		// Write header
		parser.writeLine();
		parser.writeLineF("// Model '%s'", qPrintable(m->name()));
		parser.writeLineF("// Original Filename: '%s'", qPrintable(m->filename()));

		// Write atom and bond information
		parser.writeLineF("Model %s = newModel('%s');", qPrintable(modelVar), qPrintable(m->name()));
		for (Atom* i = m->atoms(); i != NULL; i = i->next) parser.writeLineF("%s.newAtom(%s,%f,%f,%f);", qPrintable(modelVar), ElementMap::symbol(i->element()), i->r().x, i->r().y, i->r().z);
		for (Bond* b = m->bonds(); b != NULL; b = b->next) parser.writeLineF("%s.newBond(%i,%i,'%s');", qPrintable(modelVar), b->atomI()->id()+1, b->atomJ()->id()+1, Bond::bondType(b->type()));

		// Atom selection
		for (RefListItem<Atom,int>* ri = m->selection(); ri != NULL; ri = ri->next) parser.writeLineF("%s.select(%i);", qPrintable(modelVar), ri->item->id()+1);

		// Grid data
		for (Grid* g = m->grids(); g != NULL; g = g->next, ++gridId)
		{
			// Create variable pointer name
			QString gridVar = QString("grid%1").arg(gridId, 3, 10, QChar('0'));

			// Write basic grid info or filename
			if (!g->filename().isEmpty())
			{
				parser.writeLineF("Grid %s = loadGrid('%s');", qPrintable(gridVar), qPrintable(g->filename()));
			}
			parser.writeLineF("%s.name = '%s';", qPrintable(gridVar), qPrintable(g->name()));
			// Write axes, offset, and cutoff information
			double* axes = g->axes().matrix();
			parser.writeLineF("%s.axes = { %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f};", qPrintable(gridVar), axes[0], axes[1], axes[2], axes[3], axes[4], axes[5], axes[6], axes[7], axes[8], axes[9], axes[10], axes[11], axes[12], axes[13], axes[14], axes[15]);
			parser.writeLineF("%s.origin = { %f, %f, %f };", qPrintable(gridVar), g->origin().x, g->origin().y, g->origin().z);
			parser.writeLineF("%s.cutoff = %f;", qPrintable(gridVar), g->lowerPrimaryCutoff());
			parser.writeLineF("%s.upperCutoff = %f;", qPrintable(gridVar), g->upperPrimaryCutoff());
			parser.writeLineF("%s.secondaryCutoff = %f;", qPrintable(gridVar), g->lowerSecondaryCutoff());
			parser.writeLineF("%s.secondaryUpperCutoff = %f;", qPrintable(gridVar), g->upperSecondaryCutoff());
			// Style information
			parser.writeLineF("%s.colour = { %f, %f, %f, %f };", qPrintable(gridVar), g->primaryColour()[0], g->primaryColour()[1], g->primaryColour()[2], g->primaryColour()[3]);
			parser.writeLineF("%s.style = '%s';", qPrintable(gridVar), Grid::surfaceStyle(g->primaryStyle()));
			parser.writeLineF("%s.secondaryColour = { %f, %f, %f, %f };", qPrintable(gridVar), g->secondaryColour()[0], g->secondaryColour()[1], g->secondaryColour()[2], g->secondaryColour()[3]);
			parser.writeLineF("%s.secondaryStyle = '%s';", qPrintable(gridVar), Grid::surfaceStyle(g->secondaryStyle()));
			parser.writeLineF("%s.visible = %i;", qPrintable(gridVar), g->isVisible());
			parser.writeLineF("%s.outlineVolume = %i;", qPrintable(gridVar), g->outlineVolume());
			parser.writeLineF("%s.periodic = %i;", qPrintable(gridVar), g->periodic());
		}

		// View
		Matrix view = m->modelViewMatrix();
		parser.writeLineF("setView(%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f);", view[0], view[1], view[2], view[4], view[5], view[6], view[8], view[9], view[10], view[12], view[13], view[14]);
	}

	parser.closeFiles();

	return true;
}
