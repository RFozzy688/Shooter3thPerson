// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
    GENERATED_BODY()
    
public:

    AAmmo();

    virtual void Tick(float DeltaTime) override;

protected:

    virtual void BeginPlay() override;

    // Переопределить SetItemProperties, чтобы мы могли установить свойства AmmoMesh.
    virtual void SetItemProperties(EItemState State) override;

    UFUNCTION()
    void AmmoSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

private:
    /** Mesh для подбора боеприпасов */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* AmmoMesh;

    /** Тип патрона для боеприпаса */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
    EAmmoType AmmoType;

    /** Текстура для иконки патронов */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
    UTexture2D* AmmoIconTexture;

    /** сфера коллизий для сбора боеприпасов */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
    class USphereComponent* AmmoCollisionSphere;

public:

    FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
    FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
};
