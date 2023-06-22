// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
    GENERATED_BODY()
public:
    AWeapon();

    virtual void Tick(float DeltaTime) override;

protected:

    void StopFalling();

private:

    FTimerHandle ThrowWeaponTimer;
    float ThrowWeaponTime;
    bool bFalling;

    /** Количество патронов для этого оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    int32 Ammo;

public:

    /** Добавляет импульс оружию */
    void ThrowWeapon();
    
    FORCEINLINE int32 GetAmmo() const { return Ammo; }

    /** Вызывается из класса Character при стрельбе из оружия */
    void DecrementAmmo();
};
