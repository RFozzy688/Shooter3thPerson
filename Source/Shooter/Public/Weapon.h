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

    /** Скорострельность автоматического оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AutoFireRate;

    /** Вспышка, созданная в BarrelSocket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UParticleSystem* MuzzleFlash;

    /** Рандомизированный звуковой сигнал выстрела */
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

    void FinishMovingSlide();
    void UpdateSlideDisplacement();

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

    /** Таблица данных для свойств оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UDataTable* WeaponDataTable;

    int32 PreviousMaterialIndex;

    /** Текстуры для прицела оружия */
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

    /** Скорость, с которой происходит автоматический огонь */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    float AutoFireRate;

    /** Система частиц, порожденная в BarrelSocket */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFlash;

    /** Звук, воспроизводимый при выстреле из оружия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    USoundCue* FireSound;

    /** Название кости, которую нужно скрыть на оружейной сетке. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    FName BoneToHide;

    /** Величина, на которую затвор отодвигается назад во время стрельбы из пистолета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    float SlideDisplacement;

    /** Кривая смещения затвора */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    UCurveFloat* SlideDisplacementCurve;

    /** Дескриптор таймера для обновления SlideDisplacement */
    FTimerHandle SlideTimer;

    /** Время смещения затвора при стрельбе из пистолета */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    float SlideDisplacementTime;

    /** Правда при перемещении затвора пистолета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    bool bMovingSlide;

    /** Максимальное расстояние для слайда на пистолете */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    float MaxSlideDisplacement;

    /** Максимальное вращение для отдачи пистолета */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    float MaxRecoilRotation;

    /** Величина вращения пистолета во время стрельбы из пистолета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
    float RecoilRotation;

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
    FORCEINLINE void SetReloadMontageSection(FName Name) { ReloadMontageSection = Name; }
    FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
    FORCEINLINE void SetClipBoneName(FName Name) { ClipBoneName = Name; }
    FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
    FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
    FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }

    void StartSlideTimer();

    void ReloadAmmo(int32 Amount);

    FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

    bool ClipIsFull();
};
