#include "AxisProcessor.h"
#include "ButtonProcessor.h"
#include <math.h>

bool AxisProcessor::button_pair::Value(float &value)
{
	float positive = Positive ? 0.0f : Positive->Value();
	float negative = Negative ? 0.0f : Negative->Value();
	value = positive - negative;
	return positive > 0.0f || negative > 0.0f;
}

float AxisProcessor::assignment::PrevCenteredValue() { return _CenteredValue; }
bool AxisProcessor::assignment::CenteredValue(float &value)
{
	if (!Control)
	{
		value = 0.0f;
		return false;
	}

	value = Control->CenterRelative;
	if (Invert) value = -value;

	bool result = value != _CenteredValue;
	_CenteredValue = value;
	return result;
}

float AxisProcessor::assignment::PrevEndValue() { return _EndValue; }
bool AxisProcessor::assignment::EndValue(float &value)
{
	if (!Control)
	{
		value = 0.0f;
		return false;
	}

	value = Control->EndRelative;
	if (Invert) value = 1.0f - value;

	bool result = value != _EndValue;
	_EndValue = value;
	return result;
}

AxisProcessor::AxisProcessor(std::string name, std::string posDir, std::string negDir, ButtonProcessor **hold, ButtonProcessor **slew, ButtonProcessor **minMax, bool dualEnded)
{
	Name = name;
	_DualEnded = dualEnded;
	_DisallowSecondary = !dualEnded;
	// Fetch allowed modes.
	if (hold)
	{
		Hold.ModeAllowed = true;
		Hold.Positive = hold[0];
		Hold.Negative = hold[1];

		if (Hold.Positive) PosDirectionName = Hold.Positive->Name;
		else PosDirectionName = posDir;

		if (Hold.Negative) NegDirectionName = Hold.Negative->Name;
		else NegDirectionName = negDir;
	}
	else
	{
		Hold.ModeAllowed = false;
		Hold.Positive = nullptr;
		Hold.Negative = nullptr;

		PosDirectionName = posDir;
		NegDirectionName = negDir;
	}
	if (slew)
	{
		Slew.ModeAllowed = true;
		Slew.Positive = slew[0];
		Slew.Negative = slew[1];
	}
	else
	{
		Slew.ModeAllowed = false;
		Slew.Positive = nullptr;
		Slew.Negative = nullptr;
	}
	if (minMax)
	{
		MinMax.ModeAllowed = true;
		MinMax.Positive = minMax[0];
		MinMax.Negative = minMax[1];
	}
	else
	{
		MinMax.ModeAllowed = false;
		MinMax.Positive = nullptr;
		MinMax.Negative = nullptr;
	}

	_SlewTime = 0.0f;
	_NullZone0 = 0.0f;
	_NullZone1 = 0.0f;
	SetSensitivity(0.5f);

	_Value = 0.0f;
	_RawValue = 0.0f;
	_NullZoneValue = 0.0f;
}

void AxisProcessor::SetNullZone(float value, bool first)
{
	if (first)
	{
		if (value < 0.0f) value = 0.0f;
		else if (value + _NullZone1 > 1.0f) value = 1.0f - _NullZone1;
		_NullZone0 = value;
	}
	else
	{
		if (value < 0.0f) value = 0.0f;
		else if (value + _NullZone0 > 1.0f) value = 1.0f - _NullZone0;
		_NullZone1 = value;
	}
}

float AxisProcessor::GetNullZone(bool first)
{
	if (first) return _NullZone0;
	return _NullZone1;
}

void AxisProcessor::SetSensitivity(float value)
{
	if (value < 0.0f) value = 0.0f;
	else if (value > 1.0f) value = 1.0f;
	_Sensitivity = value;

	// Process sensitivity curve.
	float exponent = 0.5f - _Sensitivity;
	// Curve flat at begining of range.
	if (_Sensitivity < 0.5f) exponent = 1.0f + (exponent * 2.0f);
	// Curve flat at end of range.
	else exponent = 1.0f + exponent;
	// Cache exponent calculations
	_Exponent = exponent;
}

float AxisProcessor::GetSensitivity() { return _Sensitivity; }
void AxisProcessor::SetSlewTime(float value)
{
}
float AxisProcessor::GetSlewTime()
{
	return 0.0f;
}
float AxisProcessor::Value() { return _Value; }

float AxisProcessor::RawValue()
{
	switch (_Mode)
	{
	case mode::Hold:
	case mode::Slew:
		return _Value;
	case mode::Axis:
	default:
		return _RawValue;
	}
}

void AxisProcessor::UpdateAxis()
{
	float primary;
	if (!Secondary.Control && _DualEnded) primary = Primary.PrevCenteredValue();
	else primary = Primary.PrevEndValue();

	float secondary;
	if (_DisallowSecondary) secondary = 0.0f;
	else secondary = Secondary.PrevEndValue();

	_AdjustAxis(primary, secondary);
}

void AxisProcessor::Update(float deltaT)
{
	// Axis has top priority
	float primary;
	bool usePrimary;
	if (!Secondary.Control && _DualEnded) usePrimary = Primary.CenteredValue(primary);
	else usePrimary = Primary.EndValue(primary);

	float secondary;
	bool useSecondary = false;
	if (_DisallowSecondary) secondary = 0.0f;
	else useSecondary = Secondary.EndValue(secondary);

	if (usePrimary || useSecondary)
	{
		_AdjustAxis(primary, secondary);
		return;
	}

	// Hold Mode has Second Priority
	float holdValue;
	if (Hold.ModeAllowed && Hold.Value(holdValue))
	{
		_Mode = mode::Hold;
		if (_SlewTime == 0.0f)
		{
			_Value = holdValue;
			_RawValue = _Value;
		}
		else
		{
			float delta = holdValue / _SlewTime * deltaT;
			_Value += delta;
			if (_Value > 1.0f) _Value = 1.0f;
			else if (_Value < -1.0f) _Value = -1.0f;
			_RawValue = _Value;
		}
		return;
	}

	// Slew Mode has Third Priority
	float slewValue;
	if (Slew.ModeAllowed && Slew.Value(slewValue))
	{
		_Mode = mode::Slew;
		if (_SlewTime == 0.0f)
		{
			_Value = slewValue;
			if (!_DualEnded && _Value < 0.0f) _Value = 0.0f;
			_RawValue = _Value;
		}
		else
		{
			float delta = slewValue / _SlewTime * deltaT;
			_Value += delta;
			if (_Value > 1.0f) _Value = 1.0f;
			else
			{
				if (_DualEnded)
				{
					if (_Value < -1.0f)
						_Value = -1.0f;
				}
				else
				{
					if (_Value < 0.0f)
						_Value = 0.0f;
				}
			}
		}
		return;
	}

	// Min/Max has Last Priority
	float minMaxValue;
	if (MinMax.ModeAllowed && MinMax.Value(minMaxValue))
	{
		_Mode = mode::Slew;
		_Value = minMaxValue;
		if (!_DualEnded && _Value < 0.0f) _Value = 0.0f;
		_RawValue = _Value;
		return;
	}

	// No inputs detected.
	if (_Mode == mode::Hold)
	{
		if (_SlewTime == 0.0f)
		{
			if (_Value != 0.0f)
			{
				_Value = 0.0f;
				_RawValue = _Value;
			}
		}
		else
		{
			float delta = 1.0f / _SlewTime * deltaT;
			if (_Value > 0.0f)
			{
				_Value -= delta;
				if (_Value < 0.0f) _Value = 0.0f;
				_RawValue = _Value;
			}
			else if (_Value < 0.0f)
			{
				_Value += delta;
				if (_Value > 0.0f) _Value = 0.0f;
				_RawValue = _Value;
			}
		}
	}
}

void AxisProcessor::_AdjustAxis(float primary, float secondary)
{
	_RawValue = primary - secondary;
	// We now have the raw value of this axis
	float absValue = _RawValue < 0.0f ? -_RawValue : _RawValue;
	absValue -= _NullZone0;
	if (absValue < 0.0f) absValue = 0.0f;
	absValue /= 1.0f - (_NullZone0 + _NullZone1);
	if (isnan(absValue) || isinf(absValue)) absValue = 0.0f;
	if (absValue > 1.0f) absValue = 1.0f;
	float value = _RawValue < 0.0f ? -absValue : absValue;

	// Axis moved beyond null zone range.
	if (value != _NullZoneValue)
	{
		_Mode = mode::Axis;
		_NullZoneValue = value;
		absValue = powf(absValue, _Exponent);
		_Value = _RawValue < 0.0f ? -absValue : absValue;
	}
}
