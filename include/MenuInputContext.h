#pragma once
#include "Input/InputContext.h"
namespace Input
{
	class MenuInputContext :
		public InputContext
	{
	private:
		enum ModifierSlots : int
		{
			kFreeCameraMoveMode = 0,
			kTextInput          = 1,
		};

	public:
		MenuInputContext();

	public:
		void StartLookingAround();
		void StopLookingAround();
		void StartTextInput();
		void StopTextInput();
		void MenuOpen();
		void MenuClose();
	};
}
