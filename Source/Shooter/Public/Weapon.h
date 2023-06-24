// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
    EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
    EWT_Pistol UMETA(DisplayName = "Pistol"),

    EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

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

    /** Максимальный боезапас, который может вместить наше оружие */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    int32 MagazineCapacity;

    /** Тип оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    EWeaponType WeaponType;

    /** Тип боеприпасов для этого оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    EAmmoType AmmoType;

    /** FName для секции Reload Montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    FName ReloadMontageSection;

    /** Истинно при перемещении обоймы во время перезарядки */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    bool bMovingClip;

    /** Имя кости магазина */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    FName ClipBoneName;

public:

    /** Добавляет импульс оружию */
    void ThrowWeapon();
    
    FORCEINLINE int32 GetAmmo() const { return Ammo; }
    FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

    /** Вызывается из класса Character при стрельбе из оружия */
    void DecrementAmmo();

    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
    FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
    FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
    FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

    void ReloadAmmo(int32 Amount);

    FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
};
