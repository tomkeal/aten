/*
	*** Flow Commands
	*** src/parser/flow..cpp
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

#include "nucommand/commands.h"
#include "parser/commandnode.h"
#include "base/mathfunc.h"
#include <stdio.h>
#include <string.h>

// Dummy Node
bool NuCommand::function_NoFunction(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	return TRUE;
}

// Joiner
bool NuCommand::function_Joiner(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	// Execute both commands
	if (!c->arg(0, rv)) return FALSE;
	if (c->hasArg(1)) return c->arg(1, rv);
}

// Variable Declarations Node
bool NuCommand::function_Declarations(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	return TRUE;
}

// If test
bool NuCommand::function_If(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	NuReturnValue ifval;
	c->arg(0, ifval);
	if (ifval.asBool()) c->arg(1, rv);
	else if (c->hasArg(2)) c->arg(2, rv);
	return TRUE;
}

// Break out of current for loop
bool NuCommand::function_Break(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	return TRUE;
}

// Continue for loop at next iteration
bool NuCommand::function_Continue(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	return TRUE;
}

// For loop
bool NuCommand::function_For(NuCommandNode *c, Bundle &obj, NuReturnValue &rv)
{
	return TRUE;
}