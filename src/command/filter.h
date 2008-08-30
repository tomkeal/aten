/*
	*** File filter definition
	*** src/parse/filter.h
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

#ifndef ATEN_FILTER_H
#define ATEN_FILTER_H

#include "command/commandlist.h"
#include "base/elements.h"
#include <fstream>

// Forward Declarations
class Model;
class Grid;
class Pattern;
class Cell;
class ForcefieldBound;

// Model file import/export filter
class Filter
{
	public:
	// Constructor
	Filter();
	// List pointers
	Filter *prev, *next;
	// Filter Types
	enum FilterType { ModelImport, TrajectoryImport, ExpressionImport, GridImport, ModelExport, TrajectoryExport, ExpressionExport, GridExport, nFilterTypes };
	static const char *filterType(FilterType ft);
	static FilterType filterType(const char *s);
	
	// Filter commands
	enum FilterCommmand { NameCommand, NicknameCommand, ExtensionCommand, GlobCommand, ExactCommand, ZMapCommand, IdCommand, nFilterCommands };
	static FilterCommmand filterCommand(const char *s);
	static const char *filterCommand(FilterCommmand fc);

	// Print information on filter
	void print();
	// Load filter commands from file
	bool load(ifstream&);

	/*
	// Properties
	*/
	private:
	// Filter ID
	int id_;
	// Type of data the filter describes
	FilterType type_;
	// Long name of the filter
	Dnchar name_;
	// Nickname for the filter
	Dnchar nickname_;
	// File extension(s)
	List<Dnchar> extensions_;
	// File filter glob (for gui)
	Dnchar glob_;
	// Partner filter
	Filter *partner_;
	// Filter description
	Dnchar description_;
	// Filename alias list
	List<Dnchar> exactNames_;
	// Whether the file has an associated extension
	bool hasExtension_;
	// Whether separate zmapping has been defined
	bool hasZmapping_;
	// Type of element mapping to use
	ElementMap::ZmapType zmapping_;

	public:
	// Return the ID of the filter
	int id();
	// Return the descriptive name of the filter
	const char *name();
	// Return the short nickname of the filter
	const char *nickname();
	// Return the first file extension
	Dnchar *extensions();
	// Return the aliases list
	Dnchar *exactNames();
	// Return whether filter has an extension
	bool hasExtension();
	// Set the partner filter
	void setPartner(Filter *f);
	// Return the partner filter
	Filter *partner();
	// Return the file filter
	const char *glob();
	// Set the type of filter
	void setType(FilterType ft);
	// Return the type of filter
	FilterType type();
	// Return the long description of the filter (including glob)
	const char *description();

	/*
	// Command actions
	*/
	private:
	// Command list
	CommandList commands_;
	// Reset all targets
	void resetTargets();

	public:
	// Execute filter
	bool execute(const char *filename, ifstream *trajfile = NULL, bool trajheader = FALSE);
};

#endif