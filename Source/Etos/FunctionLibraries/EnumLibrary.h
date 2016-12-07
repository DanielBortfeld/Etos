// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "EnumLibrary.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EResource : uint8
{
	None,
	Money,
	Wood,
	Tool,
	Stone,
	Fish,
	Most,
	Coal,
	IronOre,
	Iron,

	EResource_MAX
};

/**
*
*/
UENUM(BlueprintType)
enum class EResidentLevel : uint8
{
	None,
	Peasant,
	Citizen,

	EResidentLevel_MAX
};

/**
*
*/
UENUM(BlueprintType)
enum class EResidentNeed : uint8
{
	None,
	TownCenter,
	Chapel,
	Tavern,

	EResidentNeed_MAX
};

namespace Enum
{
	static bool IsValid(EResource resource)
	{
		return resource != EResource::None && resource != EResource::EResource_MAX;
	}

	static bool IsValid(EResidentLevel level)
	{
		return level != EResidentLevel::None && level != EResidentLevel::EResidentLevel_MAX;
	}

	static bool IsValid(EResidentNeed need)
	{
		return need != EResidentNeed::None && need != EResidentNeed::EResidentNeed_MAX;
	}
}