#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "myprojectPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class AmyprojectGameMode;

UCLASS()
class MYPROJECT_API AmyprojectPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    // RPC client pour afficher le menu pause
    UFUNCTION(Client, Reliable)
    void ClientShowPauseMenu(bool bPaused);

    UFUNCTION(Client, Reliable)
    void HandlePauseMenu();


protected:

    // --- Input ---
    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    // --- Touch Controls ---
    UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

    // --- Menu Pause ---
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> PauseMenuWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> OtherPlayerPauseWidgetClass;

    UPROPERTY()
    UUserWidget* OtherPlayerPauseWidgetInstance;

    // --- Functions ---
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    void Interact();

    UFUNCTION(Server, Reliable)
    void ServerTogglePauseMenu();

    UFUNCTION(Server, Reliable)
    void ServerInteract();
};
