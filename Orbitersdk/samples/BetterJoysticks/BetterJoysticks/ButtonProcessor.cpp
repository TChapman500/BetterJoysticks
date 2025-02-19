#include "ButtonProcessor.h"

ButtonProcessor::ButtonProcessor(std::string name) { Name = name; }

bool ButtonProcessor::RisingEdge()
{
	if (Control && Control->RisingEdge) return true;
	return false;
}

bool ButtonProcessor::FallingEdge()
{
	if (Control && Control->FallingEdge) return true;
	return false;
}

bool ButtonProcessor::Pressed()
{
	if (Control && Control->IsPressed) return true;
	return false;
}

float ButtonProcessor::Value()
{
	if (Control && Control->IsPressed) return 1.0f;
	return 0.0f;
}
