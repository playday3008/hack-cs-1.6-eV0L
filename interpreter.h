#ifndef _INTERPRETER_
#define _INTERPRETER_

class CommandInterpreter
{

public:
	// registering/modifying objects
	void  AddCommand  ( const char *name, void(*func)()   )
	{ 
		Add(name, Entry::COMMAND,   func); 
	}
	void  AddCvarInt  ( const char *name, int   *var      )
	{ 
		Add(name, Entry::CVAR_INT,   var); 
	}
	void  AddCvarFloat( const char *name, float *var      )
	{ 
		Add(name, Entry::CVAR_FLOAT, var); 
	}
	
	// execution
	void exec    (const char* cmdlist);
	void exec_one(const char* cmd);
	
	// Command interface functions:
	string&  argS(int i){ --i;if(i<preExecArgs.size()) return preExecArgs[i];                  else return emptyString; }
	char*	 argC(int i){ --i;if(i<preExecArgs.size()) return const_cast<char*>(preExecArgs[i].c_str()); else return "";}
	int      argI(int i){ --i;if(i<preExecArgs.size()) return atoi(preExecArgs[i].c_str());              else return  0;}
	float    argF(int i){ --i;if(i<preExecArgs.size()) return (float)atof(preExecArgs[i].c_str());    else return  0.0f;}
	void     collectArguments(string& dest,int from=1,int to=100000);

	// initializing this class:
	void init();
	CommandInterpreter(){init();}
	
	// prefix used for commands that should bypass command blocking
	union{
		char          excludePrefixChar[5]; 
		unsigned long excludePrefixDword;   
	};

	void save_cvars(ofstream& ofs);
	void load_cvars(void);

protected:
	void   Add( const char* name, int type, void* data);
	struct Entry
	{
		enum { CVAR_FLOAT, CVAR_INT, COMMAND, ALIAS, HL_CVAR };

		int   type;
		void* data; 
	};

protected:
	void extractArguments(const char* args);
	void createRandomPrefix();

	vector<Entry>  entries;
	StringFinder   names;

	vector<string> preExecArgs;
	string         emptyString;
};

void Init_Command_Interpreter();
void SaveCvar();
void LoadCvar();

extern CommandInterpreter cmd;

#endif

