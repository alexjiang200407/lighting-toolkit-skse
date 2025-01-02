#pragma once

typedef std::uint64_t PresetID;

enum PresetTID : std::uint16_t
{
	kUnassigned    = 0,
	kLightTemplate = 1,
	kColor         = 2
};

class Preset
{
public:
	Preset()          = default;
	virtual ~Preset() = default;
	Preset(PresetTID type, PresetID objID, std::string name);

public:
	bool operator<(const Preset& rhs) const;

	PresetID    GetID() const;
	const char* GetSelectionName() const;

public:
	static constexpr PresetID configTypeMask = 0xffff000000000000;
	static constexpr PresetTID TID            = PresetTID::kUnassigned;

private:
	std::string name;
	PresetID    id = 0;
	int         refCount = 0;
};
