#pragma once
#include <string>
#include <InputSystem/InputHAT.h>

class HATProcessor
{
public:

	std::string Name;
	TChapman500::Input::InputHAT *Control;

	HATProcessor(std::string name);
	bool RisingEdge();
	bool FallingEdge();
	TChapman500::Input::hat_position Value();
};

