#pragma once

namespace Input
{
	typedef std::bitset<256> KeyboardSupressionMask;
	typedef std::bitset<256> MouseSupressionMask;
	typedef std::bitset<256> GamePadSupressionMask;

	struct InputFilter
	{
		KeyboardSupressionMask kbdSuppress          = 0;
		MouseSupressionMask    mouseSuppress        = 0;
		GamePadSupressionMask  gamepadSuppress      = 0;
		bool                   blockCharEvents      = false;
		bool                   blockMouseMoveEvents = false;
	};
}
