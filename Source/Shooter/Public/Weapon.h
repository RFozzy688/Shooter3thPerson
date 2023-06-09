// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAmmoType AmmoType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WeaponAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MagazingCapacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USoundCue* PickupSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundCue* EquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMesh* ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* InventoryIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* AmmoIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInstance* MaterialInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaterialIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ClipBoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ReloadMontageSection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UAnimInstance> AnimBP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsMiddle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsBottom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* CrosshairsTop;

    /** ���������������� ��������������� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AutoFireRate;

    /** �������, ��������� � BarrelSocket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UParticleSystem* MuzzleFlash;

    /** ����������������� �������� ������ �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundCue* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BoneToHide;
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

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;

private:

    FTimerHandle ThrowWeaponTimer;
    float ThrowWeaponTime;
    bool bFalling;

    /** ���������� �������� ��� ����� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    int32 Ammo;

    /** ������������ ��������, ������� ����� �������� ���� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    int32 MagazineCapacity;

    /** ��� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    EWeaponType WeaponType;

    /** ��� ����������� ��� ����� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    EAmmoType AmmoType;

    /** FName ��� ������ Reload Montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    FName ReloadMontageSection;

    /** ������� ��� ����������� ������ �� ����� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    bool bMovingClip;

    /** ��� ����� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    FName ClipBoneName;

    /** ������� ������ ��� ������� ������ */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UDataTable* WeaponDataTable;

    int32 PreviousMaterialIndex;

    /** �������� ��� ������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UTexture2D* CrosshairsMiddle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UTexture2D* CrosshairsLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UTexture2D* CrosshairsRight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UTexture2D* CrosshairsBottom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UTexture2D* CrosshairsTop;

    /** ��������, � ������� ���������� �������������� ����� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    float AutoFireRate;

    /** ������� ������, ����������� � BarrelSocket */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFlash;

    /** ����, ��������������� ��� �������� �� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    USoundCue* FireSound;

    /** �������� �����, ������� ����� ������ �� ��������� �����. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    FName BoneToHide;

public:

    /** ��������� ������� ������ */
    void ThrowWeapon();
    
    FORCEINLINE int32 GetAmmo() const { return Ammo; }
    FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

    /** ���������� �� ������ Character ��� �������� �� ������ */
    void DecrementAmmo();

    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
    FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
    FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
    FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }
    FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
    FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }
    FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
    FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
    FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

    void ReloadAmmo(int32 Amount);

    FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

    bool ClipIsFull();
};
