/*
	*** Log Class
	*** src/base/log.h
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

#ifndef ATEN_LOG_H
#define ATEN_LOG_H

#include "base/namespace.h"

ATEN_BEGIN_NAMESPACE

// Log Class
class Log
{
	public:
	// Constructor
	Log();
	// Operator == (test equivalence of all log values except Total)
	bool operator==(Log &l) const;
	bool operator!=(Log &l) const;
	// Log types
	enum LogType { Structure, Coordinates, Cell, Style, Selection, Grids, Glyphs, Labels, Misc, Total, nLogTypes };
	// Structure	: create/destroy atoms/bonds, change elements, change order of atoms
	// Coordinate	: atomic coordinates
	// Cell		: unit cell
	// Style	: visual changes that require re-rendering of many objects in the model (e.g. atom colours have changed)
	// Selection	: atom selection
	// Glyphs	: glyphs
	// Grids	: grids
	// Labels	: atom labels or measurements
	// Misc		: miscellaneous changes (e.g. title changes)
	// Total	: sum of all changes

	private:
	// Integer 'logs' of model changes
	int logs_[Log::nLogTypes];
	// Log point of the last save / point on load
	int savePoint_;
	// Return current logpoint of the model (sum of specific logs)
	int currentLogPoint() const;

	public:
	// Reset all logs to zero
	void reset();
	// Increment specified log
	void add(Log::LogType);
	// Set a log to a specific value
	void setLog(Log::LogType lt, int value);
	// Return the log quantity specified
	int log(Log::LogType cl) const;
	// Set the save point log for the model
	void updateSavePoint();
	// Return if the model has been modified since last being saved
	bool isModified() const;
	// Return is specified log quantity is same between this and supplied Log
	bool isSame(Log::LogType lt, Log &other);
	// Print logs
	void print() const;
};

ATEN_END_NAMESPACE

#endif
