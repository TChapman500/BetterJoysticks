#pragma once

#include <Orbitersdk.h>
#include <InputSystem/WinInputSystem.h>
#include "Configurator.h"

namespace oapi {
class BetterJoysticks : public Module
{
public:
	BetterJoysticks(HINSTANCE hDLL);
	~BetterJoysticks();

	// Simulation Start/End Functions
	virtual void clbkSimulationStart(RenderMode mode) override;
	virtual void clbkSimulationEnd() override;

	// Time Propagation Functions
	virtual void clbkPreStep(double simt, double simdt, double mjd) override;
	virtual void clbkPostStep(double simt, double simdt, double mjd) override;
	virtual void clbkTimeJump(double simt, double simdt, double mjd) override;
	virtual void clbkTimeAccChanged(double new_warp, double old_warp) override;
	virtual void clbkPause(bool pause) override;

	// Object/Vessel Handling Functions
	virtual void clbkFocusChanged(OBJHANDLE new_focus, OBJHANDLE old_focus) override;
	virtual void clbkNewVessel(OBJHANDLE hVessel) override;
	virtual void clbkDeleteVessel(OBJHANDLE hVessel) override;
	virtual void clbkVesselJump(OBJHANDLE hVessel) override;

private:
	TChapman500::Input::Windows::WinInputSystem *_InputSystem;
};
}
