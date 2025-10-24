#include "MyGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Bouton.h"
#include "Kismet/GameplayStatics.h"

AMyGameStateBase::AMyGameStateBase()
{
	Nbtache = 0;
	MaxTaches = 0;
	CountdownTime = 0;
	LevelTimeRemaining = 0;
}

void AMyGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		MaxTaches = FMath::RandRange(3, 10);

		for (int32 i = 0; i < MaxTaches; i++)
		{
			FVector SpawnLocation;
			SpawnLocation.X = FMath::RandRange(-1000.f, 1000.f);
			SpawnLocation.Y = FMath::RandRange(-1000.f, 1000.f);
			SpawnLocation.Z = 0.f;

			FActorSpawnParameters Params;
			GetWorld()->SpawnActor<ABouton>(ABouton::StaticClass(), SpawnLocation, FRotator::ZeroRotator, Params);
		}

		Nbtache = MaxTaches;
	}
}

void AMyGameStateBase::ServerModifyNbtache_Implementation(AMyPlayerState* PlayerState)
{
	if (!PlayerState) return;

	EPlayerRole RoleValue = PlayerState->GetPlayerRole();

	switch (RoleValue)
	{
	case EPlayerRole::Gentil:
		Nbtache = FMath::Clamp(Nbtache - 1, 0, MaxTaches);
		break;

	case EPlayerRole::Mechant:
		Nbtache = FMath::Clamp(Nbtache + 1, 0, MaxTaches);
		break;

	case EPlayerRole::Mort:
		break;
	}

	OnRep_Nbtache();
}

void AMyGameStateBase::OnRep_Nbtache()
{
	UE_LOG(LogTemp, Warning, TEXT("Nbtache r�pliqu� = %d"), Nbtache);
}

void AMyGameStateBase::OnRep_CountdownTime()
{
	UE_LOG(LogTemp, Warning, TEXT("CountdownTime r�pliqu� = %d"), CountdownTime);
}

void AMyGameStateBase::OnRep_LevelTimeRemaining()
{
	UE_LOG(LogTemp, Warning, TEXT("LevelTimeRemaining r�pliqu� = %d"), LevelTimeRemaining);
	// Ici tu peux appeler une fonction Blueprint pour mettre � jour le widget
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameStateBase, CountdownTime);
	DOREPLIFETIME(AMyGameStateBase, Nbtache);
	DOREPLIFETIME(AMyGameStateBase, MaxTaches);
	DOREPLIFETIME(AMyGameStateBase, LevelTimeRemaining);
}
