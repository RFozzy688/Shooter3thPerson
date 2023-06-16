// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

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
    float CameraDefaultFOV;

    /** Значение поля обзора камеры при увеличении */
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
};
