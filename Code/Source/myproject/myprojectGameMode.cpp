#include "myprojectGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AmyprojectGameMode::AmyprojectGameMode()
{
    bHasMapChanged = false;
    CountdownTime = 30; // Compte à rebours de 30 secondes
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

            // 🔹 Enregistre le temps exact de début serveur
            ServerStartTime = GetWorld()->GetTimeSeconds();
            CountdownTime = 30;

            // 🔹 Synchronise chaque joueur avec le serveur
            for (APlayerState* PS : GS->PlayerArray)
            {
                if (APlayerController* PC = Cast<APlayerController>(PS->GetOwner()))
                {
                    SyncServerTime(PC);
                }
            }

            // 🔹 Démarre le timer sur le serveur
            GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AmyprojectGameMode::UpdateCountdown, 1.0f, true);
        }

    }
    else if (CurrentMapName.Equals("Level"))
    {
        if (!HasAuthority()) return;

        AssignRolesOnLevel();
    }
}

void AmyprojectGameMode::SyncServerTime(APlayerController* NewPlayer)
{
    if (!NewPlayer) return;

    // Récupère le temps actuel du serveur (secondes depuis le lancement du monde)
    const float CurrentServerTime = GetWorld()->GetTimeSeconds();

    // Envoie ce temps au client via RPC
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

    // 🔹 Met à jour la valeur du GameState (répliquée automatiquement aux clients)
    AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
    if (MyGS)
    {
        MyGS->CountdownTime = CountdownTime;
    }

    UE_LOG(LogTemp, Warning, TEXT("Temps restant (synchro serveur) : %d secondes"), CountdownTime);

    if (CountdownTime <= 0)
    {
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        ChangeMap();
    }
}



void AmyprojectGameMode::ChangeMap()
{
    UWorld* World = GetWorld();
    if (World && World->GetNetMode() == NM_ListenServer)
    {
        FString MapPath = "/Game/Level";
        UE_LOG(LogTemp, Warning, TEXT("ServerTravel vers %s"), *MapPath);

        World->ServerTravel(MapPath + "?listen");
    }

    SpawnBoutonsOnLevel();
}


void AmyprojectGameMode::AssignRolesOnLevel()
{
    AGameStateBase* GS = GetGameState<AGameStateBase>();
    if (!GS) return;

    TArray<AMyPlayerState*> Players;

    for (APlayerState* PS : GS->PlayerArray)
    {
        AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS);
        if (MyPS)
            Players.Add(MyPS);
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
        FVector SpawnLocation;
        SpawnLocation.X = FMath::RandRange(-1000.f, 1000.f);
        SpawnLocation.Z = FMath::RandRange(-1000.f, 1000.f);
        SpawnLocation.Y = 0.f; 

        FRotator SpawnRotation = FRotator::ZeroRotator;

        ABouton* NewBouton = World->SpawnActor<ABouton>(BoutonClass, SpawnLocation, SpawnRotation);
        if (NewBouton)
        {
            
            UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/bouton"));
            if (Mat && NewBouton->MeshComp)
            {
                NewBouton->MeshComp->SetMaterial(0, Mat);
            }
        }
    }
}
