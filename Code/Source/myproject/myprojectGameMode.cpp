#include "myprojectGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerState.h"
#include "Bouton.h"
#include "MyGameStateBase.h"


AmyprojectGameMode::AmyprojectGameMode()
{
    bIsPaused = false;
    bHasMapChanged = false;
    CountdownTime = 30; 
    LevelDuration = 120; 
    LevelTimeRemaining = LevelDuration;
    PauseWidgetInstance = nullptr;
}

void AmyprojectGameMode::TogglePauseMenu()
{
    bIsPaused = !bIsPaused;

    
    UWorld* World = GetWorld();
    FString CurrentMapName = World ? World->GetMapName() : "";
    if (World) CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);

    if (bIsPaused)
    {
        
        if (CurrentMapName.Equals("Lobby"))
        {
            GetWorldTimerManager().PauseTimer(CountdownTimerHandle);
            UE_LOG(LogTemp, Warning, TEXT("⏸️ Timer du Lobby mis en pause"));
        }
        else if (CurrentMapName.Equals("Level"))
        {
            PauseLevelTimer();
        }

        
        for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
                if (APawn* P = PC->GetPawn()) P->DisableInput(PC);
        }
    }
    else
    {
        
        if (CurrentMapName.Equals("Lobby"))
        {
            GetWorldTimerManager().UnPauseTimer(CountdownTimerHandle);
            UE_LOG(LogTemp, Warning, TEXT("▶️ Timer du Lobby relancé"));
        }
        else if (CurrentMapName.Equals("Level"))
        {
            ResumeLevelTimer();
        }

        
        for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
                if (APawn* P = PC->GetPawn()) P->EnableInput(PC);
        }
    }

    MulticastTogglePause(bIsPaused);
}

void AmyprojectGameMode::MulticastTogglePause_Implementation(bool bPaused)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC) continue;

        if (bPaused)
        {
            PC->SetShowMouseCursor(true);
            PC->SetInputMode(FInputModeUIOnly());

            if (!PauseWidgetInstance)
            {
                if (PauseMenuWidgetClass) 
                {
                    PauseWidgetInstance = CreateWidget<UUserWidget>(PC, PauseMenuWidgetClass);
                    if (PauseWidgetInstance)
                        PauseWidgetInstance->AddToViewport();
                }
            }
        }
        else
        {
            PC->SetShowMouseCursor(false);
            PC->SetInputMode(FInputModeGameOnly());

            if (PauseWidgetInstance)
            {
                PauseWidgetInstance->RemoveFromParent();
                PauseWidgetInstance = nullptr;
            }
        }
    }
}


void AmyprojectGameMode::PauseLevelTimer()
{
    GetWorldTimerManager().PauseTimer(LevelDurationTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("⏸️ Timer de niveau mis en pause"));
}

void AmyprojectGameMode::ResumeLevelTimer()
{
    GetWorldTimerManager().UnPauseTimer(LevelDurationTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("▶️ Timer de niveau relancé"));
}

void AmyprojectGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UWorld* World = GetWorld();
    if (!World) return;

    FString CurrentMapName = World->GetMapName();
    CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);

    if (CurrentMapName.Equals("Lobby"))
    {
        AGameStateBase* GS = GetGameState<AGameStateBase>();
        if (!GS) return;

        int32 PlayerCount = GS->PlayerArray.Num();
        UE_LOG(LogTemp, Warning, TEXT("Nombre de joueurs connectés : %d"), PlayerCount);

        if (!bHasMapChanged && PlayerCount >= 2)
        {
            bHasMapChanged = true;

            UE_LOG(LogTemp, Warning, TEXT("2 joueurs connectés ! Synchronisation du temps et démarrage du compte à rebours..."));

            ServerStartTime = GetWorld()->GetTimeSeconds();
            CountdownTime = 30;

            for (APlayerState* PS : GS->PlayerArray)
            {
                if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
                    SyncServerTime(PC);
            }

            GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AmyprojectGameMode::UpdateCountdown, 1.0f, true);
        }
    }
    else if (CurrentMapName.Equals("Level"))
    {
        if (!HasAuthority()) return;

        AssignRolesOnLevel();

        
        LevelTimeRemaining = LevelDuration;
        GetWorldTimerManager().SetTimer(LevelDurationTimerHandle, this, &AmyprojectGameMode::UpdateLevelTimer, 1.0f, true);
    }
}

void AmyprojectGameMode::SyncServerTime(APlayerController* NewPlayer)
{
    if (!NewPlayer) return;
    const float CurrentServerTime = GetWorld()->GetTimeSeconds();
    NewPlayer->ClientMessage(FString::Printf(TEXT("ServerTimeSync: %f"), CurrentServerTime));
}

float AmyprojectGameMode::GetServerTime() const
{
    UWorld* World = GetWorld();
    return World ? World->GetTimeSeconds() : 0.f;
}

void AmyprojectGameMode::UpdateCountdown()
{
    float Elapsed = GetWorld()->GetTimeSeconds() - ServerStartTime;
    CountdownTime = 30 - FMath::FloorToInt(Elapsed);

    AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
    if (MyGS)
        MyGS->CountdownTime = CountdownTime;

    UE_LOG(LogTemp, Warning, TEXT("Temps restant (synchro serveur) : %d secondes"), CountdownTime);

    if (CountdownTime <= 0)
    {
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        ChangeMap();
    }
}

void AmyprojectGameMode::ChangeMap()
{
    
    GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("⏹️ Timer du lobby arrêté avant changement de map"));

    UWorld* World = GetWorld();
    if (World && World->GetNetMode() == NM_ListenServer)
    {
        FString MapPath = "/Game/Level";
        UE_LOG(LogTemp, Warning, TEXT("ServerTravel vers %s"), *MapPath);
        World->ServerTravel(MapPath + "?listen");
    }

    SpawnBoutonsOnLevel();

    
    LevelTimeRemaining = LevelDuration;
    GetWorldTimerManager().SetTimer(LevelDurationTimerHandle, this, &AmyprojectGameMode::UpdateLevelTimer, 1.0f, true);
}


void AmyprojectGameMode::AssignRolesOnLevel()
{
    AGameStateBase* GS = GetGameState<AGameStateBase>();
    if (!GS) return;

    TArray<AMyPlayerState*> Players;
    for (APlayerState* PS : GS->PlayerArray)
    {
        AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS);
        if (MyPS) Players.Add(MyPS);
    }
    if (Players.Num() == 0) return;

    for (int32 i = 0; i < Players.Num(); i++)
    {
        int32 SwapIndex = FMath::RandRange(0, Players.Num() - 1);
        Players.Swap(i, SwapIndex);
    }

    Players[0]->SetPlayerRole(EPlayerRole::Gentil);

    for (int32 i = 1; i < Players.Num(); i++)
    {
        EPlayerRole NewRole = (FMath::RandBool()) ? EPlayerRole::Mechant : EPlayerRole::Mort;
        Players[i]->SetPlayerRole(NewRole);
    }

    for (AMyPlayerState* PS : Players)
    {
        const TCHAR* RoleText = (PS->GetPlayerRole() == EPlayerRole::Gentil) ? TEXT("Gentil") :
            (PS->GetPlayerRole() == EPlayerRole::Mechant) ? TEXT("Méchant") : TEXT("Mort");
        UE_LOG(LogTemp, Warning, TEXT("Joueur %s est %s"), *PS->GetPlayerName(), RoleText);
    }
}

void AmyprojectGameMode::SpawnBoutonsOnLevel()
{
    if (!HasAuthority() || !BoutonClass) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FString CurrentMapName = World->GetMapName();
    CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);
    if (!CurrentMapName.Equals("Level"))
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBoutons annulé : pas sur Level"));
        return;
    }

    for (int32 i = 0; i < 5; i++)
    {
        FVector SpawnLocation(FMath::RandRange(-1000.f, 1000.f), 0.f, FMath::RandRange(-1000.f, 1000.f));
        ABouton* NewBouton = World->SpawnActor<ABouton>(BoutonClass, SpawnLocation, FRotator::ZeroRotator);
        if (NewBouton && NewBouton->MeshComp)
        {
            UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/bouton"));
            if (Mat)
                NewBouton->MeshComp->SetMaterial(0, Mat);
        }
    }
}

void AmyprojectGameMode::EndLevel()
{
    UE_LOG(LogTemp, Warning, TEXT("Fin du niveau ! Retour au Lobby"));

    UWorld* World = GetWorld();
    if (World && World->GetNetMode() == NM_ListenServer)
    {
        FString LobbyMapPath = "/Game/Lobby";
        World->ServerTravel(LobbyMapPath + "?listen");
    }
}

void AmyprojectGameMode::UpdateLevelTimer()
{
    if (!bIsPaused)
    {
        LevelTimeRemaining--;
        UE_LOG(LogTemp, Warning, TEXT("Temps restant dans le niveau : %d secondes"), LevelTimeRemaining);

        if (LevelTimeRemaining <= 0)
        {
            GetWorldTimerManager().ClearTimer(LevelDurationTimerHandle);
            EndLevel();
        }
    }
}

void AmyprojectGameMode::ResumeFromPause()
{
    if (!bIsPaused) return;

    bIsPaused = false;

    
    UWorld* World = GetWorld();
    FString CurrentMapName = World ? World->GetMapName() : "";
    if (World) CurrentMapName.RemoveFromStart(World->StreamingLevelsPrefix);

    
    if (CurrentMapName.Equals("Lobby"))
    {
        GetWorldTimerManager().UnPauseTimer(CountdownTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("▶️ Timer du Lobby relancé via bouton Reprendre"));
    }
    else if (CurrentMapName.Equals("Level"))
    {
        ResumeLevelTimer();
    }

    
    for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
            if (APawn* P = PC->GetPawn()) P->EnableInput(PC);
    }

    
    if (PauseWidgetInstance)
    {
        PauseWidgetInstance->RemoveFromParent();
        PauseWidgetInstance = nullptr;
    }

    
    MulticastTogglePause(bIsPaused);
}

void AmyprojectGameMode::ReturnAllPlayersToLobby()
{
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() != NM_ListenServer) return;

    UE_LOG(LogTemp, Warning, TEXT("🔹 Tous les joueurs retournent au Lobby"));

    
    for (auto It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            PC->SetShowMouseCursor(false);
            PC->SetInputMode(FInputModeGameOnly());
        }
    }

    FString LobbyMapPath = "/Game/Lobby";
    World->ServerTravel(LobbyMapPath + "?listen");
}
