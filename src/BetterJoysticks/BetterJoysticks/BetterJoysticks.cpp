#define STRICT
#define ORBITER_MODULE
#include "BetterJoysticks.h"

using namespace TChapman500::Input;
using namespace oapi;

BetterJoysticks::BetterJoysticks(HINSTANCE hDLL) : Module(hDLL)
{
	_InputSystem = nullptr;


}

BetterJoysticks::~BetterJoysticks()
{
	if (_InputSystem)
		delete _InputSystem;
}

void BetterJoysticks::clbkSimulationStart(RenderMode mode)
{
	// This will create a hidden window to receive HID reports from joysticks and gamepads.
	_InputSystem = new Windows::WinInputSystem();
}

void BetterJoysticks::clbkSimulationEnd()
{
	delete _InputSystem;
	_InputSystem = nullptr;
}

void BetterJoysticks::clbkPreStep(double simt, double simdt, double mjd)
{
}

void BetterJoysticks::clbkPostStep(double simt, double simdt, double mjd)
{
}

void BetterJoysticks::clbkTimeJump(double simt, double simdt, double mjd)
{
}

void BetterJoysticks::clbkTimeAccChanged(double new_warp, double old_warp)
{
}

void BetterJoysticks::clbkPause(bool pause)
{
}

void BetterJoysticks::clbkFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus)
{
}

void BetterJoysticks::clbkNewVessel(OBJHANDLE hVessel)
{
}

void BetterJoysticks::clbkDeleteVessel(OBJHANDLE hVessel)
{
}

void BetterJoysticks::clbkVesselJump(OBJHANDLE hVessel)
{
}
