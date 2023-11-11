#include <iostream>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <Windows.h>

using std::string;
using std::map;
using std::endl;
using std::cerr;
using std::cout;
using std::exception;
using std::transform;

static map<string, string> extension_commands = {
	// ADD ASSOCIATIONS TO THIS MAP
	{ "pdf", "okular" }
};

void exec(LPCSTR exe, LPSTR args)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcessA
	(
		exe,
		args,
		NULL,                // Process handle not inheritable
		NULL,                // Thread handle not inheritable
		FALSE,               // Do not handle inheritance
		CREATE_NO_WINDOW,
		NULL,                // Use parent's environment block
		NULL,                // Use parent's starting directory 
		&si,
		&pi
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int WinMain(HINSTANCE _1, HINSTANCE _2, LPSTR _3, int _4)
{
	if (!__argc || !strlen(__argv[1])) {
		cerr << "Single argument required" << endl;
		return 1;
	}

	auto target_size_incl_null = GetFullPathNameA(__argv[1], 0, NULL, NULL);
	if (!target_size_incl_null) {
		cerr << "Could not determine absolute path" << endl;
		return 3;
	}
	auto buffer = new char[target_size_incl_null];
	if (!GetFullPathNameA(__argv[1], target_size_incl_null, buffer, NULL)) {
		cerr << "Could not store absolute path" << endl;
		return 3;
	}
	string target(buffer);
	delete[] buffer;

	auto last_dot_idx = target.find_last_of('.');
	if (last_dot_idx >= target.size() - 1) {
		cerr << "File name extension missing: " << target << endl;
		return 1;
	}
	auto extension = target.substr(last_dot_idx + 1);
	transform(extension.begin(), extension.end(), extension.begin(),
		[](unsigned char c) { return std::tolower(c); });

	auto extension_command_it = extension_commands.find(extension);
	if (extension_command_it == extension_commands.end()) {
		cerr << "Extension '" << extension << "' has no association" << endl;
		return 1;
	}
	auto linux_command = extension_command_it->second;

	string wslg_arg_const = "-- bash -l -c 'WSLPATH=\"$(wslpath \"" + target + "\")\" && cd \"$(dirname \"$WSLPATH\")\" && \"" + linux_command + "\" \"$WSLPATH\"'";
	auto wslg_arg_const_size_incl_null = wslg_arg_const.size() + 1;
	LPSTR wslg_arg = new char[wslg_arg_const_size_incl_null];
	strcpy_s(wslg_arg, wslg_arg_const_size_incl_null * sizeof(wslg_arg[0]), wslg_arg_const.c_str());
	
	char wslg_exe[100];
	size_t systemroot_size_incl_null;
	if (getenv_s(&systemroot_size_incl_null, wslg_exe, "SYSTEMROOT")) {
		cerr << "Could not get SYSTEMROOT from environment" << endl;
		return 3;
	}
	if (strcpy_s(wslg_exe + systemroot_size_incl_null - 1, sizeof(wslg_exe) - systemroot_size_incl_null + 1, "\\System32\\wslg.exe")) {
		cerr << "Could not append wslg executable" << endl;
		return 3;
	}

	exec(wslg_exe, wslg_arg);
	delete[] wslg_arg;
	return 0;
}
