#pragma once
#include "FreeCameraZTranslate.h"
#include "CanMouseMove.h"
#include "CanMoveAround.h"

namespace Input
{
	class FreeCameraControl :
		public CanMouseMove,
		public FreeCameraZTranslate,
		public CanMoveAround
	{
	public:
		FreeCameraControl();
		FreeCameraControl(bool enable);

	public:
		void ApplyKeySuppressionModifier(InputFilter& filter) const override;

	};
}
