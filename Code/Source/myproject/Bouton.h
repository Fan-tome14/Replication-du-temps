#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyPlayerState.h"
#include "Bouton.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API ABouton : public AActor
{
    GENERATED_BODY()

public:
    ABouton();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    
    UPROPERTY(VisibleAnywhere)
    USphereComponent* SphereComp;

    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

   
    UPROPERTY(EditAnywhere)
    float InteractionDistance;

    
    void Interact(AMyPlayerState* PlayerState);
};
