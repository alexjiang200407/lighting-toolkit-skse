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
			kPlayerMove       = 0,
			kPlayerLookAround = 1,
			kCharInput        = 2,
			kMouseButton      = 3
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
