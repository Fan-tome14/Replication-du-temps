#pragma once 

#include "CoreMinimal.h" 
#include "GameFramework/GameStateBase.h" 
#include "MyPlayerState.h" 
#include "MyGameStateBase.generated.h" 

UCLASS()
class MYPROJECT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameStateBase();

	/** Nombre de tâches restantes, répliqué */
	UPROPERTY(ReplicatedUsing = OnRep_Nbtache, BlueprintReadOnly)
	int32 Nbtache;

	/** Nombre maximum de tâches, répliqué */
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxTaches;

	/** Timer du Lobby, répliqué */
	UPROPERTY(ReplicatedUsing = OnRep_CountdownTime, BlueprintReadOnly, Category = "Countdown")
	int32 CountdownTime;

	/** Timer du niveau, répliqué */
	UPROPERTY(ReplicatedUsing = OnRep_LevelTimeRemaining, BlueprintReadOnly, Category = "Level")
	int32 LevelTimeRemaining;

	/** Fonction serveur pour modifier Nbtache */
	UFUNCTION(Server, Reliable)
	void ServerModifyNbtache(AMyPlayerState* PlayerState);

	/** RepNotify pour Nbtache */
	UFUNCTION()
	void OnRep_Nbtache();

	/** RepNotify pour CountdownTime */
	UFUNCTION()
	void OnRep_CountdownTime();

	/** RepNotify pour LevelTimeRemaining */
	UFUNCTION()
	void OnRep_LevelTimeRemaining();

	/** Fonction UE pour déclarer les propriétés à répliquer */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
};
