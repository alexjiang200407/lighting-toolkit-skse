#pragma once

#include <nlohmann/json.hpp>

namespace preset
{
	typedef uuids::uuid PresetSID;

	enum PresetTID : std::uint16_t
	{
		kUnassigned    = 0,
		kLightTemplate = 1,
		kColor         = 2,
		kInvalid       = 3
	};

	class Preset;

	class PresetID
	{
	public:
		PresetID() = default;
		PresetID(PresetTID tid, PresetSID sid);

		bool operator<(const PresetID& id) const;
		bool operator<(const PresetTID& tid) const;

		template <typename T, typename std::enable_if<std::is_base_of<Preset, T>::value>::type* = nullptr>
		static PresetID GenID()
		{
			return PresetID(T::TID, uuids::uuid_system_generator{}());
		}

		template <typename T, typename std::enable_if<std::is_base_of<Preset, T>::value>::type* = nullptr>
		static PresetID SIDToID(PresetSID sid)
		{
			return PresetID(T::TID, sid);
		}

		template <typename T, typename std::enable_if<std::is_base_of<Preset, T>::value>::type* = nullptr>
		static PresetID SIDToID(std::string sid)
		{
			return SIDToID(T::TID, sid);
		}

		static PresetID SIDToID(PresetTID tid, std::string sid)
		{
			if (const auto id = uuids::uuid::from_string(sid))
				return PresetID(tid, id.value());

			return PresetID();
		}

		explicit operator bool() const
		{
			return IsNull();
		}

		PresetTID GetTID() const;

		bool IsNull() const;

		std::string GetSIDAsString() const;

	private:
		PresetTID tid = kUnassigned;
		PresetSID sid;
	};

	class Preset
	{
	public:
		Preset()          = default;
		virtual ~Preset() = default;
		Preset(PresetTID type, std::string name);
		Preset(PresetID id, std::string name);

	public:
		bool operator<(const Preset& rhs) const;

		std::string        GetSIDAsString() const;
		PresetTID          GetTID() const;
		PresetID           GetID() const;
		const char*        GetSelectionName() const;
		std::string&       GetName();
		const std::string& GetName() const;
		virtual json       Serialize() const;

	public:
		static constexpr PresetTID TID = PresetTID::kUnassigned;

	private:
		std::string name;
		PresetID    id;
		int         refCount = 0;
	};
	typedef std::unique_ptr<Preset> PresetPtr;
}
