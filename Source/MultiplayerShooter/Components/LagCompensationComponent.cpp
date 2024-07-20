// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "DrawDebugHelpers.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		
		ShowFramePackage(ThisFrame, FColor::Red);
	}
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<AMultiplayerShooterCharacter>(GetOwner()) : Character;
	if(Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for(auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}

FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame,
	const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0, 1);

	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;

	for(auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}
	
	return InterpFramePackage;
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for(auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

void ULagCompensationComponent::ServerSideRewind(AMultiplayerShooterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	bool bReturn =
		HitCharacter == nullptr ||
		HitCharacter->GetLagCompoensationComponent() == nullptr ||
		HitCharacter->GetLagCompoensationComponent()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompoensationComponent()->FrameHistory.GetTail() == nullptr;

	// Frame package that we check to verify a hit.
	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;

	// Frame history of the Hit Character.
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompoensationComponent()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
	
	if(OldestHistoryTime > HitTime)
	{
		// Too far back - Too laggy for SSR
		return;
	}
	if(OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if(NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;

	while (Older->GetValue().Time > HitTime) // Is Older still younger than HitTime.
	{
		// March back until: OlderTime < HitTime < YoungerTime
		if(Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();

		if(Older->GetValue().Time > HitTime)
			Younger = Older;
	}
	if(Older->GetValue().Time == HitTime) // In unlikely case time is equal, we found frame to check.
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}
	if(bShouldInterpolate)
	{
		// Interpolate between Younger and Older.
	}
	
	if(bReturn)
		return;

	
}
