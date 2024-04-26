#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_BlueTeam UMETA(DisplayName = "Blue Team"),
	ET_ReadTeam UMETA(DisplayName = "Red Team"),
	ET_NoTeam UMETA(DisplayName = "No Team"),
	
	ET_MAX UMETA(DisplayName = "DefaultMAX")
};