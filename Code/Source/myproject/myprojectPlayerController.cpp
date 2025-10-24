#include "myprojectPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "myprojectGameMode.h"
#include "Bouton.h"
#include "MyPlayerState.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "myprojectPlayerController.h"
#include "myprojectGameMode.h"
#include "Engine/World.h"

void AmyprojectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    
    if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
    {
        if (MobileControlsWidgetClass)
        {
            MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
            if (MobileControlsWidget)
            {
                MobileControlsWidget->AddToPlayerScreen(0);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Could not spawn mobile controls widget."));
            }
        }
    }
}

void AmyprojectPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    
    if (IsLocalPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            {
                Subsystem->AddMappingContext(CurrentContext, 0);
            }

            if (!SVirtualJoystick::ShouldDisplayTouchInterface())
            {
                for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
                {
                    Subsystem->AddMappingContext(CurrentContext, 0);
                }
            }
        }
    }

    
    InputComponent->BindKey(EKeys::P, IE_Pressed, this, &AmyprojectPlayerController::Interact);
    InputComponent->BindKey(EKeys::M, IE_Pressed, this, &AmyprojectPlayerController::HandlePauseMenu);
}

void AmyprojectPlayerController::Interact()
{
    ServerInteract();
}

void AmyprojectPlayerController::ServerInteract_Implementation()
{
    APawn* MyPawn = GetPawn();
    if (!MyPawn) return;

    
    TArray<AActor*> Boutons;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABouton::StaticClass(), Boutons);

    for (AActor* Actor : Boutons)
    {
        ABouton* Btn = Cast<ABouton>(Actor);
        if (!Btn) continue;

        if (FVector::Dist(MyPawn->GetActorLocation(), Btn->GetActorLocation()) <= Btn->InteractionDistance)
        {
            AMyPlayerState* PS = GetPlayerState<AMyPlayerState>();
            if (PS)
            {
                Btn->Interact(PS); 
                break;
            }
        }
    }
}

void AmyprojectPlayerController::HandlePauseMenu()
{
    AmyprojectGameMode* GM = Cast<AmyprojectGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->TogglePauseMenu();
    }
}

