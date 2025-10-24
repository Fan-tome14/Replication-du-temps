#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMyPlayerState::AMyPlayerState()
{
    PlayerRole = EPlayerRole::Gentil; 
}

void AMyPlayerState::SetPlayerRole(EPlayerRole NewRole)
{
    if (HasAuthority()) 
    {
        PlayerRole = NewRole;
        OnRep_PlayerRole();
    }
}

EPlayerRole AMyPlayerState::GetPlayerRole() const
{
    return PlayerRole;
}

void AMyPlayerState::OnRep_PlayerRole()
{
    const TCHAR* RoleText = (PlayerRole == EPlayerRole::Gentil) ? TEXT("Gentil") :
        (PlayerRole == EPlayerRole::Mechant) ? TEXT("Méchant") :
        TEXT("Mort");

    UE_LOG(LogTemp, Warning, TEXT("Le rôle du joueur a changé : %s"), RoleText);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyPlayerState, PlayerRole);
}
