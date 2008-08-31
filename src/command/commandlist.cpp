/*
	*** Command list functions
	*** src/command/commandlist.cpp
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

#include "command/commandlist.h"
#include "command/format.h"
#include "variables/expression.h"
#include "main/aten.h"
#include "model/model.h"
#include "base/sysfunc.h"

// If Conditions
const char *IfTestStrings[6] = { "eq", "l", "le", "g", "ge", "neq" };
const char *IfTests::ifTest(IfTest i)
{
	return IfTestStrings[i-1];
}
// Variable Operators
const char *AssignOpKeywords[AssignOps::nAssignOps] = { "=", "+=", "-=", "/=", "*=" };
AssignOps::AssignOp AssignOps::assignOp(const char *s)
{
	return (AssignOps::AssignOp) enumSearch("assignment operator", AssignOps::nAssignOps, AssignOpKeywords, s);
}
const char *AssignOps::assignOp(AssignOps::AssignOp ao)
{
	return AssignOpKeywords[ao];
}

// Constructors
Command::Command()
{
	// Private variables
	action_ = CA_ROOTNODE;
	parent_ = NULL;
	function_ = NULL;
	ptr_ = NULL;
	branch_ = NULL;
	format_ = NULL;
	loopActive_ = FALSE;

	// Public variables
	next = NULL;
	prev = NULL;
}

CommandList::CommandList()
{
	// Private variables
	inputFile_ = NULL;
	outputFile_ = NULL;
	readOptions_ = 0;
	pushBranch(&commands_, CA_ROOTNODE, NULL);

	// Public variables
	next = NULL;
	prev = NULL;
}

// Destructors
Command::~Command()
{
	if (branch_ != NULL) delete branch_;
	if (format_ != NULL) delete format_;
}

/*
// Command
*/

// Set parent CommandList
void Command::setParent(CommandList *cl)
{
	parent_ = cl;
}

// Get parent CommandList
CommandList *Command::parent()
{
	return parent_;
}

// Get command
CommandAction Command::command()
{
	return action_;
}

// Returns the formatter
Format *Command::format()
{
	return format_;
}

// Delete the associated format
void Command::deleteFormat()
{
	if (format_ != NULL) delete format_;
	format_ = NULL;
}

// Set status of loop
void Command::setLoopActive(bool b)
{
	loopActive_ = b;
}

// Get status of loop
bool Command::isLoopActive()
{
	return loopActive_;
}

// Set iteration count
void Command::setLoopIterations(int n)
{
	loopIterations_ = n;
}

// Get iteration count
int Command::loopIterations()
{
	return loopIterations_;
}

// Increase interation count
void Command::increaseIterations()
{
	loopIterations_ ++;
}

// Returns branch list structure
List<Command> *Command::branch()
{
	return branch_;
}

// Returns first item in branch 
Command *Command::branchCommands()
{
	return (branch_ != NULL ? branch_->first() : NULL);
}

// Set FormatNode pointer variable
void Command::setPointer(Command *f)
{
	ptr_ = f;
}

// Return FormatNode pointer variable
Command *Command::pointer()
{
	return ptr_;
}

// Return variable argument
Variable *Command::arg(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ? NULL : rv->item);
}

// Return argument as character
const char *Command::argc(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ?  "NULL" : rv->item->asCharacter());
}

// Return argument as integer
int Command::argi(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ?  0 : rv->item->asInteger());
}

// Return argument as double
double Command::argd(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ? 0.0 : rv->item->asDouble());
}

// Return argument as float
float Command::argf(int argno)
{
	return (float) argd(argno);
}

// Return argument as bool
bool Command::argb(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ? -1 : rv->item->asBool());
}

// Return argument as atom pointer
Atom *Command::arga(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	return (Atom*) rv->item->asPointer(VTypes::AtomData);
}

// Return argument as pattern pointer
Pattern *Command::argp(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	return (Pattern*) rv->item->asPointer(VTypes::PatternData);
}

// Return argument as grid pointer
Grid *Command::argg(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	return (Grid*) rv->item->asPointer(VTypes::GridData);
}

// Return argument as model pointer
Model *Command::argm(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	return (Model*) rv->item->asPointer(VTypes::ModelData);
}

// Return argument as PatternBound pointer
PatternBound *Command::argpb(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	else if ((rv->item->type() < VTypes::BondData) || (rv->item->type() > VTypes::TorsionData)) msg.print("Command can't convert variable '%s' into a PatternBound\n.", rv->item->name());
	else return (PatternBound*) rv->item->asPointer(VTypes::BondData);
	return NULL;
}

// Return argument as ForcefieldAtom pointer
ForcefieldAtom *Command::argffa(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	if (rv == NULL) return NULL;
	return (ForcefieldAtom*) rv->item->asPointer(VTypes::AtomtypeData);
}

// Returns whether argument 'n' was provided
bool Command::hasArg(int argno)
{
	return ((argno+1) > args_.nItems() ? FALSE : TRUE);
}

// Return variable type of argument
VTypes::DataType Command::argt(int argno)
{
	Refitem<Variable,int> *rv = args_[argno];
	return (rv == NULL ? VTypes::nDataTypes : rv->item->type());
}

// Set command and function
void Command::setCommand(CommandAction ca)
{
	action_ = ca;
	function_ = CA_data[ca].function;
}

// Clear command list and reinitialise
void CommandList::clear()
{
	commands_.clear();
	branchStack_.clear();
	branchCommandStack_.clear();
	pushBranch(&commands_, CA_ROOTNODE, NULL);
}

// Print data variables
void Command::printArgs()
{
	msg.enter("Command::printArgs");
	int i = 0;
	Variable *v;
	for (Refitem<Variable,int> *rv = args_.first(); rv != NULL; rv = rv->next)
	{
		v = rv->item;
		printf("%2i %20li", i, v);
		printf("%12s [%10s]",v->name(), VTypes::dataType(v->type()));
		if (v->type() < VTypes::AtomData) printf("%20s\n",v->asCharacter());
// 		else printf("%li\n", v->asPointer());
		else printf("ptr\n");
		i++;
	}
	msg.exit("Command::printArgs");
}

// Return arguments as Vec3<double>
Vec3<double> Command::arg3d(int i)
{
	msg.enter("Command::arg3d");
        static Vec3<double> result;
        if (i > (args_.nItems()-3)) printf("Command::arg3d - Starting point too close to end of argument list.\n");
        result.set(args_[i]->item->asDouble(),args_[i+1]->item->asDouble(),args_[i+2]->item->asDouble());
	msg.exit("Command::arg3d");
        return result;
}

// Return arguments as Vec3<float>
Vec3<float> Command::arg3f(int i)
{
	msg.enter("Command::arg3f");
        static Vec3<float> result;
        if (i > (args_.nItems()-3)) printf("Command::arg3f - Starting point too close to end of argument list.\n");
        result.set(args_[i]->item->asFloat(),args_[i+1]->item->asFloat(),args_[i+2]->item->asFloat());
	msg.exit("Command::arg3f");
        return result;
}

// Return arguments as Vec3<int>
Vec3<int> Command::arg3i(int i)
{
	msg.enter("Command::arg3i");
	static Vec3<int> result;
	if (i > (args_.nItems()-3)) printf("Command::arg3i - Starting point too close to end of argument list.\n");
        result.set(args_[i]->item->asInteger(),args_[i+1]->item->asInteger(),args_[i+2]->item->asInteger());
	msg.exit("Command::arg3i");
	return result;
}

// Create branch
List<Command> *Command::createBranch()
{
	msg.enter("Command::createBranch");
	if (branch_ != NULL) printf("Command::createBranch <<<< Already has a branch >>>>\n");
	branch_ = new List<Command>;
	msg.exit("Command::createBranch");
	return branch_;
}

// Create branch
bool Command::createFormat(const char *s, VariableList &vars, bool delimited)
{
	msg.enter("Command::createFormat");
	bool result = FALSE;
	if (format_ != NULL) printf("Command::createFormat <<<< Already has a format >>>>\n");
	else
	{
		format_ = new Format;
		result = format_->create(s, vars, delimited);
	}
	msg.exit("Command::createFormat");
	return result;
}

// Set if condition test
bool Command::setIfTest(const char *s)
{
	msg.enter("Command::setIfTest");
	bool result = TRUE;
	int n, m;
	m = 0;
	for (n=0; s[n] != '\0'; n++)
		switch (s[n])
		{
			case ('='):
				m += 1;
				break;
			case ('<'):
				m += 2;
				break;
			case ('>'):
				m += 4;
				break;
			default:
				msg.print( "Unrecognised character '%c' in 'if' condition\n", s[n]);
				result = FALSE;
				break;
		}
	if (m > IfTests::NotEqualTo) result = FALSE;
	else ifTest_ = (IfTests::IfTest) m;
	msg.exit("Command::setIfTest");
	return result;
}

// Evaluate condition
bool Command::ifEvaluate()
{
	msg.enter("Command::ifEvaluate");
	bool result;
	static Variable *v1, *v2;
	static char string1[512], string2[512];
	static double d1, d2;
	VTypes::DataType vt1, vt2;
	static int i1, i2;
	v1 = args_[0]->item;
	v2 = args_[2]->item;
	// Determine how to do the comparison
	vt1 = v1->type();
	vt2 = v2->type();
	if (vt1 == VTypes::ExpressionData) vt1 = ((ExpressionVariable*) v1)->evaluatesToReal() ? VTypes::RealData : VTypes::IntegerData;
	if (vt2 == VTypes::ExpressionData) vt2 = ((ExpressionVariable*) v2)->evaluatesToReal() ? VTypes::RealData : VTypes::IntegerData;
	if (vt2 > vt1) vt1 = vt2;
	if (vt1 == VTypes::CharacterData)
	{
		strcpy(string1, v1->asCharacter());
		strcpy(string2, v2->asCharacter());
		msg.print(Messenger::Commands, "If Test: var1(%s)=[%s] (%s) var2(%s)=[%s]\n", v1->name(), string1, IfTests::ifTest(ifTest_), v2->name(), string2);

		switch (ifTest_)
		{
			case (IfTests::EqualTo):
				result = (strcmp(string1,string2) == 0 ? TRUE : FALSE);
				break;
			case (IfTests::LessThan):
				result = (strcmp(string1,string2) < 0 ? TRUE : FALSE);
				break;
			case (IfTests::LessThanEqualTo):
				result = (strcmp(string1,string2) <= 0 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThan):
				result = (strcmp(string1,string2) > 0 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThanEqualTo):
				result = (strcmp(string1,string2) >= 0 ? TRUE : FALSE);
				break;
			case (IfTests::NotEqualTo):
				result = (strcmp(string1,string2) != 0 ? TRUE : FALSE);
				break;
		}
	}
	else if (vt1 == VTypes::IntegerData)
	{
		i1 = v1->asInteger();
		i2 = v2->asInteger();
		msg.print(Messenger::Commands, "If Test: var1(%s)=[%i] (%s) var2(%s)=[%i] : %s\n", v1->name(), i1, IfTests::ifTest(ifTest_), v2->name(), i2, result ? "True" : "False");
		switch (ifTest_)
		{
			case (IfTests::EqualTo):
				result = (i1 == i2 ? TRUE : FALSE);
				break;
			case (IfTests::LessThan):
				result = (i1 < i2 ? TRUE : FALSE);
				break;
			case (IfTests::LessThanEqualTo):
				result = (i1 <= i2 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThan):
				result = (i1 > i2 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThanEqualTo):
				result = (i1 >= i2 ? TRUE : FALSE);
				break;
			case (IfTests::NotEqualTo):
				result = (i1 != i2 ? TRUE : FALSE);
				break;
		}
	}
	else
	{
		d1 = v1->asDouble();
		d2 = v2->asDouble();
		msg.print(Messenger::Commands, "If Test: var1(%s)=[%f] (%s) var2(%s)=[%f] : %s\n", v1->name(), d1, IfTests::ifTest(ifTest_), v2->name(), d2, result ? "True" : "False");
		// Do comparison
		switch (ifTest_)
		{
			case (IfTests::EqualTo):
				msg.print("Warning: Comparing between floating point values...\n");
				result = (d1 == d2 ? TRUE : FALSE);
				break;
			case (IfTests::LessThan):
				result = (d1 < d2 ? TRUE : FALSE);
				break;
			case (IfTests::LessThanEqualTo):
				result = (d1 <= d2 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThan):
				result = (d1 > d2 ? TRUE : FALSE);
				break;
			case (IfTests::GreaterThanEqualTo):
				result = (d1 >= d2 ? TRUE : FALSE);
				break;
			case (IfTests::NotEqualTo):
				msg.print("Warning: Comparing between floating point values...\n");
				result = (d1 != d2 ? TRUE : FALSE);
				break;
		}
		msg.print(Messenger::Commands, "If Test: var1(%s)=[%f] (%s) var2(%s)=[%f] : %s\n", v1->name(), d1, IfTests::ifTest(ifTest_), v2->name(), d2, result ? "True" : "False");
	}
	//printf("IF TEST : [%s] [%i] [%s] = %s\n",value1,type,value2,(result ? "TRUE" : "FALSE"));
	msg.exit("Command::ifEvaluate");
	return result;
}

// Add variables to command
bool Command::addVariables(const char *cmd, const char *v, VariableList &vars)
{
	msg.enter("Command::addVariables");
	bool required = TRUE, repeat = FALSE;
	int n, m, argcount, last = -1;
	Variable *var;
	VTypes::DataType vt;
	AssignOps::AssignOp ao;
	Parser::ArgumentForm af;
	static char arg[512];
	argcount = 0;
	n = 0;
	while (v[n] != '\0')
	{
		// Check for lowercase letter (optional argument)
		required = ((v[n] > 90) || (v[n] == '*') ? FALSE : TRUE);
		
		// Move on to next argument.
		argcount ++;

		//printf("Adding variable %c which should have value %s\n", v[n], parser.argc(argcount));
		// Is this a required argument?
		if (argcount > (parser.nArgs() - 1))
		{
			if (required && (!repeat))
			{
				msg.print("Error: '%s' requires argument %i\n", cmd, argcount);
				msg.exit("Command::addVariables");
				return FALSE;
			}
			else break;	// No more arguments, so may as well quit.
		}
		strcpy(arg,parser.argc(argcount));
		// Go through possible specifiers
		switch (v[n])
		{
			// Formats (delimited)
			case ('f'):
			case ('F'):
				if (!createFormat(arg, vars, TRUE)) return FALSE;
				break;
			// Formats (exact)
			case ('g'):
			case ('G'):
				if (!createFormat(arg, vars, FALSE)) return FALSE;
				break;
			// Delimited (J) / exact (K) format *or* variable
			case ('J'):
			case ('K'):
				if (!parser.wasQuoted(argcount))
				{
					// See if it has been declared
					var = parent_->variables.get(&arg[1]);
					if (var == NULL)
					{
						msg.print("Error: Variable '%s' has not been declared.\n", &arg[1]);
						return FALSE;
					}
					else args_.add(var);
				}
				else if (!createFormat(arg, vars, v[n] == 'J' ? TRUE : FALSE)) return FALSE;
				break;
			// Discard
			case ('x'):
			case ('X'):
				args_.add(NULL);
				break;
			// String as-is
			case ('s'):
			case ('S'):
				var = vars.addConstant(arg, TRUE);
				args_.add(var);
				break;
			// Operators
			case ('O'):
			case ('~'):
			case ('='):
				// Get operator enum
				ao = AssignOps::assignOp(&arg[0]);
				if (ao == AssignOps::nAssignOps)
				{
					msg.print("Error: Unrecognised assignment operator '%s'.\n", &arg[0]);
					return FALSE;
				}
				// Whether we accept the operator we found depends on the specifier
				switch (v[n])
				{
					// 'O' - accept any
					case ('O'):
						break;
					// '=' - accept '=' only
					case ('='):
						if (ao != AssignOps::Equals) 
						{
							msg.print("Error: Expected '=' as argument %i for command '%s'.\n", argcount, cmd);
							return FALSE;
						}
						break;
					// '~' - accept '=' and '+=' only
					case ('~'):
						if ((ao != AssignOps::Equals) && (ao != AssignOps::PlusEquals)) 
						{
							msg.print("Error: Expected '=' or '+=' as argument %i for command '%s'.\n", argcount, cmd);
							return FALSE;
						}
						break;
				}
				// Add operator as an integer variable
				args_.add(parent_->variables.addConstant(ao));
				break;
			// Variable, expression, or constant
			case ('e'):
			case ('E'):
				// Get form of argument in parser object
				af = parser.argumentForm(argcount);
				if (af == Parser::ExpressionForm)
				{
					var = parent_->variables.addExpression(&arg[0]);
					if (var == NULL) return FALSE;
					args_.add(var);
				}
				else if (af == Parser::VariableForm)
				{
					// See if it has been declared
					var = parent_->variables.get(&arg[1]);
					if (var == NULL)
					{
						msg.print("Error: Variable '%s' has not been declared.\n", &arg[1]);
						return FALSE;
					}
					else args_.add(var);
				}
				else args_.add(parent_->variables.addConstant(arg));
				break;
			// Normal, non-expression variable or constant (Q forces constant type to be Character)
			case ('n'):
			case ('N'):
			case ('q'):
			case ('Q'):
				if (arg[0] == '$')
				{
					// See if it has been declared
					var = parent_->variables.get(&arg[1]);
					if (var == NULL)
					{
						msg.print("Error: Variable '%s' has not been declared.\n", &arg[1]);
						return FALSE;
					}
					else args_.add(var);
				}
				else
				{
					if ((v[n] == 'q') || (v[n] == 'Q')) args_.add(parent_->variables.addConstant(arg), TRUE);
					else args_.add(parent_->variables.addConstant(arg));
				}
				break;
			// Variable ('U' indicates a pointer variable having subvariables)
			case ('v'):
			case ('V'):
			case ('u'):
			case ('U'):
				// Get form of argument in parser object
				af = parser.argumentForm(argcount);
				if (af == Parser::ExpressionForm)
				{
					msg.print("Error: argument %i to '%s' cannot be an expression (found '%s').\n", cmd, argcount, &arg[0]);
					return FALSE;
				}
				else if (af == Parser::VariableForm)
				{
					// See if it has been declared
					var = parent_->variables.get(&arg[1]);
					if (var == NULL)
					{
						msg.print( "Error: Variable '%s' has not been declared.\n", &arg[1]);
						return FALSE;
					}
					else
					{
						args_.add(var);
						// Create subvariables if necessary
						// Create extra variables in the command structure
// 						if (!parent_->createSubvariables(var)) return FALSE; TGAY
					}
				}
				else
				{
					msg.print("Error: '%s' expected a declared variable for argument %i, but found '%s' instead.\n", cmd, argcount, &arg[0]);
					return FALSE;
				}
				break;
			// Pointer-style variable (that also need to create subvariables)
			case ('A'):
			case ('a'):
			case ('B'):
			case ('b'):
			case ('P'):
			case ('p'):
			case ('M'):
			case ('m'):
				if (arg[0] != '$')
				{
					switch (v[n])
					{
						case ('A'):
						case ('a'):
							vt = VTypes::AtomData;
							break;
						case ('B'):
						case ('b'):
							vt = VTypes::BondData;
							break;
						case ('P'):
						case ('p'):
							vt = VTypes::PatternData;
							break;
						case ('M'):
						case ('m'):
							vt = VTypes::ModelData;
							break;
						default:
							vt = VTypes::nDataTypes;
							break;
					}
					msg.print( "Error: '%s' expected a variable of type '%s', but found '%s' instead.\n", cmd, VTypes::dataType(vt), &arg[0]);
					return FALSE;
				}
				// See if it has been declared
				var = parent_->variables.get(&arg[1]);
				if (var == NULL)
				{
					msg.print("Error: Variable '%s' has not been declared.\n", &arg[1]);
					return FALSE;
				}
				else args_.add(var);
				// Create extra variables in the command structure
// 				if (!parent_->createSubvariables(var)) return FALSE; TGAY
				break;
			// Character variable
			case ('C'):
				if (arg[0] != '$')
				{
					msg.print("Error: '%s' expected a variable of type 'character', but found '%s' instead.\n", cmd, &arg[0]);
					return FALSE;
				}
				// See if it has been declared
				var = parent_->variables.get(&arg[1]);
				if (var == NULL)
				{
					msg.print( "Error: Variable '%s' has not been declared.\n", &arg[1]);
					return FALSE;
				}
				else if (var->type() != VTypes::CharacterData)
				{
					msg.print("Error: '%s' expected a variable of type 'character', but found '%s' which is of type '%s'.\n", cmd, &arg[1], VTypes::dataType(var->type()));
					return FALSE;
				}
				else args_.add(var);
				break;
			// Repeat as many of the last variable type as possible
			case ('*'):
				if (n == 0)
				{
					printf("Internal error: Repeat specifier given to command arguments list without prior specifier.\n");
					msg.exit("Command::addVariables");
					return FALSE;
				}
				// Set the repeat flag to TRUE, and go back to last parameter type considered
				repeat = TRUE;
				n --;
				// Decrement argcount so we consider again the current argument
				argcount --;
				break;
		}
		// Go to next character (if we're not repeating)
		if (!repeat) n++;
	}
	// Are there still unused arguments in the parser?
	if (argcount < (parser.nArgs() - 1))
	{
		msg.print("Error: Unexpected argument '%s' given to command '%s'.\n", parser.argc(++argcount), cmd);
		msg.exit("Command::addVariables");
		return FALSE;
	}
	msg.exit("Command::addVariables");
	return TRUE;
}

// Return number of arguments given to command
int Command::nArgs()
{
	return args_.nItems();
}

/*
// CommandList
*/

// Set name of CommandList
void CommandList::setName(const char *s)
{
	name_ = s;
}

// Return name of CommandList
const char *CommandList::name()
{
	return name_.get();
}

// Return filename of CommandList
const char *CommandList::scriptFilename()
{
	return scriptFilename_.get();
}

// Return size of branch stack
int CommandList::nBranches()
{
	return branchStack_.nItems();
}

// Set parent filter
void CommandList::setFilter(Filter *f)
{
	parentFilter_ = f;
}

// Return parent filter
Filter *CommandList::filter()
{
	return parentFilter_;
}

// Push branch on to stack
void CommandList::pushBranch(List<Command> *branch, CommandAction ca, Command *basenode)
{
	branchStack_.add(branch);
	Command *cn = branchCommandStack_.add();
	cn->setCommand(ca);
	cn->setPointer(basenode);
	cn->setParent(this);
}

// Pop topmost branch on stack
void CommandList::popBranch()
{
	if (branchStack_.nItems() == 0)
	{
		printf("CommandList::popBranch <<<< No branches in branch list! >>>>\n");
		return;
	}
	branchStack_.remove(branchStack_.last());
	branchCommandStack_.remove(branchCommandStack_.last());
}

// Return basic command type of topmost branch
CommandAction CommandList::topBranchType()
{
	if (branchCommandStack_.nItems() == 0)
	{
		printf("CommandList::topBranchType <<<< No branches in branch list! >>>>\n");
		return CA_NITEMS;
	}
	else return branchCommandStack_.last()->command();
}

// Return base node of topmost branch
Command* CommandList::topBranchBaseNode()
{
	if (branchCommandStack_.nItems() == 0)
	{
		printf("CommandList::topBranchBaseNode <<<< No branches in branch list! >>>>\n");
		return NULL;
	}
	else return branchCommandStack_.last()->pointer();
}

// Add command to topmost branch
Command* CommandList::addTopBranchCommand(CommandAction ca, Command *nodeptr)
{
	if (branchStack_.nItems() == 0)
	{
		printf("CommandList::addTopBranchCommand <<<< No branches in branch list! >>>>\n");
		return NULL;
	}
	Command *cn = branchStack_.last()->item->add();
	cn->setCommand(ca);
	cn->setPointer(nodeptr);
	cn->setParent(this);
	return cn;
}

// Return basenode of topmost branch of specified type in current stack (if any)
Command *CommandList::topmostBranch(CommandAction ca)
{
	Command *result;
	for (result = branchCommandStack_.last(); result != NULL; result = result->prev)
		if (result->command() == ca) break;
	return result->pointer();
}

// // Create subvariables for variable (based on its type)
// bool CommandList::createSubvariables(Variable *v)
// {
// 	switch (v->type())
// 	{
// 		case (VTypes::AtomData):
// 			if (!createAtomVariables( v->name() )) return FALSE;
// 			break;
// 		case (VTypes::PatternData):
// 			if (!createPatternVariables( v->name() )) return FALSE;
// 			break;
// 		case (VTypes::GridData):
// 			if (!createGridVariables( v->name() )) return FALSE;
// 			break;
// 		case (VTypes::ModelData):
// 			if (!createModelVariables( v->name() )) return FALSE;
// 			break;
// 		case (VTypes::BondData):
// 		case (VTypes::AngleData):
// 		case (VTypes::TorsionData):
// 			if (!createPatternBoundVariables(v->name())) return FALSE;
// 			break;
// 		case (VTypes::AtomtypeData):
// 			if (!createAtomtypeVariables(v->name())) return FALSE;
// 			break;
// 	}
// 	return TRUE;
// }

// // Set subvariables for variable (based on its type)
// void CommandList::setSubvariables(Variable *v)
// {
// 	switch (v->type())
// 	{
// 		case (VTypes::AtomData):
// 			setAtomVariables( v->name(), (Atom*) v->asPointer() );
// 			break;
// 		case (VTypes::PatternData):
// 			setPatternVariables( v->name(), (Pattern*) v->asPointer() );
// 			break;
// 		case (VTypes::ModelData):
// 			setModelVariables( v->name(), (Model*) v->asPointer() );
// 			break;
// 		case (VTypes::BondData):
// 		case (VTypes::AngleData):
// 		case (VTypes::TorsionData):
// 			setPatternBoundVariables( v->name(), (PatternBound*) v->asPointer() );
// 			break;
// 		case (VTypes::AtomtypeData):
// 			setAtomtypeVariables( v->name(), (ForcefieldAtom*) v->asPointer() );
// 			break;
// 	}
// }

// Add basic command
bool CommandList::addCommand(CommandAction ca)
{
	msg.enter("CommandList::addCommand");
	Command *c, *topc;
	CommandAction branchca;
	VTypes::DataType vt;
	int n;
	Variable *v;
	bool result = TRUE, varresult = TRUE;
	switch (ca)
	{
		/*
		// Variable Declaration
		// All arguments to commands are names of variables to create
		*/
		case (CA_CHAR):
		case (CA_INT):
		case (CA_FLOAT):
		case (CA_ATOM):
		case (CA_PATTERN):
		case (CA_MODEL):
		case (CA_BOND):
		case (CA_ANGLE):
		case (CA_TORSION):
		case (CA_ATOMTYPE):
			for (n=1; n<parser.nArgs(); n++)
			{
				// Check for existing variable with same name
				v = variables.get(parser.argc(n));
				if (v != NULL)
				{
					printf("Error: Variable '%s': redeclared as type '%s' (from '%s').\n", parser.argc(n), VTypes::dataType((VTypes::DataType) ca),  VTypes::dataType(v->type()));
					result = FALSE;
				}
				else
				{
					vt = (VTypes::DataType) ca;
					v = variables.addVariable(parser.argc(n), vt);
				}
			}
			break;
		// 'If' statement (if 'x condition y')
		case (CA_IF):
			c = addTopBranchCommand(CA_IF, NULL);
			pushBranch(c->createBranch(), CA_IF, c);
			varresult = c->addVariables(CA_data[ca].keyword, CA_data[ca].arguments, variables);
			if (!c->setIfTest(parser.argc(2))) result = FALSE;
			break;
		// 'Else If' statement (acts as CA_END to previous 'if' or 'elseif' branch.
		case (CA_ELSEIF):
			// If the previous branch was an 'if' or 'elseif', set the *ptr of that node to this node
			branchca = topBranchType();
			if ((branchca != CA_IF) && (branchca != CA_ELSEIF))
			{
				msg.print("Error: 'elseif' used without previous if/elseif.\n");
				result = FALSE;
				break;
			}
			// Add GOTONONIF command to topmost branch to end the if sequence
			c = addTopBranchCommand(CA_GOTONONIF, topBranchBaseNode());
			// Pop topmost (previous IF/ELSEIF) branch
			popBranch();
			// Add new command node to new topmost branch and get variables
			c = addTopBranchCommand(CA_ELSEIF, NULL);
			//printf("New node is %li, command = %s\n",c,CA_keywords[cmd]);
			// Add new branch to this node for new if test to run
			pushBranch(c->createBranch(), CA_ELSEIF, c);
			varresult = c->addVariables(CA_data[ca].keyword, CA_data[ca].arguments, variables);
			if (!c->setIfTest(parser.argc(2))) result = FALSE;
			break;
		// 'Else' statement (acts as CA_END to previous 'if' or 'elseif' branch.
		case (CA_ELSE):
			// If the previous branch was an 'if' or 'elseif', set the *ptr of that node to this node
			branchca = topBranchType();
			if ((branchca != CA_IF) && (branchca != CA_ELSEIF))
			{
				msg.print("Error: 'else' used without previous if/elseif.\n");
				result = FALSE;
				break;
			}
			// Add GOTONONIF command to current topbranch to terminate that branch
			c = addTopBranchCommand(CA_GOTONONIF, topBranchBaseNode());
			// Pop previous branch from stack and add new command to new topmost branch
			popBranch();
			// Add new node to new top branch
			c = addTopBranchCommand(CA_ELSE, NULL);
			//printf("New node is %li, command = %s\n",c,CA_keywords[cmd]);
			// Add new branch to this node for new if test to run
			pushBranch(c->createBranch(), CA_ELSE, c);
			break;
		// Loop for n iterations (or until file ends) or over items
		case (CA_FOR):
			c = addTopBranchCommand(CA_FOR, NULL);
			pushBranch(c->createBranch(), ca, c);
			varresult = c->addVariables(CA_data[ca].keyword, CA_data[ca].arguments, variables);
			// Create subvariables if necessary
// 			if (varresult) varresult = createSubvariables(c->arg(0)); TGAY
			break;
		// End the topmost branch in the stack
		case (CA_END):
			if (branchStack_.nItems() == 0)
			{
				msg.print("Error: 'end' does not end a block.\n");
				result = FALSE;
				break;
			}
			// Check command stack to choose list ending pointer
			branchca = topBranchType();
			switch (branchca)
			{
				// For repeats, jump back to node at start of loop (the branch owner)
				case (CA_FOR):
					addTopBranchCommand(CA_GOTO, topBranchBaseNode());
					break;
				// For IFs, jump to node containing IF/ELSEIF/ELSE branch (the branch owner)
				case (CA_IF):
				case (CA_ELSEIF):
				case (CA_ELSE):
					addTopBranchCommand(CA_GOTONONIF, topBranchBaseNode());
					break;
				case (CA_ROOTNODE):
					addTopBranchCommand(CA_TERMINATE, NULL);
					break;
				default:
					printf("CommandList::addCommand <<<< No END action defined for command '%s' >>>>\n", CA_data[branchca].keyword);
					result = FALSE;
					break;
			}
			// Remove the topmost branch from the stack
			popBranch();
			break;
		// Break out from current loop
		case (CA_BREAK):
			// Find the topmost FOR branch
			topc = topmostBranch(CA_FOR);
			if (topc == NULL)
			{
				msg.print("Error: no loop for 'break' to terminate.\n");
				result = FALSE;
				break;
			}
			c = addTopBranchCommand(CA_BREAK, topc);
			break;
		// Cycle current loop
		case (CA_CONTINUE):
			// Find the topmost FOR branch
			topc = topmostBranch(CA_FOR);
			if (topc == NULL)
			{
				msg.print("Error: no loop for 'cycle' to iterate.\n");
				result = FALSE;
				break;
			}
			c = addTopBranchCommand(CA_CONTINUE, topc);
			break;
		// Unrecognised command
		case (CA_NITEMS):
			printf("Internal error: Unrecognised command in CommandList::addCommand()\n");
			result = FALSE;
			break;
		// All other commands do not alter the flow of the CommandList...
		default:
			c = addTopBranchCommand(ca, NULL);
			varresult = c->addVariables(CA_data[ca].keyword, CA_data[ca].arguments, variables);
			break;
	}
	// Check variable assignment result
	if (!varresult)
	{
		//msg.print("Error: Command '%s' was not given the correct variables.\n", CA_data[ca].keyword);
		result = FALSE;
	}
	msg.exit("CommandList::addCommand");
	return result;
}

// Cache script commands from line containing semicolon-separated commands
bool CommandList::cacheLine(const char *s)
{
	msg.enter("CommandList::cacheLine");
	// Use a local parser to split up the semi-colon'd line into individual commands
	static Parser lines;
	lines.getLinesDelim(s);
	for (int n=0; n<lines.nArgs(); n++)
	{
		// Parse the argument in our local line_parser and call cache_command())
		parser.getArgsDelim(lines.argc(n), Parser::UseQuotes+Parser::SkipBlanks);
		if (!cacheCommand())
		{
			msg.exit("CommandList::cacheLine");
			return FALSE;
		}
	}
	msg.exit("CommandList::cacheLine");
	return TRUE;
}

// Cache command arguments in global Parser object
bool CommandList::cacheCommand()
{
	msg.enter("CommandList::cacheCommand");
	CommandAction ca;
	bool result = TRUE, addcmd = FALSE;
	/*
	Assume that the main parser object contains the data we require.
	If there is no argument 0 in the parser, then just return true. Otherwise, check for the first argument being a variable (denoted by a '$') and the second being an '='. If so, move the first argument to the second and add a CA_LET2 command.
	*/
	if (parser.isBlank(0)) result = TRUE;
	else if (parser.argc(0)[0] == '$')
	{
		// Shift all arguments up one position in the parser
		parser.shiftArgsUp();
		// Set command based on type of variable
		Variable *v = variables.get(&parser.argc(1)[1]);
		if (v == NULL)
		{
			msg.print( "Variable '%s' has not been declared.\n", &parser.argc(1)[1]);
			result = FALSE;
		}
		else
		{
			// The command we use depends on the type of the target variable
			addcmd = TRUE;
			switch (v->type())
			{
				case (VTypes::CharacterData):
					addcmd = addCommand(CA_LETCHAR);
					break;
				case (VTypes::IntegerData):
				case (VTypes::RealData):
					addcmd = addCommand(CA_LET);
					break;
				default:
					addcmd = addCommand(CA_LETPTR);
					break;
			}
			if (!addcmd)
			{
				msg.print( "Error adding variable assignment command.\n");
				result = FALSE;
			}
		}
	}
	else
	{
		ca = CA_from_text(parser.argc(0));
		if (ca != CA_NITEMS)
		{
			// If addCommand() returns FALSE then we encountered an error
			if (!addCommand(ca))
			{
				//msg.print("Error parsing command '%s'.\n", parser.argc(0));
				msg.print("Command usage is: %s %s\n", CA_data[ca].keyword, CA_data[ca].argText);
				result = FALSE;
			}
		}
		else
		{
			msg.print("Unrecognised command '%s'.\n", parser.argc(0));
			result = FALSE;
		}
	}
	msg.exit("CommandList::cacheCommand");
	return result;
}

/*
// Files
*/

// Get input stream
ifstream *CommandList::inputFile()
{
	return inputFile_;
}

// Get output stream
ofstream *CommandList::outputFile()
{
	return outputFile_;
}

// Return filename associated to infile/outfile
const char *CommandList::filename()
{
	return filename_.get();
}

// Add read option
void CommandList::addReadOption(Parser::ParseOption po)
{
	if (!(readOptions_&po)) readOptions_ += po;
}

// Remove read option
void CommandList::removeReadOption(Parser::ParseOption po)
{
	if (readOptions_&po) readOptions_ -= po;
}

// Return read options
int CommandList::readOptions()
{
	return readOptions_;
}

// Load commands from file
bool CommandList::load(const char *filename)
{
	msg.enter("CommandList::load");
	scriptFilename_ = filename;
	ifstream cmdfile(filename,ios::in);
	int success;
	clear();
	name_ = filename;
	// Read in commands
	while (!cmdfile.eof())
	{
		success = parser.getArgsDelim(&cmdfile,Parser::UseQuotes+Parser::SkipBlanks);
		if (success == 1)
		{
			msg.print("CommandList::load - Error reading command file.\n");
			msg.exit("CommandList::load");
			return FALSE;
		}
		else if (success == -1) break;
		// Attempt to cache the command
		if (cacheCommand()) continue;
		else
		{
			msg.exit("CommandList::load");
			return FALSE;
		}
	}
	// Check the flowstack - it should be empty...
	int itemsleft = branchStack_.nItems();
	if (itemsleft != 1)
	{
		printf("CommandList::load <<<< %i block%s not been terminated >>>>\n", itemsleft, (itemsleft == 1 ? " has" : "s have"));
		msg.exit("CommandList::load");
		return FALSE;
	}
	msg.exit("CommandList::load");
	return TRUE;
}

// Set input file (pointer)
bool CommandList::setInputFile(const char *sourcefile)
{
	msg.enter("CommandList::setInFile");
	if (inputFile_ != NULL) printf("CommandList::setInFile <<<< Inputfile already set >>>>\n");
        inputFile_ = new ifstream(sourcefile,ios::in);
	filename_ = sourcefile;
	msg.exit("CommandList::setInFile");
	return (!inputFile_->good() ? FALSE : TRUE);
}

// Set output file
bool CommandList::setOutputFile(const char *destfile)
{
	msg.enter("CommandList::setOutputFile");
	outputFile_ = new ofstream(destfile,ios::out);
	filename_ = destfile;
	msg.exit("CommandList::setOutputFile");
	if (!outputFile_->good()) return FALSE;
	else return TRUE;
}

// Close files
void CommandList::closeFiles()
{
	msg.enter("CommandList::closeFiles");
	if (inputFile_ != NULL)
	{
		inputFile_->close();
		delete inputFile_;
	}
	if (outputFile_ != NULL)
	{
		outputFile_->close();
		delete outputFile_;
	}
	inputFile_ = NULL;
	outputFile_ = NULL;
	msg.exit("CommandList::closeFiles");
}

// Execute command
int Command::execute(Command *&c)
{
	// Make sure the current rendersource is up-to-date
	aten.current.rs = (aten.current.m == NULL ? NULL : aten.current.m->renderSource());
	return CALL_COMMAND(CA_data[action_],function_)(c, aten.current);
}

// Execute commands in command list
bool CommandList::execute(ifstream *sourcefile)
{
	msg.enter("CommandList::execute");
	// Set alternative input file if one was supplied
	if (sourcefile != NULL)
	{
		if ((inputFile_ != NULL) && (inputFile_ != sourcefile)) printf("Warning - supplied ifstream overrides file in CommandList.\n");
		inputFile_ = sourcefile;
	}
	static bool result;
	result = TRUE;
	// Get first command in list and execute
	Command *c = commands_.first();
	while (c != NULL)
	{
		// Run command and get return value
		msg.print(Messenger::Commands, "Commandlist executing command '%s'...\n",CA_data[c->command()].keyword);
		switch (c->execute(c))
		{
			// Command succeeded - get following command
			case (CR_SUCCESS):
				c = c->next;
				break;
			// Command succeeded - new command already set
			case (CR_SUCCESSNOMOVE):
				break;
			// Command failed - show message and quit
			case (CR_FAIL):
				printf("Command list failed at '%s'.\n", CA_data[c->command()].keyword);
				c = NULL;
				result = FALSE;
				break;
			// Command failed - show message and continue to next command
			case (CR_FAILCONTINUE):
				printf("Continuing past failed command '%s'...\n", CA_data[c->command()].keyword);
				c = c->next;
				break;
			// Exit with error
			case (CR_EXITWITHERROR):
				c = NULL;
				result = FALSE;
				break;
			// Exit - we're done
			case (CR_EXIT):
				c = NULL;
				break;
		}
	}
	msg.exit("CommandList::execute");
	return result;
}
