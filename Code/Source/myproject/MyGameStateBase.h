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

	
	UPROPERTY(ReplicatedUsing = OnRep_Nbtache, BlueprintReadOnly)
	int32 Nbtache;

	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxTaches;

	
	UFUNCTION(Server, Reliable)
	void ServerModifyNbtache(AMyPlayerState* PlayerState);

	UFUNCTION()
	void OnRep_Nbtache();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(ReplicatedUsing = OnRep_CountdownTime, BlueprintReadOnly, Category = "Countdown")
	int32 CountdownTime;

	
	UFUNCTION()
	void OnRep_CountdownTime();


protected:
	virtual void BeginPlay() override;
};
