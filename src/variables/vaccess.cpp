/*
	*** Variable Access Class
	*** src/variables/vaccess.cpp
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

#include "variables/vaccess.h"
#include "base/messenger.h"

// Constructor
VAccess::VAccess()
{
	// Clear default variables in VariableList
	accessors_.clear();
}

// Return address of VariableList
VariableList *VAccess::accessors()
{
	return &accessors_;
}

// Add new accessor
Variable *VAccess::addAccessor(const char *name, VTypes::DataType dt, bool readonly)
{
	msg.enter("VAccess::addAccessor");
	Variable *result = accessors_.addVariable(name, dt);
	if (readonly) result->setReadOnly();
	msg.exit("VAccess::addAccessor");
	return result;
}

// Add new accessor
Variable *VAccess::addListAccessor(const char *name, VTypes::DataType dt)
{
	msg.enter("VAccess::addListAccessor");
	Variable *result = accessors_.addVariable(name, dt);
	result->setReadOnly();
	result->setListArray();
	msg.exit("VAccess::addListAccessor");
	return result;
}

// Return 'id' (position in list) of supplied accessor
int VAccess::accessorId(Variable *accessor)
{
	return accessors_.variableId(accessor);
}