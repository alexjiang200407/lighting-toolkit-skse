#pragma once

namespace ImGui
{
	class ImGuiInputAdapter
	{
	public:
		typedef std::bitset<256> KeyboardSupressionMask;
		typedef std::bitset<256> MouseSupressionMask;
		typedef std::bitset<256> GamePadSupressionMask;

	private:
		struct SendInputEvent
		{
			static void                                    thunk(RE::BSTEventSource<RE::InputEvent*>* dispatcher, RE::InputEvent** ppEvents);
			static inline REL::Relocation<decltype(thunk)> func;
		};

	public:
		void                      Init();
		void                      EnableSupression(KeyboardSupressionMask kbd, MouseSupressionMask mouse, GamePadSupressionMask gamepad, bool suppressChar, bool suppressMouseMove);
		void                      DisableSupression();
		static ImGuiInputAdapter* GetSingleton();
		bool                      IsSuppressingButtons();

	private:
		ImGuiInputAdapter() = default;

		static ImGuiKey ToImGuiKey(RE::BSWin32KeyboardDevice::Key key);
		static ImGuiKey ToImGuiKey(RE::BSWin32GamepadDevice::Key key);
		static int      ToImGuiKey(RE::BSWin32MouseDevice::Key key);

		typedef std::pair<RE::InputEvent*, RE::InputEvent*> InputList;
		template <size_t S>
		static void UpdateInputList(std::bitset<S> keySet, InputList& list, InputList& removed, RE::ButtonEvent* const event)
		{
			if (!singleton.IsSuppressingButtons() || !keySet.test(event->GetIDCode()))
			{
				AddToInputList(list, event);
			}
			else
			{
				AddToInputList(removed, event);
			}
		}

		static void AddToInputList(InputList& list, RE::InputEvent* const event)
		{
			list.second       = list.first ? list.second->next = event : list.first = event;
			list.second->next = nullptr;
		}

		void HandleButtonEvent(RE::ButtonEvent* const event, InputList& list, InputList& removed);
		void HandleKeyboardButtonEvent(uint32_t key, bool isPressed) const;
		void HandleMouseButtonEvent(uint32_t key, float value, bool isPressed) const;
		void HandleGamepadButtonEvent(uint32_t key, bool isPressed) const;
		void HandleCharEvent(RE::CharEvent* const charEvt) const;

	private:
		KeyboardSupressionMask   kbdSuppress          = 0;
		MouseSupressionMask      mouseSuppress        = 0;
		GamePadSupressionMask    gamepadSuppress      = 0;
		bool                     blockCharEvents      = false;
		bool                     blockMouseMoveEvents = false;
		static ImGuiInputAdapter singleton;
	};

}
