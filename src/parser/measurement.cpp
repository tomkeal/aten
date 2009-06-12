/*
	*** Measurement Variable and Array
	*** src/parser/measurement.cpp
	Copyright T. Youngs 2007-2009

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

#include "parser/measurement.h"
#include "parser/stepnode.h"
#include "base/measurement.h"
#include "base/constants.h"
#include "base/forms.h"
#include <string.h>

/*
// Variable
*/

// Constructor
MeasurementVariable::MeasurementVariable(Measurement *ptr, bool constant)
{
	// Private variables
	returnType_ = VTypes::MeasurementData;
	readOnly_ = constant;
	pointerData_ = ptr;
}

// Destructor
MeasurementVariable::~MeasurementVariable()
{
}

/*
// Accessors
*/

// Accessor data
Accessor MeasurementVariable::accessorData[MeasurementVariable::nAccessors] = {
	{ "atoms",	VTypes::AtomData,		4, TRUE },
	{ "i",		VTypes::AtomData,		0, TRUE },
	{ "j",		VTypes::AtomData,		0, TRUE },
	{ "k",		VTypes::AtomData,		0, TRUE },
	{ "l",		VTypes::AtomData,		0, TRUE }
};

// Search variable access list for provided accessor (call private static function)
StepNode *MeasurementVariable::findAccessor(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	return MeasurementVariable::accessorSearch(s, arrayindex, arglist);
}

// Private static function to search accessors
StepNode *MeasurementVariable::accessorSearch(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	msg.enter("MeasurementVariable::accessorSearch");
	StepNode *result = NULL;
	int i = 0;
	for (i = 0; i < nAccessors; i++) if (strcmp(accessorData[i].name,s) == 0) break;
	if (i == nAccessors)
	{
		// No accessor found - is it a function definition?
		for (i = 0; i < nFunctions; i++) if (strcmp(functionData[i].name,s) == 0) break;
		if (i == nFunctions)
		{
			msg.print("Error: Type 'measurement&' has no member or function named '%s'.\n", s);
			msg.exit("MeasurementVariable::accessorSearch");
			return NULL;
		}
		msg.print(Messenger::Parse, "FunctionAccessor match = %i (%s)\n", i, functionData[i].name);
		if (arrayindex != NULL)
		{
			msg.print("Error: Array index given to 'measurement&' function '%s'.\n", s);
			msg.exit("MeasurementVariable::accessorSearch");
			return NULL;
		}
		// Add and check supplied arguments...
		result = new StepNode(i, VTypes::MeasurementData, functionData[i].returnType);
		result->addArgumentList(arglist);
		if (!result->checkArguments(functionData[i].arguments, functionData[i].name))
		{
			msg.print("Error: Syntax for 'measurement&' function '%s' is '%s(%s)'.\n", functionData[i].name, functionData[i].name, functionData[i].argText );
			delete result;
			result = NULL;
		}
	}
	else
	{
		msg.print(Messenger::Parse, "Accessor match = %i (%s)\n", i, accessorData[i].name);
		// Were we given an array index when we didn't want one?
		if ((accessorData[i].arraySize == 0) && (arrayindex != NULL))
		{
			msg.print("Error: Irrelevant array index provided for member '%s'.\n", accessorData[i].name);
			result = NULL;
		}
		else result = new StepNode(i, VTypes::MeasurementData, arrayindex, accessorData[i].returnType, accessorData[i].isReadOnly, accessorData[i].arraySize);
	}
	msg.exit("MeasurementVariable::accessorSearch");
	return result;
}

// Retrieve desired value
bool MeasurementVariable::retrieveAccessor(int i, ReturnValue &rv, bool hasArrayIndex, int arrayIndex)
{
	msg.enter("MeasurementVariable::retrieveAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Measurement type.\n", i);
		msg.exit("MeasurementVariable::retrieveAccessor");
		return FALSE;
	}
	Accessors acc = (Accessors) i;
	// Check for correct lack/presence of array index given
	if ((accessorData[i].arraySize == 0) && hasArrayIndex)
	{
		msg.print("Error: Unnecessary array index provided for member '%s'.\n", accessorData[i].name);
		msg.exit("MeasurementVariable::retrieveAccessor");
		return FALSE;
	}
	else if ((accessorData[i].arraySize > 0) && (hasArrayIndex))
	{
		if ((arrayIndex < 1) || (arrayIndex > accessorData[i].arraySize))
		{
			msg.print("Error: Array index out of bounds for member '%s' (%i, range is 1-%i).\n", accessorData[i].name, arrayIndex, accessorData[i].arraySize);
			msg.exit("MeasurementVariable::retrieveAccessor");
			return FALSE;
		}
	}
	// Get current data from ReturnValue
	bool result = TRUE;
	Measurement *ptr = (Measurement*) rv.asPointer(VTypes::MeasurementData, result);
	if (result) switch (acc)
	{
		case (MeasurementVariable::Atoms):
			if ((arrayIndex < 1) || (arrayIndex > 4))
			{
				msg.print("Array index [%i] is out of range for 'atoms' member.\n", arrayIndex);
				result = FALSE;
			}
			else rv.set(VTypes::AtomData, ptr->atom(arrayIndex-1));
			break;
		case (MeasurementVariable::I):
			rv.set(VTypes::AtomData, ptr->atom(0));
			break;
		case (MeasurementVariable::J):
			rv.set(VTypes::AtomData, ptr->atom(1));
			break;
		case (MeasurementVariable::K):
			rv.set(VTypes::AtomData, ptr->atom(2));
			break;
		case (MeasurementVariable::L):
			rv.set(VTypes::AtomData, ptr->atom(3));
			break;
		default:
			printf("Internal Error: Access to member '%s' has not been defined in MeasurementVariable.\n", accessorData[i].name);
			result = FALSE;
			break;
	}
	msg.exit("MeasurementVariable::retrieveAccessor");
	return result;
}

// Set desired value
bool MeasurementVariable::setAccessor(int i, ReturnValue &sourcerv, ReturnValue &newvalue, bool hasArrayIndex, int arrayIndex)
{
	msg.enter("MeasurementVariable::setAccessor");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nAccessors))
	{
		printf("Internal Error: Accessor id %i is out of range for Measurement type.\n", i);
		msg.exit("MeasurementVariable::retrieveAccessor");
		return FALSE;
	}
	Accessors acc = (Accessors) i;
	// Check for correct lack/presence of array index given to original accessor, and nature of new value
	bool result = TRUE;
	if (accessorData[i].arraySize != 0)
	{
		if (hasArrayIndex)
		{
			if ((accessorData[i].arraySize > 0) && ( (arrayIndex < 1) || (arrayIndex > accessorData[i].arraySize) ))
			{
				msg.print("Error: Array index provided for member '%s' is out of range (%i, range is 1-%i).\n", accessorData[i].name, arrayIndex, accessorData[i].arraySize);
				result = FALSE;
			}
			if (newvalue.arraySize() > 0)
			{
				msg.print("Error: An array can't be assigned to the single valued member '%s'.\n", accessorData[i].name);
				result = FALSE;
			}
		}
		else
		{
			if ((newvalue.arraySize() > 0) && (newvalue.arraySize() != accessorData[i].arraySize))
			{
				msg.print("Error: The array being assigned to member '%s' is not of the same size (%i cf. %i).\n", accessorData[i].name, newvalue.arraySize(), accessorData[i].arraySize);
				result = FALSE;
			}
		}
	}
	else
	{
		// This is not an array member, so cannot be assigned an array unless its a Vector
		if (newvalue.arraySize() != -1)
		{
			if (accessorData[i].returnType != VTypes::VectorData)
			{
				msg.print("Error: An array can't be assigned to the single valued member '%s'.\n", accessorData[i].name);
				result = FALSE;
			}
			else if ((newvalue.type() != VTypes::VectorData) && (newvalue.arraySize() != 3))
			{
				msg.print("Error: Only an array of size 3 can be assigned to a vector (member '%s').\n", accessorData[i].name);
				result = FALSE;
			}
		}
	}
	if (!result)
	{
		msg.exit("MeasurementVariable::setAccessor");
		return FALSE;
	}
	// Get current data from ReturnValue
	VdwFunctions::VdwFunction vf;
	Measurement *ptr= (Measurement*) sourcerv.asPointer(VTypes::MeasurementData, result);
	if (result) switch (acc)
	{
		default:
			printf("MeasurementVariable::setAccessor doesn't know how to use member '%s'.\n", accessorData[acc].name);
			result = FALSE;
			break;
	}
	msg.exit("MeasurementVariable::setAccessor");
	return result;
}

// Perform desired function
bool MeasurementVariable::performFunction(int i, ReturnValue &rv, TreeNode *node)
{
	msg.enter("MeasurementVariable::performFunction");
	// Cast 'i' into Accessors enum value
	if ((i < 0) || (i >= nFunctions))
	{
		printf("Internal Error: FunctionAccessor id %i is out of range for Measurement type.\n", i);
		msg.exit("MeasurementVariable::performFunction");
		return FALSE;
	}
	// Get current data from ReturnValue
	bool result = TRUE;
	Measurement *ptr= (Measurement*) rv.asPointer(VTypes::MeasurementData, result);
	if (result) switch (i)
	{
		default:
			printf("Internal Error: Access to function '%s' has not been defined in MeasurementVariable.\n", functionData[i].name);
			result = FALSE;
			break;
	}
	msg.exit("MeasurementVariable::performFunction");
	return result;
}

/*
// Variable Array
*/

// Constructor
MeasurementArrayVariable::MeasurementArrayVariable(TreeNode *sizeexpr, bool constant)
{
	// Private variables
	returnType_ = VTypes::MeasurementData;
	pointerArrayData_ = NULL;
	arraySize_ = 0;
	nodeType_ = TreeNode::ArrayVarNode;
	readOnly_ = constant;
	arraySizeExpression_ = sizeexpr;
}

// Search variable access list for provided accessor
StepNode *MeasurementArrayVariable::findAccessor(const char *s, TreeNode *arrayindex, TreeNode *arglist)
{
	return MeasurementVariable::accessorSearch(s, arrayindex, arglist);
}