#pragma once
#include <OrbiterAPI.h>
#include <vector>
#include <InputSystem/WinInputSystem.h>
#include <stack>

class ConfigRoot : public LaunchpadItem
{
public:
	ConfigRoot();
	~ConfigRoot();
	virtual char *Name() override;
	virtual char *Description() override;
};

class ButtonProcessor;
class AxisProcessor;
class HATProcessor;
class Configurator : public LaunchpadItem
{
public:
	Configurator(HINSTANCE hInst);
	~Configurator();

	virtual char *Name() override;
	virtual char *Description() override;
	virtual bool clbkOpen(HWND hLaunchpad) override;
	virtual int clbkWriteConfig() override;

private:
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void _RunDialog(UINT dialogID, HWND parent, void (Configurator::*loopProc)(), DLGPROC dialogProc);

	void _ConfiguratorProc();
	void _JoystickConfigProc();
	void _AxisConfigProc();
	void _AxisAssignProc();
	void _ButtonAssignProc();
	void _HATAssignProc();

	void _SetColumns(HWND list, size_t columnCount, LPWSTR *labels);

	std::vector<ButtonProcessor*> _Buttons;
	std::vector<AxisProcessor*> _Axes;
	std::vector<HATProcessor*> _HATs;

	enum class last_window
	{
		ButtonList,
		AxisList,
		HATList
	};

	TChapman500::Input::Windows::WinInputSystem *_InputSystem;

	unsigned _PrevDeviceCount;

	HWND _ThisHWND;
	// Control Lists
	HWND _ButtonList;
	HWND _HATList;
	HWND _AxisList;
	// Other Lists
	HWND _VesselList;
	HWND _DeviceList;

	// Default Procs
	static WNDPROC ListViewProc;
	static WNDPROC ListProc;
	// We'll need this for the other procs
	static Configurator *PThis;

	// This DLL
	HINSTANCE _HInstance;
};

