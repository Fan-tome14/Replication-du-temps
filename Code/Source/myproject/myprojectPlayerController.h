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

    
    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    
    UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

   
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> PauseMenuWidget;

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    
    void Interact();

    
    void HandlePauseMenu();

    
    UFUNCTION(Server, Reliable)
    void ServerInteract();
};
