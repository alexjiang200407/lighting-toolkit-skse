#pragma once
#include "Input/InputContext.h"
namespace Input
{
	class MenuInputContext : public InputContext
	{
	private:
		enum ModifierSlots : int
		{
			kCanRun,
			kFreeCameraLook,
			kFreeCameraMoveMode,
			kTextInput,
		};

	public:
		MenuInputContext();

	public:
		void StartLookingAround();
		void StopLookingAround();
		void StartTextInput();
		void StopTextInput();
		void StopAllInput();
		void MenuOpen();
		void MenuHidden();
		void MenuClose();
		void EnablePositioning();
		void DisablePositioning();
		bool AllowsPositioning() const;

	private:
		bool allowPositioning = false;
	};
}
