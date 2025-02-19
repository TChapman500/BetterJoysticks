#pragma once
#include <InputSystem/InputButton.h>
#include <string>

class ButtonProcessor
{
public:
	std::string Name;
	TChapman500::Input::InputButton *Control;

	ButtonProcessor(std::string name);
	bool RisingEdge();
	bool FallingEdge();
	bool Pressed();
	float Value();

};

