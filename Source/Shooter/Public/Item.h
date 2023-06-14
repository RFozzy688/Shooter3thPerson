// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SHOOTER_API AItem : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    AItem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    /** Skeletal Mesh для предмета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    /** Трассировка линии сталкивается с полем для отображения виджетов HUD */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CollisionBox;

    /** Всплывающий виджет, когда игрок смотрит на предмет */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* PickupWidget;

public:

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};
