#include "HATProcessor.h"

using namespace TChapman500::Input;

HATProcessor::HATProcessor(std::string name)
{
	Name = name;
	Control = nullptr;
}

bool HATProcessor::RisingEdge() { if (Control) return Control->ChangeFrom == hat_position::Centered; }
bool HATProcessor::FallingEdge() { if (Control) return Control->ChangeTo == hat_position::Centered; }
hat_position HATProcessor::Value() { if (Control) return Control->Position; }
