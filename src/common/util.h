#pragma once
#include <type_traits>

#define ASSERT_BASE(super, sub)             \
	static_assert(                          \
		std::is_base_of<sub, super>::value, \
		"Class " #super " must be a superclass of " #sub)

template <std::size_t N, std::string_view const (&Strs)[N]>
struct join_array
{
	static consteval std::size_t total_length() noexcept
	{
		std::size_t length = 0;
		for (std::size_t i = 0; i < N; ++i) length += Strs[i].size();
		return length;
	}

	static constexpr std::size_t len = total_length();

	static constexpr auto impl() noexcept
	{
		std::array<char, len + 1> arr{};

		std::size_t idx = 0;
		for (std::size_t i = 0; i < N; ++i)
		{
			for (char c : Strs[i])
			{
				arr[idx++] = c;
			}
		}
		arr[len] = '\0';

		return arr;
	}

	static constexpr auto             arr = impl();
	static constexpr std::string_view value{ arr.data(), arr.size() - 1 };
};

#define JOIN_V(arr) join_array<sizeof(arr) / sizeof(const std::string_view), arr>::value

template <typename T>
bool parseStr(const char* str, T& buf)
{
	std::istringstream iss(str);
	iss >> buf;

	bool success = iss.eof() && !iss.fail();

	if (!success)
		logger::warn("Failed parsing `{}` as {}", str, typeid(T).name());

	return success;
}

std::string GetEditorID(const RE::TESForm* form);
