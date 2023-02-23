#pragma once

#include "Definitions.h"
#include "Includes.h"
#include "Utils.h"
#include "LCU.h"
#include "Misc.h"
#include "Config.h"

class SettingsTab
{
public:

	static void Render()
	{
		typedef LSTATUS(WINAPI* tRegCreateKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass,
			DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
		static tRegCreateKeyExA RegCreateKeyExA = (tRegCreateKeyExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegCreateKeyExA");

		typedef LSTATUS(WINAPI* tRegOpenKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
			REGSAM samDesired, PHKEY phkResult);
		static tRegOpenKeyExA RegOpenKeyExA = (tRegOpenKeyExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegOpenKeyExA");

		typedef LSTATUS(WINAPI* tRegQueryValueExA)(HKEY hKey, LPCSTR lpValueName,
			LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbDatan);
		static tRegQueryValueExA RegQueryValueExA = (tRegQueryValueExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegQueryValueExA");

		typedef LSTATUS(WINAPI* tRegSetValueExA)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved,
			DWORD dwType, const BYTE* lpData, DWORD cbData);
		static tRegSetValueExA RegSetValueExA = (tRegSetValueExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegSetValueExA");

		typedef LSTATUS(WINAPI* tRegDeleteValueA)(HKEY hKey, LPCSTR lpValueName);
		static tRegDeleteValueA RegDeleteValueA = (tRegDeleteValueA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegDeleteValueA");

		typedef LSTATUS(WINAPI* tRegCloseKey)(HKEY hKe);
		static tRegCloseKey RegCloseKey = (tRegCloseKey)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegCloseKey");

		static bool once = true;
		if (ImGui::BeginTabItem("Settings"))
		{
			if (once)
			{
				once = false;
				HKEY hkResult;
				if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\LeagueClientUx.exe", 0, KEY_READ, &hkResult) == ERROR_SUCCESS)
				{
					char buffer[MAX_PATH];
					DWORD dwLen = sizeof(buffer);
					LSTATUS regQuery = RegQueryValueExA(hkResult, "debugger", 0, NULL, (LPBYTE)buffer, &dwLen);
					if (regQuery == ERROR_SUCCESS)
					{
						S.currentDebugger = std::string(buffer, dwLen);
					}
					else if (regQuery == ERROR_FILE_NOT_FOUND)
					{
						S.currentDebugger = "Nothing";
						S.debugger = false;
					}
					else
					{
						S.currentDebugger = "Failed, error code " + regQuery;
					}
					RegCloseKey(hkResult);
				}
				else
					S.currentDebugger = "Error";
			}
			static std::string result;

			ImGui::Checkbox("Auto-rename", &S.autoRename);
			ImGui::SameLine();
			ImGui::HelpMarker("Automatically renames the program on launch");

			if (ImGui::Checkbox("Stream proof", &S.streamProof))
			{
				if (S.streamProof)
					SetWindowDisplayAffinity(S.hwnd, WDA_EXCLUDEFROMCAPTURE);
				else
					SetWindowDisplayAffinity(S.hwnd, WDA_NONE);
			}
			ImGui::SameLine();
			ImGui::HelpMarker("Hides the program in recordings and screenshots");

			ImGui::Checkbox("Launch client without admin", &S.noAdmin);

			if (ImGui::Checkbox("Register debugger IFEO", &S.debugger))
			{
				HKEY hkResult;
				LSTATUS regCreate = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
					"Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\LeagueClientUx.exe",
					0, 0, 0, KEY_SET_VALUE | KEY_QUERY_VALUE | KEY_CREATE_SUB_KEY, 0, &hkResult, 0);
				if (regCreate == ERROR_SUCCESS)
				{
					char* buffer[MAX_PATH];
					DWORD bufferLen = sizeof(buffer);

					char filePath[MAX_PATH + 1];
					GetModuleFileNameA(NULL, filePath, MAX_PATH);
					DWORD len = (DWORD)(strlen(filePath) + 1);

					LSTATUS regQuery = RegQueryValueExA(hkResult, "debugger", 0, 0, (LPBYTE)buffer, &bufferLen);
					if (regQuery == ERROR_SUCCESS || regQuery == ERROR_FILE_NOT_FOUND)
					{
						if (S.debugger)
						{
							auto messageBoxStatus = IDYES;
							if (S.autoRename || S.noAdmin)
								messageBoxStatus = MessageBoxA(0, "Having \"Auto-rename\" or \"Launch client without admin\" "
									"enabled with \"debugger IFEO\" will prevent League client from starting\n\n"
									"Do you wish to continue?", "Warning", MB_YESNO | MB_SETFOREGROUND);

							if (messageBoxStatus == IDYES)
							{
								if (RegSetValueExA(hkResult, "debugger", 0, REG_SZ, (const BYTE*)filePath, len) == ERROR_SUCCESS)
								{
									S.currentDebugger = filePath;
								}
							}
							else
							{
								S.debugger = false;
							}
						}
						else if (regQuery == ERROR_SUCCESS)
						{
							RegDeleteValueA(hkResult, "debugger");
							S.currentDebugger = "Nothing";
						}
					}
					RegCloseKey(hkResult);
				}
			}
			ImGui::SameLine();
			ImGui::HelpMarker("Allows for client traffic analysis via a web debugging proxy such as Fiddler."
				"Disable before deleting the program. Doesn't work when \"Auto-rename\" or \"Launch client without admin\" are enabled.");
			ImGui::SameLine();
			ImGui::Text("| Hooked to: %s", S.currentDebugger.c_str());

			if (ImGui::Button("Clean logs"))
			{
				if (MessageBoxA(0, "Are you sure?", "Cleaning logs", MB_OKCANCEL) == IDOK)
				{
					result = Misc::ClearLogs();
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Force close client"))
				Misc::TaskKillLeague();

			ImGui::SameLine();
			if (ImGui::Button("Internet Properties Repair"))
				RepairInternetProperties();

			static char bufLeaguePath[MAX_PATH];
			std::copy(S.leaguePath.begin(), S.leaguePath.end(), bufLeaguePath);
			ImGui::Text("League path:");
			ImGui::InputText("##leaguePath", bufLeaguePath, MAX_PATH);
			S.leaguePath = bufLeaguePath;

			/*	if (ImGui::Button("Save Settings"))
				{
					CSettings::Save();
					result = "Saved";
				}*/

			ImGui::Separator();

			ImGui::Text("Font Scale:");
			if (ImGui::SliderFloat("##fontScaleSlider", &S.fontScale, 0.4f, 4.f, "%0.1f"))
			{
				ImGuiIO& io = ImGui::GetIO();
				io.FontGlobalScale = S.fontScale;
			}

			if (ImGui::Button("Reset window size"))
			{
				S.Window.width = 730;
				S.Window.height = 530;
				::SetWindowPos(S.hwnd, 0, 0, 0, S.Window.width, S.Window.height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				Config::Save();
			}
			ImGui::SameLine();
			ImGui::Text(std::format("{0}x{1}", S.Window.width, S.Window.height).c_str());

			ImGui::Separator();
			ImGui::Text("Program's version: %s | Latest version: %s",
				Misc::programVersion.c_str(), Misc::latestVersion.c_str());
			ImGui::Text("GitHub repository:");
			ImGui::TextURL("Click me!", "https://github.com/BennyExtreme/KBotExt", 1, 0);

			if (!result.empty())
				ImGui::Separator();
			ImGui::TextWrapped(result.c_str());

			ImGui::EndTabItem();
		}
		else
			once = true;
	}

private:

	static void RepairInternetProperties() {
		typedef LSTATUS(WINAPI* tRegOpenKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
			REGSAM samDesired, PHKEY phkResult);
		static tRegOpenKeyExA RegOpenKeyExA = (tRegOpenKeyExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegOpenKeyExA");

		typedef LSTATUS(WINAPI* tRegSetValueExA)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved,
			DWORD dwType, const BYTE* lpData, DWORD cbData);
		static tRegSetValueExA RegSetValueExA = (tRegSetValueExA)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegSetValueExA");

		typedef LSTATUS(WINAPI* tRegCloseKey)(HKEY hKe);
		static tRegCloseKey RegCloseKey = (tRegCloseKey)GetProcAddress(LoadLibraryW(L"advapi32.dll"), "RegCloseKey");

		HKEY hkResult;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ | KEY_WRITE, &hkResult) == ERROR_SUCCESS)
		{
			const static std::vector<std::string> regDWORDKeys = {
				"BackgroundConnections",
				"CertificateRevocation",
				"CoInternetCombineIUriCacheSize",
				"CreateUriCacheSize",
				"DisableCachingOfSSLPages",
				"DisableIDNPrompt",
				"EnableAutodial",
				"EnableHttp1_1",
				"EnableHTTP2",
				"EnableNegotiate",
				"EnablePunycode",
				"EnableSSL3Fallback",
				"MigrateProxy",
				"NoNetAutodial",
				"PreventIgnoreCertErrors",
				"PrivacyAdvanced",
				"PrivDiscUiShown",
				"ProxyEnable",
				"ProxyHttp1.1",
				"SecureProtocols",
				"SecurityIdIUriCacheSize",
				"ShowPunycode",
				"SpecialFoldersCacheSize",
				"SyncMode5",
				"UrlEncoding",
				"WarnonBadCertRecving",
				"WarnOnIntranet",
				"WarnOnPostRedirect",
				"WarnonZoneCrossing"
			};
			const static std::vector<DWORD> regDWORDValues = { 1, 1, 0x50, 0x50, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0xA80, 0x1E, 0, 8, 4, 0, 1, 1, 1, 0 };

			DWORD value;
			DWORD dwSize = sizeof(value);
			for (int i=0; i<regDWORDKeys.size(); i++)
				RegSetValueExA(hkResult, regDWORDKeys[i].c_str(), 0, REG_DWORD, (LPBYTE)&regDWORDValues[i], dwSize);

			RegCloseKey(hkResult);

			MessageBoxA(NULL, "Internet Properties repaired successfully!\n\nPlease, restart the program.", "Internet Properties Repair", MB_OK | MB_ICONINFORMATION);
			exit(EXIT_SUCCESS);
		} else
			MessageBoxA(NULL, "An unexpected error occurred during the repair process! :(", "Internet Properties Repair", MB_OK | MB_ICONERROR);
	}
};