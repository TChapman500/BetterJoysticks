#include <Orbitersdk.h>
#include "BetterJoysticks.h"

using namespace oapi;

BetterJoysticks *_Instance;

bool _OwnsConfiguratorRoot;
LAUNCHPADITEM_HANDLE _Root;
LAUNCHPADITEM_HANDLE _Configurator;
ConfigRoot *_RootItem;
Configurator *_ConfiguratorItem;

DLLCLBK void InitModule(HINSTANCE hModule)
{
	_Instance = new BetterJoysticks(hModule);
	oapiRegisterModule(_Instance);

	_Root = oapiFindLaunchpadItem("Input systems", nullptr);
	if (_Root)
	{
		_OwnsConfiguratorRoot = false;
		_RootItem = nullptr;
	}
	else
	{
		_OwnsConfiguratorRoot = true;
		_RootItem = new ConfigRoot();
		_Root = oapiRegisterLaunchpadItem(_RootItem, nullptr);
	}
	_ConfiguratorItem = new Configurator(hModule);
	_Configurator = oapiRegisterLaunchpadItem(_ConfiguratorItem, _Root);
}

DLLCLBK void ExitModule(HINSTANCE hModule)
{
	oapiUnregisterLaunchpadItem(_ConfiguratorItem);
	delete _ConfiguratorItem;

	if (_OwnsConfiguratorRoot)
	{
		oapiUnregisterLaunchpadItem(_RootItem);
		delete _RootItem;
	}

	delete _Instance;
}
