#include "client.h"

CommandInterpreter cmd;

inline string getHackDirFile(const char* basename)
{
	if (strstr(basename, "..")) { return ":*?\\/<>\""; }
	string ret = hackdir;
	return (ret + basename);
}

void HlEngineCommand(const char* command)
{
	if (!g_Engine.pfnClientCmd) { return; }
	g_Engine.pfnClientCmd(const_cast<char*>(command));
}

void HandleCvarInt(char* name, int* value)
{
	char* arg1 = cmd.argC(1);
	if (!strcmp(arg1, "change")) { if (*value)* value = 0; else *value = 1; return; }
	if (!strcmp(arg1, "up")) { *value += cmd.argI(2); return; }
	if (!strcmp(arg1, "down")) { *value -= cmd.argI(2); return; }
	if (!strcmp(arg1, "hide")) { *value = cmd.argI(2); return; }
	if (!*arg1)
	{
		ConsolePrint("CVAR &w%s&a = %i\n", name, *value);
		return;
	}
	*value = cmd.argI(1);
}

void HandleCvarFloat(char* name, float* value)
{
	char* arg1 = cmd.argC(1);
	if (!strcmp(arg1, "change")) { if (*value)* value = 0; else *value = 1; return; }
	if (!strcmp(arg1, "up")) { *value += cmd.argF(2); return; }
	if (!strcmp(arg1, "down")) { *value -= cmd.argF(2); return; }
	if (!strcmp(arg1, "hide")) { *value = cmd.argI(2); return; }
	if (!*arg1)
	{
		ConsolePrint("CVAR %s = %f\n", name, *value);
		return;
	}
	*value = cmd.argF(1);
}

bool isHlCvar(char* name)
{
	if (!g_Engine.pfnGetCvarPointer) { return false; }

	cvar_s* test = g_Engine.pfnGetCvarPointer(name);
	return (test != NULL);
}

bool HandleHlCvar(char* name)
{
	if (!g_Engine.pfnGetCvarPointer) { return false; }
	cvar_s* ptr = g_Engine.pfnGetCvarPointer(name);
	if (!ptr) { return false; }
	HandleCvarFloat(name, &ptr->value);
	return true;
}

void CommandInterpreter::exec(const char* cmdlist)
{
	string my_copy = cmdlist;
	// find end:
	char* from = const_cast<char*>(my_copy.c_str());
	char* to = from;

	while (*from == ' ' || *from == '\t') { ++from; ++to; } // skip ws
	while (*to >= ' ' && *to != ';') { // find end
		if (*to == '\"')
		{
			do { ++to; } while (*to && *to != '\"');
		}
		else
		{
			++to;
		}
	}

	do {
		// comments...
		if (from[0] == '/' && from[1] == '/') { return; }

		// split up and exec
		if (from < to)
		{
			char oldch = *to;
			*to = 0;
			exec_one(from);
			*to = oldch;
		}

		// advance
		if (!*to) { break; }
		++to;
		from = to;
		while (*from == ' ' || *from == '\t') { ++from; ++to; }  // skip ws
		while (*to >= ' ' && *to != ';') ++to;              // find end
	} while (1);
}

void CommandInterpreter::extractArguments(const char* const_args)
{
	preExecArgs.clear();

	char* args = const_cast<char*>(const_args);

	while (*args)
	{
		while (*args && *args <= ' ') args++;
		if (!*args) break;

		char* start;
		if (*args == '\"') { start = ++args; while (*args != '\"' && *args) args++; }
		else { start = args;   while (*args > ' ') args++; }

		char last = *args;
		*args = 0;
		preExecArgs.push_back(start);
		*args = last;
		if (*args == '\"') args++;
	}
}

void CommandInterpreter::collectArguments(string& dest, int from, int to)
{
	dest.erase();
	--from; --to;

	int nArgs = preExecArgs.size();
	to = (to < nArgs) ? to : (nArgs - 1);

	while (from <= to) {
		dest += preExecArgs[from];
		if (from != to) dest += " ";
		++from;
	};
}

void CommandInterpreter::init()
{ 
	createRandomPrefix(); 
}

void CommandInterpreter::createRandomPrefix()
{
	static char* characterBox = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!$%&/()=?{}[]*#-.<>~+_";
	static int len = sizeof(characterBox) - 1;

	excludePrefixChar[0] = characterBox[rand() % len];
	excludePrefixChar[1] = characterBox[rand() % len];
	excludePrefixChar[2] = characterBox[rand() % len];
	excludePrefixChar[3] = characterBox[rand() % len];
	excludePrefixChar[4] = 0;
}

void CommandInterpreter::exec_one(const char* cur_cmd)
{
	if (*cur_cmd == '#' || *cur_cmd == '.')
	{
		if (false)
		{
			static string hlcommand;
			hlcommand.erase();
			hlcommand += excludePrefixChar;
			hlcommand += (cur_cmd + 1);
			HlEngineCommand(hlcommand.c_str());
		}
		else
		{
			HlEngineCommand(cur_cmd + 1);
		}
		return;
	}

	// extract command
	char  command[32];
	char* commandPos = command;
	int   commandCharsLeft = 31;
	while (*cur_cmd > ' ' && commandCharsLeft)
	{
		*commandPos = tolower(*cur_cmd);
		commandPos++;
		cur_cmd++;
		commandCharsLeft--;
	}
	*commandPos = 0;
	while (*cur_cmd > ' ') cur_cmd++; // skip parts bigger than 31 chars.

	if (names.find(command))
	{
		Entry& entry = entries[names.num];

		switch (entry.type)
		{
		case Entry::ALIAS: {
			string& content = *(string*)(entry.data);
			exec(const_cast<char*>(content.c_str()));
		}break;
		case Entry::COMMAND: {
			typedef void(*CmdFunc)();
			CmdFunc function = (CmdFunc)(entry.data);
			extractArguments(cur_cmd);
			function();
		}break;
		case Entry::CVAR_INT:
			extractArguments(cur_cmd);
			HandleCvarInt(command, (int*)entry.data);
			break;
		case Entry::CVAR_FLOAT:
			extractArguments(cur_cmd);
			HandleCvarFloat(command, (float*)entry.data);
			break;
		case Entry::HL_CVAR:
			extractArguments(cur_cmd);
			HandleHlCvar(command);
			break;
		}
	}
	else 
	{
		if (!isHlCvar(command))
		{
			ConsolePrint("Unknown command: %s", command);
		}
		else
		{
			Add(command, Entry::HL_CVAR, NULL);
			extractArguments(cur_cmd);
			HandleHlCvar(command);
		}
	}
}

void CommandInterpreter::Add(const char* name, int type, void* data)
{
	if (names.find(name))
	{
		ConsolePrint("%s is already registered.\n", name);
		return;
	}

	int index = entries.size();
	Entry tmp = { type,data };
	entries.push_back(tmp);
	names.add(name, index);
}

void CommandInterpreter::save_cvars(ofstream& ofs)
{
	ofs << "[CVAR]\n";

	for (names.it_start(); names.it_running(); names.it_next())
	{
		Entry& r = entries[names.num];
		if (r.type == Entry::CVAR_FLOAT)	ofs << names.str << "=" << *(float*)r.data << "\n";
	}
}

void CommandInterpreter::load_cvars(void)
{
	char cvar_f[100];
	DWORD size = 500;

	for (names.it_start(); names.it_running(); names.it_next())
	{
		Entry& r = entries[names.num];

		if (r.type == Entry::CVAR_FLOAT)
		{
			GetPrivateProfileString("CVAR", names.str, "NULL", cvar_f, size, getHackDirFile("cvar.ini").c_str());
			*(float*)r.data = atof(cvar_f);
		}
	}
}

void SaveCvar()
{
	ofstream ofs(getHackDirFile("cvar.ini").c_str());
	cmd.save_cvars(ofs);
	ofs.close();
}

bool fileExists(const char* filename)
{
	WIN32_FIND_DATA finddata;
	HANDLE handle = FindFirstFile(filename, &finddata);
	return (handle != INVALID_HANDLE_VALUE);
}

void LoadCvar()
{
	if (!fileExists(getHackDirFile("cvar.ini").c_str())) return;
	cmd.load_cvars();
}

void Init_Command_Interpreter()
{
	cmd.init();
	cvar.init();
	LoadCvar();

	ID_HookCommands();
	RouteCommands();
}
