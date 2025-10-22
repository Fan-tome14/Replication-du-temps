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

    // 🔹 Active ou désactive le menu pause
    UFUNCTION(BlueprintCallable)
    void TogglePauseMenu();

    // 🔹 RPC multicast pour tous les clients
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

    // 🔹 Compte à rebours de 30 secondes avant le niveau
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CountdownTime;

    FTimerHandle CountdownTimerHandle;
    float ServerStartTime; // Temps exact du début du compte à rebours
    void UpdateCountdown();

    // 🔹 Synchronisation du temps côté client
    void SyncServerTime(APlayerController* NewPlayer);
    float GetServerTime() const;

    // 🔹 Widget pause
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

private:
    void ChangeMap();
    void AssignRolesOnLevel();
    void SpawnBoutonsOnLevel();
    void UpdateLevelTimer();

    // 🔹 Timer du niveau
    FTimerHandle LevelDurationTimerHandle;
    int32 LevelDuration;       // Durée totale du niveau en secondes
    int32 LevelTimeRemaining;  // Temps restant pour affichage
    void EndLevel();
    void PauseLevelTimer();
    void ResumeLevelTimer();

    UPROPERTY()
    UUserWidget* PauseWidgetInstance;
};
