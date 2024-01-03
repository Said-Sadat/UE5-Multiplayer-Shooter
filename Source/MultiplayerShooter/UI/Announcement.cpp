// Fill out your copyright notice in the Description page of Project Settings.


#include "Announcement.h"
#include "Components/TextBlock.h"

void UAnnouncement::SetWarmupTimeText(float RemainingTime)
{
	int32 Minutes = FMath::FloorToInt(RemainingTime / 60.f);
	int32 Seconds = RemainingTime - (Minutes * 60);
	
	FString TimerText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	WarmupTime->SetText(FText::FromString(TimerText));
}
