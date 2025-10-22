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


protected:

    /** Input Mapping Contexts */
    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    /** Widget pour contrôles mobiles */
    UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

    /** Widget Pause/Menu */
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> PauseMenuWidget;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    /** Action d'interaction (touche P) */
    void Interact();

    /** Action Toggle Pause/Menu (touche M) */
    void HandlePauseMenu();

    /** RPC côté serveur pour Interact */
    UFUNCTION(Server, Reliable)
    void ServerInteract();
};
