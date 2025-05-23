#pragma once

namespace MCM
{
	class SettingsImpl;

	class Settings
	{
		friend class SettingsImpl;

	public:
		void             Init();
		static Settings* GetSingleton();

		template <typename T>
		static bool Get_s(const char* a_section, const char* a_key, T& a_buf)
		{
			return GetSingleton()->Get(a_section, a_key, a_buf);
		}

		template <typename T>
		static T GetOrDefault_s(const char* a_section, const char* a_key, T a_default)
		{
			return GetSingleton()->GetOrDefault(a_section, a_key, a_default);
		}

		template <typename T>
		T GetOrDefault(const char* a_section, const char* a_key, T a_default)
		{
			T a_buf;
			if (Get(a_section, a_key, a_buf))
			{
				return a_buf;
			}
			return a_default;
		}

		bool Get(const char* a_section, const char* a_key, int& a_buf) const;
		bool Get(const char* a_section, const char* a_key, float& a_buf) const;
		bool Get(const char* a_section, const char* a_key, std::string& a_buf) const;
		bool Get(const char* a_section, const char* a_key, unsigned int& a_buf) const;
		bool Get(const char* a_section, const char* a_key, bool& a_buf) const;

	private:
		Settings() = default;

	private:
		std::shared_ptr<SettingsImpl> impl;
		static Settings               singleton;
	};
}
