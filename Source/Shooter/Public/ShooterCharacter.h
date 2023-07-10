// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
    ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
    ECS_Reloading UMETA(DisplayName = "Reloading"),
    ECS_Equipping UMETA(DisplayName = "Equipping"),

    ECS_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
    GENERATED_BODY()

    // ��������� ����� ��� ������������� � �������� ��� �������������� ��� �������������
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* SceneComponent;

    // ���������� ���������, ������������� � ���� ����� ���������� �����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AShooterCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

    /** ��������� ��� bAiming �������� true ��� false �������� ������ */
    void AimingButtonPressed();
    void AimingButtonReleased();

    void CameraInterpZoom(float DeltaTime);

    void CalculateCrosshairSpread(float DeltaTime);

    void StartCrosshairBulletFire();

    UFUNCTION()
    void FinishCrosshairBulletFire();

    void FireButtonPressed();
    void FireButtonReleased();

    void StartFireTimer();

    UFUNCTION()
    void AutoFireReset();

    /** ����������� ��� ����������� � ����������. ����������� ������� �� ����������� */
    bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutBeamLocation);

    /** ���������� ��� ��������, ���� OverlappedItemCount > 0 */
    void TraceForItems();

    // ������� ����������� ������ � ��������� ���
    class AWeapon* SpawnDefaultWeapon();

    /** ����� ������ � ����������� ��� � ����� */
    void EquipWeapon(AWeapon* WeaponToEquip);

    /** ����� ������ �� ����� */
    void DropWeapon();

    void SelectButtonPressed();
    void SelectButtonReleased();

    /** ���������� ������� ������������ ������ � ��������� ��������� ��������� ��� ������ ���������� */
    void SwapWeapon(AWeapon* WeaponToSwap);

    /** ���������������� ����� ����������� ���������� ����������� */
    void InitializeAmmoMap();

    /** ���������, ��� � ����� ������ ���� �������. */
    bool WeaponHasAmmo();

    /** FireWeapon functions */
    void PlayFireSound();
    void SendBullet();
    void PlayGunfireMontage();

    /** �������� � ������� R */
    void ReloadButtonPressed();

    /** ������ ����������� ������ */
    void ReloadWeapon();

    /** ���������, ���� �� � ��� ���������� ���� ������������ ������. */
    bool CarryingAmmo();

    /** ���������� �� Animation Blueprint � ������������ Grab Clip */
    UFUNCTION(BlueprintCallable)
    void GrabClip();

    /** ���������� �� Animation Blueprint � ������������ Release Clip */
    UFUNCTION(BlueprintCallable)
    void ReleaseClip();

    void CrouchButtonPressed();

    virtual void Jump() override;

    /** ���������������� ������� �� �������� ������ ��� ����������/������� */
    void InterpCapsuleHalfHeight(float DeltaTime);

    void Aim();
    void StopAiming();

    void PickupAmmo(class AAmmo* Ammo);

    void InitializeInterpLocations();

    void FKeyPressed();
    void OneKeyPressed();
    void TwoKeyPressed();
    void ThreeKeyPressed();
    void FourKeyPressed();
    void FiveKeyPressed();

    void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

    // �����-����� ������� ������ ������ �� ������� � ������� �� ���
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    /** ����������������� �������� ������ �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class USoundCue* FireSound;

    /** �������, ��������� � BarrelSocket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UParticleSystem* MuzzleFlash;

    /** ������ ��� �������� �� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UAnimMontage* HipFireMontage;

    // �������, ������������ ��� ��������� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* ImpactParticle;

    // ������ ���� �� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* BeamParticle;

    /** ������� ��� ������������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    /** �������� ���� ������ ������ �� ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float CameraDefaultFOV;

    /** �������� ���� ������ ������ ��� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float CameraZoomedFOV;

    /** ������� ���� ������ � ������ ����� */
    float CameraCurrentFOV;

    /** �������� ������������ ��� ��������������� ��� ������������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float ZoomInterpSpeed;

    /** �����������, �������� ��������, ����� �� �� ��������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipTurnRate;

    /** �����������, �������� �������, ����� �� ��������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipLookUpRate;

    /** �����������, �������� ��������, ����� �� ��������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingTurnRate;

    /** �����������, �������� �������, ����� �� ��������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingLookUpRate;

    /** ���������� ������� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairSpreadMultiplier;

    /** ��������� �������� ��� �������� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairVelocityFactor;

    /** ��������� ��� �������� ����������� � �������*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairInAirFactor;

    /** ��������� ������������ ��� �������� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairAimFactor;

    /** ��������� �������� ��� �������� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairShootingFactor;

    float ShootTimeDuration;
    bool bFiringBullet;
    FTimerHandle CrosshairShootTimer;

    /** ������ ����� ������ ���� */
    bool bFireButtonPressed;

    /** true, ����� �� ����� ��������. False ��� �������� ������� */
    bool bShouldFire;

    /** ���������������� ��������������� ������ */
    float AutomaticFireRate;

    /** ������������� ������ ����� ���������� */
    FTimerHandle AutoFireTimer;

    /** �������, ���� �� ������ ����������� � ������ ����� ��� ��������� */
    bool bShouldTraceForItems;

    /** ���������� ����������� AItems */
    int8 OverlappedItemCount;

    /** AItem, � ������� ������ ���������� � ��������� ����� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    class AItem* TraceHitItemLastFrame;

    /** ������� ������������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AWeapon* EquippedWeapon;

    /** ���������� ��� � Blueprints ��� ������ ������ �� ���������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AWeapon> DefaultWeaponClass;

    /** �������, ������� � ��������� ����� �������� � ���� ����������� � TraceForItems (����� ���� �������) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AItem* TraceHitItem;

    /** ���������� ������ �� ������ �� ������ ���������� interp */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpDistance;

    /** ���������� ����� �� ������ �� ������ ���������� interp */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpElevation;

    /** Map ��� ������������ ����������� ��������� ����� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    TMap<EAmmoType, int32> AmmoMap;

    /** ��������� ���������� �������� ������� 9 �� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
    int32 Starting9mmAmmo;

    /** ��������� ���������� ����������� AR */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
    int32 StartingARAmmo;

    /** ������ ���������, ����� �������� ��� �������������� ������ � Unoccupied. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    ECombatState CombatState;

    /** ������ �������� ����������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UAnimMontage* ReloadMontage;

    /** ������ �������� �������������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UAnimMontage* EquipMontage;

    UFUNCTION(BlueprintCallable)
    void FinishReloading();

    UFUNCTION(BlueprintCallable)
    void FinishEquipping();

    /** ������������� ������, ����� �� ������� ����� ������ �� ����� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    FTransform ClipTransform;

    /** ��������� ����� ��� ������������ � ���� ��������� �� ����� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    USceneComponent* HandSceneComponent;

    /** ������ ��� ���������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bCrouching;

    /** ������� �������� ������������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseMovementSpeed;

    /** �������� ������������ � ������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchMovementSpeed;

    /** ������� �������� ������ ������� */
    float CurrentCapsuleHalfHeight;

    /** �������� ������ �������, ����� �� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float StandingCapsuleHalfHeight;

    /** �������� ������ ������� ��� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchingCapsuleHalfHeight;

    /** ������ � �����, �� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseGroundFriction;

    /** ������ � ����� ��� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchingGroundFriction;

    /** ������������ ��� ����������� ������� ������� ������ ������������. */
    bool bAimingButtonPressed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* WeaponInterpComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp4;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp5;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* InterpComp6;

    /** ������ �������� �������������� �������������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TArray<FInterpLocation> InterpLocations;

    FTimerHandle PickupSoundTimer;
    FTimerHandle EquipSoundTimer;

    bool bShouldPlayPickupSound;
    bool bShouldPlayEquipSound;

    void ResetPickupSoundTimer();
    void ResetEquipSoundTimer();

    /** ����� ��������� ����� ������������� ����� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float PickupSoundResetTime;

    /** ����� ��������� ����� ������������� ����� �������������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float EquipSoundResetTime;

    /** ������ AItems ��� ������ ��������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    TArray<AItem*> Inventory;

    const int32 INVENTORY_CAPACITY{ 6 };

    /** ������� ��� �������� ���������� � ����� � InventoryBar ��� ���������� */
    UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
    FEquipItemDelegate EquipItemDelegate;

public:

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void TurnAround(float Amount);
    void LookUp(float Amount);

    /** ���������� ��� ������� ������ Fire */
    void FireWeapon();

    FORCEINLINE bool GetAiming() const { return bAiming; }

    UFUNCTION(BlueprintCallable)
    float GetCrosshairSpreadMultiplier() const;

    FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

    /** ���������/�������� �� OverlappedItemCount � ��������� bShouldTraceForItems */
    void IncrementOverlappedItemCount(int8 Amount);

    // ��������� ����. ����� ����� �������
    //FVector GetCameraInterpLocation();

    // �������� �������� � �������������
    void GetPickupItem(AItem* Item);

    FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
    FORCEINLINE bool GetCrouching() const { return bCrouching; }
    FInterpLocation GetInterpLocation(int32 Index);

    // ���������� ������ � ������� InterpLocations � ���������� ��������� ItemCount.
    int32 GetInterpLocationIndex();

    void IncrementInterpLocItemCount(int32 Index, int32 Amount);

    FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
    FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

    void StartPickupSoundTimer();
    void StartEquipSoundTimer();
};
