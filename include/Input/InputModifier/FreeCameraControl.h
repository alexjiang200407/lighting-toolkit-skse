#pragma once
#include "CanMouseButton.h"
#include "CanMouseMove.h"
#include "CanMoveAround.h"

namespace Input
{
	class FreeCameraControl :
		public CanMouseButton,
		public CanMouseMove,
		public CanMoveAround
	{
	public:
		FreeCameraControl();
		FreeCameraControl(bool enable);
	};
}
