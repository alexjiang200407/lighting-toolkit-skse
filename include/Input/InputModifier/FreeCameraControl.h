#pragma once
#include "CanMouseButton.h"
#include "CanMouseMove.h"
#include "CanMoveAround.h"

namespace Input
{
	class FreeCameraControl :
		public CanMouseMove,
		public CanMouseButton,
		public CanMoveAround
	{
	public:
		FreeCameraControl();
		FreeCameraControl(bool enable);

	public:
		void ApplyKeySuppressionModifier(InputFilter& filter) const override;

	};
}
