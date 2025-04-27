#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>

bool freeze = false;
DWORD id;
bool holdortoggle = false; /* hold is true, toggle is false */

void suspend() {
	HANDLE snHandle = NULL;
	BOOL rvBool = FALSE;
	THREADENTRY32 te32 = { 0 };

	snHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snHandle == INVALID_HANDLE_VALUE) return;
	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(snHandle, &te32)) {
		do {
			if (te32.th32OwnerProcessID == id) {
				HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
				if (h) {
					if (!freeze) {
						SuspendThread(h);
					}
					else {
						ResumeThread(h);
					}
					CloseHandle(h);
				}
			}
		} while (Thread32Next(snHandle, &te32));
	}
}

void ntsuspend() {

}

void toggle() {
	if (!id) {
		HWND hwnd = FindWindowA(NULL, "Roblox");
		GetWindowThreadProcessId(hwnd, &id);
	}

	suspend();

	freeze = !freeze;
}

int main() {
	SetConsoleTitleA("Roblox freezer");
	std::cout << "[+] Please put your keybind here: ";
	std::string inp;
	std::cin >> inp;

	SHORT keybind = 0;

	if (inp.length() == 1) { /* 1 character like F */
		keybind = VkKeyScanA(inp[0]) & 0xFF;
	}
	else if (inp.length() >= 2) { /* 2 or more characters like F6 */
		int number = std::stoi(inp.substr(1));
		keybind = VK_F1 + (number - 1);
	}

	std::cout << "[+] Would you like to toggle or hold the key? (t/h): ";
	char mode;
	std::cin >> mode;

	if (mode == 'h') {
		holdortoggle = true;
	}
	else if (mode == 't') {
		holdortoggle = false;
	}
	else {
		std::cout << "?\n";
		return 0;
	}

	std::cout << "[+] Started!\n";

	while (true) {
		if (holdortoggle) { /* hold */
			if (GetAsyncKeyState(keybind) & 0x8000) {
				if (!freeze) {
					toggle();
				}
			}
			else {
				if (freeze) {
					toggle();
				}
			}
		}
		else { /* toggle */
			if (GetAsyncKeyState(keybind) & 1) {
				toggle();
			}
		}

		Sleep(50);
	}
}