#include "Configurator.h"
#include "resource.h"
#include <string>
#include <fstream>
#include <commctrl.h>
#include <INIReader/INIFile.h>
#include <INIReader/INIKey.h>
#include <INIReader/INISection.h>

#include "ButtonProcessor.h"
#include "AxisProcessor.h"
#include "HATProcessor.h"

WNDPROC Configurator::ListViewProc;
WNDPROC Configurator::ListProc;
Configurator *Configurator::PThis;

ConfigRoot::ConfigRoot() {}
ConfigRoot::~ConfigRoot() {}
char *ConfigRoot::Name() { return (char *)"Input systems"; }
char *ConfigRoot::Description() { return (char *)"Configuration options for addon input systems."; }

Configurator::Configurator(HINSTANCE hInst)
{
	PThis = this;

	_HInstance = hInst;

	_PrevDeviceCount = 0;

	_AxisList = nullptr;
	_HATList = nullptr;
	_ButtonList = nullptr;
	_InputSystem = nullptr;
	_DeviceList = nullptr;
	_ThisHWND = nullptr;
	_VesselList = nullptr;

	std::wstring configPath = L"Config\\BetterJoysticks\\";
	std::string line;

	// Button List
	std::fstream file(configPath + L"Buttons.cfg", std::ios::in);
	while (std::getline(file, line))
	{
		line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) { return !std::isspace(ch); }));
		if (line.empty() || line[0] == ';') continue;
		_Buttons.push_back(new ButtonProcessor(line));
	}

	// HAT List
	file = std::fstream(configPath + L"HATs.cfg", std::ios::in);
	while (std::getline(file, line))
	{
		line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) { return !std::isspace(ch); }));
		if (line.empty() || line[0] == ';') continue;
		_HATs.push_back(new HATProcessor(line));
	}

	// Axis List
	TChapman500::INIFile iniFile(configPath + L"Axes.ini");
	_Axes.resize((size_t)iniFile.GetUInt(L"General", L"AxisCount", 0));

	ButtonProcessor *holdButtons[2];
	ButtonProcessor *slewButtons[2];
	ButtonProcessor *minMaxButtons[2];

	std::wfstream log("Log.txt", std::ios::out);

	for (size_t i = 0; i < _Axes.size(); i++)
	{
		memset(holdButtons, 0, sizeof(ButtonProcessor **) * 2);
		memset(slewButtons, 0, sizeof(ButtonProcessor **) * 2);
		memset(minMaxButtons, 0, sizeof(ButtonProcessor **) * 2);

		std::wstring wstr = iniFile.GetString(std::wstring(L"Axis_" + std::to_wstring(i)), std::wstring(L"Name"));

		std::string str;
		str.resize(wstr.length());
		for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
		std::string axisName = str;

		std::string posName = "";
		std::string negName = "";

		// Positive Hold Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"Hold.Positive"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					holdButtons[0] = button;
					break;
				}
			}
		}
		// Negative Hold Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"Hold.Negative"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					holdButtons[1] = button;
					break;
				}
			}
		}

		// Positive Name
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"PositiveName"));
		if (!wstr.empty())
		{
			posName.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) posName[c] = (unsigned char)wstr[c];
		}
		// Negative Name
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"NegativeName"));
		if (!wstr.empty())
		{
			negName.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) negName[c] = (unsigned char)wstr[c];
		}
		// Positive Slew Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"Slew.Positive"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					slewButtons[0] = button;
					break;
				}
			}
		}
		// Negative Slew Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"Slew.Negative"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					slewButtons[1] = button;
					break;
				}
			}
		}

		// Positive Min-Max Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"MinMax.Positive"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					minMaxButtons[0] = button;
					break;
				}
			}
		}
		// Negative Min-Max Button
		wstr = iniFile.GetString(std::wstring(L"Axis_" + i), std::wstring(L"MinMax.Negative"));
		if (!wstr.empty())
		{
			str.resize(wstr.length());
			for (size_t c = 0; c < wstr.length(); c++) str[c] = (unsigned char)wstr[c];
			for (ButtonProcessor *button : _Buttons)
			{
				if (button->Name == str)
				{
					minMaxButtons[1] = button;
					break;
				}
			}
		}

		// See if the axis is dual-ended
		bool dualEnded = iniFile.GetUInt(std::wstring(L"Axis_" + i), std::wstring(L"DualEnded"), 0) != 0;

		AxisProcessor *newAxis = new AxisProcessor(
			axisName, posName, negName,
			holdButtons[0] ? holdButtons : nullptr,
			slewButtons[0] ? slewButtons : nullptr,
			minMaxButtons[0] ? minMaxButtons : nullptr,
			dualEnded
		);
		_Axes[i] = newAxis;
	}
}

Configurator::~Configurator()
{
	for (ButtonProcessor *button : _Buttons)
		delete button;
	for (HATProcessor *button : _HATs)
		delete button;
	for (AxisProcessor *button : _Axes)
		delete button;
}

char *Configurator::Name() { return (char *)"Better Joysticks"; }
char *Configurator::Description() { return (char *)"Assign joystick buttons and axes to the global controls.  If a vessel supports the Better Joysicks plugin, you can also assign its vessel-specific controls here."; }

bool Configurator::clbkOpen(HWND hLaunchpad)
{
	_InputSystem = new TChapman500::Input::Windows::WinInputSystem();
	_RunDialog(IDD_BETTER_JOYSTICKS, hLaunchpad, &Configurator::_ConfiguratorProc, (DLGPROC)&Configurator::DialogProc);
	delete _InputSystem;
	return true;
}

int Configurator::clbkWriteConfig()
{
	return 0;
}



BOOL Configurator::DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:		// This is how we will start assigning controls
	{
		NMHDR *info = (NMHDR *)lParam;
		switch (info->code)
		{
		// Interested only in the lists that aren't the vessel list
		case NM_DBLCLK:
		{

			return TRUE;
		}
		// Only interested in the vessel list
		case NM_CLICK:
		{

			return TRUE;
		}
		}
		return FALSE;
	}
	case WM_INITDIALOG:
	{
		Configurator *pThis = (Configurator *)lParam;
		SetWindowLongW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->_ThisHWND = hWnd;

		// List Item
		LVITEM item;
		ZeroMemory(&item, sizeof(LVITEM));
		item.mask = LVIF_TEXT;
		item.iSubItem = 0;

		// BEGIN BUTTON CONTROL LIST MODIFICATIONS
		pThis->_ButtonList = GetDlgItem(hWnd, IDC_BUTTON_LIST);
		pThis->_HATList = GetDlgItem(hWnd, IDC_HAT_LIST);
		pThis->_AxisList = GetDlgItem(hWnd, IDC_AXIS_LIST);

		// Button/HAT control list columns
		LPWSTR buttonColumns[] = {
			(LPWSTR)L"Control",
			(LPWSTR)L"Assignment"
		};
		// Axis control list columns
		LPWSTR axisColumns[] = {
			(LPWSTR)L"Control",
			(LPWSTR)L"Primary",
			(LPWSTR)L"Secondary",
			(LPWSTR)L"Null Zone 0",
			(LPWSTR)L"Null Zone 1",
			(LPWSTR)L"Sensitivity",
			(LPWSTR)L"Slew Time",
			(LPWSTR)L"Value",
			(LPWSTR)L"Raw Value"
		};
		// Apply column settings
		pThis->_SetColumns(pThis->_ButtonList, 2, buttonColumns);
		pThis->_SetColumns(pThis->_HATList, 2, buttonColumns);
		pThis->_SetColumns(pThis->_AxisList, 9, axisColumns);

		// Populate the Button Control List
		for (size_t i = 0; i < pThis->_Buttons.size(); i++)
		{
			item.iItem = i;
			item.pszText = new WCHAR[pThis->_Buttons[i]->Name.length()] + 1;
			for (size_t c = 0; c < pThis->_Buttons[i]->Name.length(); c++)
				item.pszText[c] = (WCHAR)pThis->_Buttons[i]->Name[c];
			item.pszText[pThis->_Buttons[i]->Name.length()] = L'\0';
			ListView_InsertItem(pThis->_ButtonList, &item);
			delete[] item.pszText;
		}
		// Populate the HAT Control List
		for (size_t i = 0; i < pThis->_HATs.size(); i++)
		{
			item.iItem = i;
			item.pszText = new WCHAR[pThis->_HATs[i]->Name.length()] + 1;
			for (size_t c = 0; c < pThis->_HATs[i]->Name.length(); c++)
				item.pszText[c] = (WCHAR)pThis->_HATs[i]->Name[c];
			item.pszText[pThis->_HATs[i]->Name.length()] = L'\0';
			ListView_InsertItem(pThis->_HATList, &item);
			delete[] item.pszText;
		}
		// Populate the Axis Control List
		for (size_t i = 0; i < pThis->_Axes.size(); i++)
		{
			item.iItem = i;
			item.pszText = new WCHAR[pThis->_Axes[i]->Name.length()] + 1;
			for (size_t c = 0; c < pThis->_Axes[i]->Name.length(); c++)
				item.pszText[c] = (WCHAR)pThis->_Axes[i]->Name[c];
			item.pszText[pThis->_Axes[i]->Name.length()] = L'\0';
			ListView_InsertItem(pThis->_AxisList, &item);
			delete[] item.pszText;
		}

		return FALSE;
	}
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void Configurator::_RunDialog(UINT dialogID, HWND parent, void(Configurator:: *loopProc)(), DLGPROC dialogProc)
{
	HWND result = CreateDialogParamW(_HInstance, MAKEINTRESOURCE(dialogID), parent, (DLGPROC)&Configurator::DialogProc, (LPARAM)this);
	if (!result) return;

	MSG msg;
	// Disable launchpad window
	EnableWindow(parent, FALSE);
	// Show the dialog box
	ShowWindow(result, SW_SHOW);
	UpdateWindow(result);
	// We're implementing our own modal loop.
	while (IsWindow(result))
	{
		// Handle System Messages
		if (PeekMessageW(&msg, nullptr, NULL, NULL, PM_REMOVE))
		{
			if (!IsDialogMessageW(result, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			continue;
		}
		_InputSystem->UpdateJoystickStates();
		(this->*loopProc)();
	}
	// Re-enable launchpad window
	EnableWindow(parent, TRUE);
}

void Configurator::_ConfiguratorProc()
{
	// Update Device States
	

	// Update Axis States


	// Update Button States


	// Update HAT States


}

void Configurator::_JoystickConfigProc()
{
	// Update Button States

	
	// Update HAT States


}

void Configurator::_AxisConfigProc()
{
	// Update Sensitivity Label


	// Update Null Zone 0 Slider


	// Update Null Zone 1 Slider


}

void Configurator::_AxisAssignProc()
{
	// Scan through all axes on each joystick and wait for a change of >25%


}

void Configurator::_ButtonAssignProc()
{
	// Wait for a rising edge


}

void Configurator::_HATAssignProc()
{
	// Wait for any state change


}

void Configurator::_SetColumns(HWND list, size_t columnCount, LPWSTR *labels)
{
	// No valid content
	if (!columnCount || !labels || !labels[0]) return;

	// Update list style
	ListView_SetExtendedListViewStyle(list, (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SINGLESEL) & ~LVS_EX_CHECKBOXES);

	// Column List
	LVCOLUMNW column;
	memset(&column, 0, sizeof(LVCOLUMNW));
	column.mask = LVCF_TEXT | LVCF_WIDTH;

	// Set first column
	column.pszText = labels[0];
	column.cx = 135;
	ListView_InsertColumn(list, 0, &column);

	column.cx = 100;
	// Set remaining columns
	for (size_t i = 1; i < columnCount; i++)
	{
		column.pszText = labels[i];
		ListView_InsertColumn(list, i, &column);
	}
}
