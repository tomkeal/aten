/*
	*** Messaging routines
	*** src/base/messenger.cpp
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

#include "gui/gui.h"
#include <stdarg.h>

// Singleton
Messenger msg;

// Message output types
const char *OutputTypeKeywords[] = { "calls", "typing", "parse", "verbose", "commands", "_ERROR_" };
Messenger::OutputType Messenger::outputType(const char *s)
{
	return (Messenger::OutputType) power(2,enumSearch("output type",Messenger::nOutputTypes,OutputTypeKeywords,s));
}

// Constructor
Messenger::Messenger()
{
	// Private variables
	outputTypes_= 0;
	callLevel_ = 0;
	quiet_ = FALSE;
}

// Add a debug level to the debug output bitvector
void Messenger::addOutputType(Messenger::OutputType dm)
{
	if (!(outputTypes_&dm)) outputTypes_ += dm;
}

// Remove a debug level from the debug output bitvector
void Messenger::removeOutputType(Messenger::OutputType dm)
{
	if (outputTypes_&dm) outputTypes_ -= dm;
}

// Returns whether the specified debug level is set
bool Messenger::isOutputActive(Messenger::OutputType dm)
{
	return ((outputTypes_&dm) ? TRUE : FALSE);
}

// Set status of quiet mode
void Messenger::setQuiet(bool b)
{
	quiet_ = b;
}

// Return status of quiet mode
bool Messenger::isQuiet()
{
	return quiet_;
}

// Standard message
void Messenger::print(const char *fmt ...)
{
	// Print to the text view in the main window if it has been initialised.
	// If program is in quiet mode, don't print anything to stdout
	// Otherwise, print to stdout. Also print to stdout if debuglevel >= msglevel.
	va_list arguments;
	static char msgs[8096];
	msgs[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(msgs,fmt,arguments);
	// We always print standard messages to stdout *or* the GUI (if it has been initialised)
	if (gui.exists()) gui.printMessage(msgs);
	else if (!quiet_) printf("%s",msgs);
	va_end(arguments);
}

// Standard message in specific output level
void Messenger::print(Messenger::OutputType ot, const char *fmt ...)
{
	// Print to the text view in the main window if it has been initialised.
	// If program is in quiet mode, don't print anything except Messenger::Error calls
	if (quiet_ && (ot != Messenger::Error)) return;
	va_list arguments;
	static char msgs[8096];
	msgs[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(msgs,fmt,arguments);
	// Print message to stdout, but only if specified output type is active
// 	if (ot == Debug::None)
// 	{
// 		if (gui.exists()) gui.printMessage(msgs);
// 		else printf("%s",msgs);
// 	}
// 	else
	if (ot == Messenger::Error) printf("%s",msgs);
	else if (isOutputActive(ot) && (!quiet_)) printf("%s",msgs);
	va_end(arguments);
}

// Function enter
void Messenger::enter(const char *callname)
{
	if (!(outputTypes_&Messenger::Calls)) return;
	// Debug Messaging - Enter Function
	printf("%2i ",callLevel_);
	for (int n=0; n<callLevel_; n++) printf("--");
	printf("Begin : %s...\n", callname);
	callLevel_ ++;
}

// Function leave
void Messenger::exit(const char *callname)
{
	if (!(outputTypes_&Messenger::Calls)) return;
	// Debug Messaging - Leave Function
	callLevel_ --;
	printf("%2i ", callLevel_);
	for (int n=0; n<callLevel_; n++) printf("--");
	printf("End   : %s.\n", callname);
}