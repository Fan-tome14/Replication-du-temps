#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyPlayerState.h"
#include "Bouton.h"
#include "myprojectGameMode.generated.h"

UCLASS()
class MYPROJECT_API AmyprojectGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AmyprojectGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
    UPROPERTY(Transient)
    bool bHasMapChanged;

    
    UPROPERTY(EditAnywhere, Category = "Bouton")
    TSubclassOf<ABouton> BoutonClass;

private:
    void ChangeMap();
    void AssignRolesOnLevel();
    void SpawnBoutonsOnLevel();
    void UpdateCountdown();

    FTimerHandle CountdownTimerHandle;
    int32 CountdownTime;

    // 🔹 Ajout : gestion du temps synchronisé
    void SyncServerTime(APlayerController* NewPlayer);
    float GetServerTime() const;

    float ServerStartTime; // temps exact du début du compte à rebours


    FTimerHandle LevelDurationTimerHandle;
    int32 LevelDuration; // Durée totale du niveau en secondes
    int32 LevelTimeRemaining; // Temps restant pour affichage
    void EndLevel();

    // 🔹 Nouveau : fonction appelée chaque seconde pour log
    void UpdateLevelTimer();



};
