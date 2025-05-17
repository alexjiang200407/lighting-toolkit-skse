#pragma once
#include "CanMouseMove.h"
#include "CanMoveAround.h"
#include "FreeCameraZTranslate.h"

namespace Input
{
	class FreeCameraControl : public CanMouseMove, public FreeCameraZTranslate, public CanMoveAround
	{
	public:
		FreeCameraControl();
		FreeCameraControl(bool enable);

	public:
		void ApplyKeySuppressionModifier(InputFilter& filter) const override;
		bool ApplyBlockMouseMoveModifier(bool prevBlockMouseMove) const override;
	};
}
