#pragma once
#include <InputSystem/InputAxis.h>
#include <string>

class ButtonProcessor;
class AxisProcessor
{
public:
	AxisProcessor(std::string name, std::string posDir, std::string negDir, ButtonProcessor **hold, ButtonProcessor **slew, ButtonProcessor **minMax, bool dualEnded);



	std::string Name;
	std::string PosDirectionName;
	std::string NegDirectionName;

	struct button_pair
	{
		ButtonProcessor *Positive;
		ButtonProcessor *Negative;
		bool ModeAllowed;

		bool Value(float &value);
	};
	button_pair Hold;
	button_pair Slew;
	button_pair MinMax;

	struct assignment
	{
		TChapman500::Input::InputAxis *Control;
		bool Invert;

		float _CenteredValue;
		float _EndValue;

		float PrevCenteredValue();
		bool CenteredValue(float &value);
		float PrevEndValue();
		bool EndValue(float &value);
	};
	assignment Primary;
	assignment Secondary;

	void SetNullZone(float value, bool first);
	float GetNullZone(bool first);

	void SetSensitivity(float value);
	float GetSensitivity();

	void SetSlewTime(float value);
	float GetSlewTime();

	float Value();
	float RawValue();

	void UpdateAxis();		// Forces a re-evaluation of axis state for when the null zone or sensitivity is changed
	void Update(float deltaT);	// Called every time step

private:
	enum class mode
	{
		Hold,
		Slew,
		Axis
	};
	mode _Mode;

	float _NullZone0;
	float _NullZone1;
	float _Sensitivity;
	float _Exponent;
	float _SlewTime;

	float _Value;
	float _RawValue;
	float _NullZoneValue;

	bool _DualEnded;
	bool _DisallowSecondary;

	void _AdjustAxis(float primary, float secondary);
};

