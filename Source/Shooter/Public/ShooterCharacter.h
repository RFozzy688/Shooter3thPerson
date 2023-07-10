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

    // Компонент сцены для использования в качестве его местоположения для интерференции
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* SceneComponent;

    // Количество элементов, интерактивных в этом месте композиции сцены
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

    /** Установит для bAiming значение true или false нажатием кнопки */
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

    /** Трассировка для пересечения с предметами. Трассировка выходит из перекрестия */
    bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutBeamLocation);

    /** трасировка для предметы, если OverlappedItemCount > 0 */
    void TraceForItems();

    // Создает стандартное оружие и экипирует его
    class AWeapon* SpawnDefaultWeapon();

    /** Берет оружие и прикрепляет его к сетке */
    void EquipWeapon(AWeapon* WeaponToEquip);

    /** Сброс оружия на землю */
    void DropWeapon();

    void SelectButtonPressed();
    void SelectButtonReleased();

    /** Сбрасывает текущее экипированое оружие и экипирует предметом найденным при помощи трасировки */
    void SwapWeapon(AWeapon* WeaponToSwap);

    /** Инициализировать карту боеприпасов значениями боеприпасов */
    void InitializeAmmoMap();

    /** Убедитесь, что в нашем оружии есть патроны. */
    bool WeaponHasAmmo();

    /** FireWeapon functions */
    void PlayFireSound();
    void SendBullet();
    void PlayGunfireMontage();

    /** Привязан к клавише R */
    void ReloadButtonPressed();

    /** Ручная перезарядка оружия */
    void ReloadWeapon();

    /** Проверяет, есть ли у нас боеприпасы типа снаряженного оружия. */
    bool CarryingAmmo();

    /** Вызывается из Animation Blueprint с уведомлением Grab Clip */
    UFUNCTION(BlueprintCallable)
    void GrabClip();

    /** Вызывается из Animation Blueprint с уведомлением Release Clip */
    UFUNCTION(BlueprintCallable)
    void ReleaseClip();

    void CrouchButtonPressed();

    virtual void Jump() override;

    /** Иртерполирование капсулы на половину высоты при приседании/стоянии */
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

    // селфи-палка которая держит камеру за игроком и следует за ним
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    /** Рандомизированный звуковой сигнал выстрела */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class USoundCue* FireSound;

    /** Вспышка, созданная в BarrelSocket */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UParticleSystem* MuzzleFlash;

    /** Монтаж для стрельбы из оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    class UAnimMontage* HipFireMontage;

    // Частицы, образующиеся при попадании пули
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* ImpactParticle;

    // дымный след от пули
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* BeamParticle;

    /** Истинно при прицеливании */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    /** Значение поля обзора камеры по умолчанию */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float CameraDefaultFOV;

    /** Значение поля обзора камеры при увеличении */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float CameraZoomedFOV;

    /** Текущее поле обзора в данном кадре */
    float CameraCurrentFOV;

    /** скорость интерполяции для масштабирования при прицеливании */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float ZoomInterpSpeed;

    /** коэффициент, скорость поворота, когда вы не целитесь. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipTurnRate;

    /** коэффициент, скорость наклона, когда не целитесь. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipLookUpRate;

    /** коэффициент, скорость поворота, когда вы целитесь. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingTurnRate;

    /** коэффициент, скорость наклона, когда вы целитесь. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimingLookUpRate;

    /** Определяет разброс перекрестия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairSpreadMultiplier;

    /** Компонент скорости для разброса перекрестия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairVelocityFactor;

    /** компонент для разброса перекрестия в воздухе*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairInAirFactor;

    /** Компонент прицеливания для разброса перекрестия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairAimFactor;

    /** Компонент стрельбы для разброса перекрестия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairShootingFactor;

    float ShootTimeDuration;
    bool bFiringBullet;
    FTimerHandle CrosshairShootTimer;

    /** Нажата левая кнопка мыши */
    bool bFireButtonPressed;

    /** true, когда мы можем стрелять. False при ожидании таймера */
    bool bShouldFire;

    /** Скорострельность автоматического оружия */
    float AutomaticFireRate;

    /** Устанавливает таймер между выстрелами */
    FTimerHandle AutoFireTimer;

    /** Истинно, если мы должны отслеживать в каждом кадре для элементов */
    bool bShouldTraceForItems;

    /** Количество пересечений AItems */
    int8 OverlappedItemCount;

    /** AItem, в который попала трасировка в последнем кадре */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    class AItem* TraceHitItemLastFrame;

    /** Текущее экипированное оружие */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AWeapon* EquippedWeapon;

    /** Установите это в Blueprints для класса оружия по умолчанию. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AWeapon> DefaultWeaponClass;

    /** Элемент, который в настоящее время попадает в нашу трассировку в TraceForItems (может быть нулевым) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    AItem* TraceHitItem;

    /** Расстояние вперед от камеры до пункта назначения interp */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpDistance;

    /** Расстояние вверх от камеры до пункта назначения interp */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float CameraInterpElevation;

    /** Map для отслеживания боеприпасов различных типов */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    TMap<EAmmoType, int32> AmmoMap;

    /** Начальное количество патронов калибра 9 мм */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
    int32 Starting9mmAmmo;

    /** Начальное количество боеприпасов AR */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
    int32 StartingARAmmo;

    /** Боевое состояние, может стрелять или перезаряжаться только в Unoccupied. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    ECombatState CombatState;

    /** Монтаж анимации перезарядки */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UAnimMontage* ReloadMontage;

    /** Монтаж анимации экипировывания */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UAnimMontage* EquipMontage;

    UFUNCTION(BlueprintCallable)
    void FinishReloading();

    UFUNCTION(BlueprintCallable)
    void FinishEquipping();

    /** Трансформация обоймы, когда мы впервые берем обойму во время перезарядки */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    FTransform ClipTransform;

    /** Компонент сцены для прикрепления к руке персонажа во время перезарядки */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    USceneComponent* HandSceneComponent;

    /** Правда при приседании */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bCrouching;

    /** Обычная скорость передвижения */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseMovementSpeed;

    /** Скорость передвижения в приседе */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchMovementSpeed;

    /** Текущая половина высоты капсулы */
    float CurrentCapsuleHalfHeight;

    /** Половина высоты капсулы, когда не приседаешь */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float StandingCapsuleHalfHeight;

    /** Половина высоты капсулы при приседании */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchingCapsuleHalfHeight;

    /** Трение о землю, не приседая */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float BaseGroundFriction;

    /** Трение о землю при приседании */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float CrouchingGroundFriction;

    /** Используется для определения момента нажатия кнопки прицеливания. */
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

    /** Массив структур местоположения взаимодействия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TArray<FInterpLocation> InterpLocations;

    FTimerHandle PickupSoundTimer;
    FTimerHandle EquipSoundTimer;

    bool bShouldPlayPickupSound;
    bool bShouldPlayEquipSound;

    void ResetPickupSoundTimer();
    void ResetEquipSoundTimer();

    /** Время интервала между проигрованием звука поднятия */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float PickupSoundResetTime;

    /** Время интервала между проигрованием звука экипировывания */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
    float EquipSoundResetTime;

    /** Массив AItems для нашего инвентаря */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    TArray<AItem*> Inventory;

    const int32 INVENTORY_CAPACITY{ 6 };

    /** Делегат для отправки информации о слоте в InventoryBar при экипировке */
    UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
    FEquipItemDelegate EquipItemDelegate;

public:

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void TurnAround(float Amount);
    void LookUp(float Amount);

    /** Вызывается при нажатии кнопки Fire */
    void FireWeapon();

    FORCEINLINE bool GetAiming() const { return bAiming; }

    UFUNCTION(BlueprintCallable)
    float GetCrosshairSpreadMultiplier() const;

    FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

    /** Добавляет/вычитает из OverlappedItemCount и обновляет bShouldTraceForItems */
    void IncrementOverlappedItemCount(int8 Amount);

    // возращает коор. точки перед камерой
    //FVector GetCameraInterpLocation();

    // поднятие предмета с иртерполяцией
    void GetPickupItem(AItem* Item);

    FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
    FORCEINLINE bool GetCrouching() const { return bCrouching; }
    FInterpLocation GetInterpLocation(int32 Index);

    // Возвращает индекс в массиве InterpLocations с наименьшим значением ItemCount.
    int32 GetInterpLocationIndex();

    void IncrementInterpLocItemCount(int32 Index, int32 Amount);

    FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
    FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

    void StartPickupSoundTimer();
    void StartEquipSoundTimer();
};
