#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyPlayerState.h"
#include "Bouton.h"
#include "Blueprint/UserWidget.h"
#include "myprojectGameMode.generated.h"

UCLASS()
class MYPROJECT_API AmyprojectGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AmyprojectGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;

    
    UFUNCTION(BlueprintCallable)
    void TogglePauseMenu();

    
    UFUNCTION(NetMulticast, Reliable)
    void MulticastTogglePause(bool bPaused);

    UFUNCTION(BlueprintCallable)
    void ResumeFromPause();

    UFUNCTION(BlueprintCallable)
    void ReturnAllPlayersToLobby();


protected:
    UPROPERTY(Transient)
    bool bHasMapChanged;

    UPROPERTY(Transient)
    bool bIsPaused;

    UPROPERTY(EditAnywhere, Category = "Bouton")
    TSubclassOf<ABouton> BoutonClass;

    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CountdownTime;

    FTimerHandle CountdownTimerHandle;
    float ServerStartTime; 
    void UpdateCountdown();

    
    void SyncServerTime(APlayerController* NewPlayer);
    float GetServerTime() const;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

private:
    void ChangeMap();
    void AssignRolesOnLevel();
    void SpawnBoutonsOnLevel();
    void UpdateLevelTimer();

    
    FTimerHandle LevelDurationTimerHandle;
    int32 LevelDuration;       
    int32 LevelTimeRemaining;  
    void EndLevel();
    void PauseLevelTimer();
    void ResumeLevelTimer();

    UPROPERTY()
    UUserWidget* PauseWidgetInstance;
};
