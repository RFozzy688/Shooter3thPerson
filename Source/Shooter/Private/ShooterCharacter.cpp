// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Ammo.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
    CameraDefaultFOV(0.f),
    CameraZoomedFOV(40.f),
    CameraCurrentFOV(0.f),
    ZoomInterpSpeed(20.f),
    // коэффициенты чувствительности мыши 
    MouseHipTurnRate(1.0f),
    MouseHipLookUpRate(1.0f),
    MouseAimingTurnRate(0.2f),
    MouseAimingLookUpRate(0.2f),
    // Факторы разброса прицела
    CrosshairSpreadMultiplier(0.f),
    CrosshairVelocityFactor(0.f),
    CrosshairInAirFactor(0.f),
    CrosshairAimFactor(0.f),
    CrosshairShootingFactor(0.f),
    // переменные таймера
    ShootTimeDuration(0.05f),
    bFiringBullet(false),
    // Автоматический огнь, переменные 
    AutomaticFireRate(0.1f),
    bShouldFire(true),
    bFireButtonPressed(false),
    // элементы трасировки, переменные 
    bShouldTraceForItems(false),
    OverlappedItemCount(0),
    // местоположения взаимодействия с камерой, переменные 
    CameraInterpDistance(250.f),
    CameraInterpElevation(65.f),
    // Начальное количество боеприпасов
    Starting9mmAmmo(85),
    StartingARAmmo(120),
    // Боевые переменные
    CombatState(ECombatState::ECS_Unoccupied),
    bCrouching(false),
    BaseMovementSpeed(650.f),
    CrouchMovementSpeed(300.f),
    StandingCapsuleHalfHeight(88.f),
    CrouchingCapsuleHalfHeight(44.f),
    BaseGroundFriction(2.f),
    CrouchingGroundFriction(100.f),
    bAimingButtonPressed(false),
    // Свойства звукового таймера при поднятии
    bShouldPlayPickupSound(true),
    bShouldPlayEquipSound(true),
    PickupSoundResetTime(0.2f),
    EquipSoundResetTime(0.2f)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // создаёт объект CameraBoom (подтягивается к персонажу при столкновении)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f; // длина селфи-палки
    CameraBoom->bUsePawnControlRotation = true; // использует вращение пешки
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // Камера не вращается относительно CameraBoom

    // персонаж не вращается, когда вращается контроллер. Контроллер влияет только на камеру.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Настраиваем движение персонажа
    GetCharacterMovement()->bOrientRotationToMovement = false; //(true) Персонаж движется в направлении ввода...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... при этой частоте вращения
    GetCharacterMovement()->JumpZVelocity = 600.f; // скорость прыжка
    GetCharacterMovement()->AirControl = 0.2f; // контроль персонажа в воздухе

    // Create Hand Scene Component 
    HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

    // Создать компоненты интерполяции
    WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
    WeaponInterpComp->SetupAttachment(GetFollowCamera());

    InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
    InterpComp1->SetupAttachment(GetFollowCamera());

    InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
    InterpComp2->SetupAttachment(GetFollowCamera());

    InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
    InterpComp3->SetupAttachment(GetFollowCamera());

    InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
    InterpComp4->SetupAttachment(GetFollowCamera());

    InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
    InterpComp5->SetupAttachment(GetFollowCamera());

    InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
    InterpComp6->SetupAttachment(GetFollowCamera());
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (FollowCamera)
    {
        CameraDefaultFOV = GetFollowCamera()->FieldOfView;
        CameraCurrentFOV = CameraDefaultFOV;
    }

    // Spawn дефолтного оружия и прикрепление его к мешу
    EquipWeapon(SpawnDefaultWeapon());

    InitializeAmmoMap();

    GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

    // Создание структуры FInterpLocation для каждого промежуточного расположения. Добавить в массив
    InitializeInterpLocations();
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
    // проверка на попадание трасировки от перекрестия
    FHitResult CrosshairHitResult;
    bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

    if (bCrosshairHit)
    {
        // Предварительное расположение луча - все еще нужна трасировка от ствола оружия
        OutBeamLocation = CrosshairHitResult.Location;
    }
    else // нет попадания трасировки от перекрестия
    {
        // OutBeamLocation конечное положение линейной трасировки
    }

    // Выполняем вторую трассировку, на этот раз от ствола орудия
    FHitResult WeaponTraceHit;
    const FVector WeaponTraceStart{ MuzzleSocketLocation };
    const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
    const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

    GetWorld()->LineTraceSingleByChannel(
        WeaponTraceHit,
        WeaponTraceStart,
        WeaponTraceEnd,
        ECollisionChannel::ECC_Visibility);

    if (WeaponTraceHit.bBlockingHit) // объект между стволом и BeamEndPoint?
    {
        OutBeamLocation = WeaponTraceHit.Location;
        return true;
    }

    return false;
}

void AShooterCharacter::AimingButtonPressed()
{
    bAimingButtonPressed = true;

    if (CombatState != ECombatState::ECS_Reloading)
    {
        Aim();
    }
}

void AShooterCharacter::AimingButtonReleased()
{
    bAimingButtonPressed = false;
    StopAiming();
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
    // Установить текущее поле обзора камеры
    if (bAiming)
    {
        // Интерполяция для увеличения FOV
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
    }
    else
    {
        // Интерполяция к FOV по умолчанию
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
    }
    GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Обработка интерполяции для зума при прицеливании
    CameraInterpZoom(DeltaTime);

    // Рассчитать множитель разброса прицела
    CalculateCrosshairSpread(DeltaTime);

    // количество пересеченных предметов
    TraceForItems();

    // Интерполировать половину высоты капсулы на основе приседания/стояния
    InterpCapsuleHalfHeight(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

    PlayerInputComponent->BindAxis("TurnAround", this, &AShooterCharacter::TurnAround);
    PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
    PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

    PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
    PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

    PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
    PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

    PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
}

void AShooterCharacter::FinishReloading()
{
    // Обновить боевое состояние
    CombatState = ECombatState::ECS_Unoccupied;

    if (bAimingButtonPressed)
    {
        Aim();
    }

    if (EquippedWeapon == nullptr) return;
    const auto AmmoType{ EquippedWeapon->GetAmmoType() };

    // Обновить карту боеприпасов
    if (AmmoMap.Contains(AmmoType))
    {
        // Количество боеприпасов, которое персонаж носит с собой, для снаряженного оружия.
        int32 CarriedAmmo = AmmoMap[AmmoType];

        // Осталось места в магазине снаряженного оружия
        const int32 MagEmptySpace =
            EquippedWeapon->GetMagazineCapacity() -
            EquippedWeapon->GetAmmo();

        if (MagEmptySpace > CarriedAmmo)
        {
            // зарядить магазин всеми боеприпасами, которые у нас есть.
            EquippedWeapon->ReloadAmmo(CarriedAmmo);
            CarriedAmmo = 0;
        }
        else
        {
            // заполнить магазин
            EquippedWeapon->ReloadAmmo(MagEmptySpace);
            CarriedAmmo -= MagEmptySpace;
        }

        AmmoMap.Add(AmmoType, CarriedAmmo);
    }
}

void AShooterCharacter::ResetPickupSoundTimer()
{
    bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
    bShouldPlayEquipSound = true;
}

void AShooterCharacter::MoveForward(float Amount)
{
    if (Controller && Amount)
    {
        const FRotator Rotation{ Controller->GetControlRotation() };
        const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

        const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
        AddMovementInput(Direction, Amount);
    }
}

void AShooterCharacter::MoveRight(float Amount)
{
    if (Controller && Amount)
    {
        const FRotator Rotation{ Controller->GetControlRotation() };
        const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

        const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
        AddMovementInput(Direction, Amount);
    }
}

void AShooterCharacter::TurnAround(float Amount)
{
    if (Controller && Amount)
    {
        float TurnScaleFactor{};

        if (bAiming)
        {
            TurnScaleFactor = MouseAimingTurnRate;
        }
        else
        {
            TurnScaleFactor = MouseHipTurnRate;
        }

        AddControllerYawInput(Amount * TurnScaleFactor);
    }
}

void AShooterCharacter::LookUp(float Amount)
{
    if (Controller && Amount)
    {
        float LookUpScaleFactor{};

        if (bAiming)
        {
            LookUpScaleFactor = MouseAimingLookUpRate;
        }
        else
        {
            LookUpScaleFactor = MouseHipLookUpRate;
        }

        AddControllerPitchInput(Amount * LookUpScaleFactor);
    }
}

void AShooterCharacter::FireWeapon()
{
    if (!EquippedWeapon) return;

    if (CombatState != ECombatState::ECS_Unoccupied) return;

    if (WeaponHasAmmo())
    {
        // звук стрельбы
        PlayFireSound();
        // посылает пулю
        SendBullet();
        // воспроизведение монтажа стрельбы от бедра
        PlayGunfireMontage();
        // вычитает единицу из количества патронов в оружии
        EquippedWeapon->DecrementAmmo();

        StartFireTimer();
    }
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
    FVector2D WalkSpeedRange{ 0.f, 600.f };
    FVector2D VelocityMultiplierRange{ 0.f, 1.f };
    FVector Velocity{ GetVelocity() };
    Velocity.Z = 0.f;

    CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
        WalkSpeedRange,
        VelocityMultiplierRange,
        Velocity.Size());

    // расширение прицела в воздухе
    if (GetCharacterMovement()->IsFalling()) // is in air?
    {
        // медленное расширение прицела в воздухе
        CrosshairInAirFactor = FMath::FInterpTo(
            CrosshairInAirFactor,
            2.25f,
            DeltaTime,
            2.25f);
    }
    else // Персонаж находится на земле
    {
        // Быстро сжимайте прицела, находясь на земле
        CrosshairInAirFactor = FMath::FInterpTo(
            CrosshairInAirFactor,
            0.f,
            DeltaTime,
            30.f);
    }

    // персонаж прицеливается
    if (GetAiming())
    {
        // сужение прицела при прицеливании
        CrosshairAimFactor = FMath::FInterpTo(
            CrosshairAimFactor,
            0.5f,
            DeltaTime,
            2.25f);
    }
    else // персонаж не прицеливается
    {
        // возврат прицела к дефолтному состоянию
        CrosshairAimFactor = FMath::FInterpTo(
            CrosshairAimFactor,
            0.f,
            DeltaTime,
            30.f);
    }

    // True через 0,05 секунды после выстрела
    if (bFiringBullet)
    {
        CrosshairShootingFactor = FMath::FInterpTo(
            CrosshairShootingFactor,
            1.1f,
            DeltaTime,
            60.f);
    }
    else
    {
        CrosshairShootingFactor = FMath::FInterpTo(
            CrosshairShootingFactor,
            0.f,
            DeltaTime,
            60.f);
    }

    CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
    return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
    if (OverlappedItemCount + Amount <= 0)
    {
        OverlappedItemCount = 0;
        bShouldTraceForItems = false;
    }
    else
    {
        OverlappedItemCount += Amount;
        bShouldTraceForItems = true;
    }
}

//FVector AShooterCharacter::GetCameraInterpLocation()
//{
//    const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
//    const FVector CameraForward{ FollowCamera->GetForwardVector() };
//    // Desired = CameraWorldLocation + Forward * A + Up * B
//    return CameraWorldLocation + CameraForward * CameraInterpDistance
//        + FVector(0.f, 0.f, CameraInterpElevation);
//}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
    Item->PlayEquipSound();

    auto Weapon = Cast<AWeapon>(Item);
    if (Weapon)
    {
        SwapWeapon(Weapon);
    }

    auto Ammo = Cast<AAmmo>(Item);
    if (Ammo)
    {
        PickupAmmo(Ammo);
    }
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
    if (Index <= InterpLocations.Num())
    {
        return InterpLocations[Index];
    }
    return FInterpLocation();
}

void AShooterCharacter::StartCrosshairBulletFire()
{
    bFiringBullet = true;

    GetWorldTimerManager().SetTimer(
        CrosshairShootTimer,
        this,
        &AShooterCharacter::FinishCrosshairBulletFire,
        ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
    bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
    bFireButtonPressed = true;

    FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
    CombatState = ECombatState::ECS_FireTimerInProgress;

    GetWorldTimerManager().SetTimer(
        AutoFireTimer,
        this,
        &AShooterCharacter::AutoFireReset,
        AutomaticFireRate);
}

void AShooterCharacter::AutoFireReset()
{
    CombatState = ECombatState::ECS_Unoccupied;

    if (WeaponHasAmmo())
    {
        if (bFireButtonPressed)
        {
            FireWeapon();
        }
    }
    else
    {
        // Reload Weapon
        ReloadWeapon();
    }
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
    FVector2D ViewportSize;

    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize); // Получить текущий размер viewport
    }

    // Получаем местоположение прицела в пространстве экрана
    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    // Получить положение в мире и направление прицела
    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection);

    if (bScreenToWorld) // deprojection прошло успешно?
    {
        FHitResult ScreenTraceHit;
        const FVector Start{ CrosshairWorldPosition };
        const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };
        OutHitLocation = End;

        // Трассировка от местоположения Crosshair наружу
        GetWorld()->LineTraceSingleByChannel(
            OutHitResult,
            Start,
            End,
            ECollisionChannel::ECC_Visibility);

        if (OutHitResult.bBlockingHit)
        {
            OutHitLocation = OutHitResult.Location;
            return true;
        }
    }
    return false;
}

void AShooterCharacter::TraceForItems()
{
    if (bShouldTraceForItems)
    {
        FHitResult ItemTraceResult;
        FVector HitLocation;
        TraceUnderCrosshairs(ItemTraceResult, HitLocation);
        if (ItemTraceResult.bBlockingHit)
        {
            TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
            if (TraceHitItem && TraceHitItem->GetPickupWidget())
            {
                // Показать виджет получения предмета
                TraceHitItem->GetPickupWidget()->SetVisibility(true);
            }

            // AItem в который мы попали в последнем кадре 
            if (TraceHitItemLastFrame)
            {
                if (TraceHitItem != TraceHitItemLastFrame)
                {
                    // если мы не смотрим на AItem из предыдущего кадра, то убираем уго
                    // Или AItem имеет значение null.
                    TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
                }
            }
            // Сохранить ссылку на HitItem для следующего кадра
            TraceHitItemLastFrame = TraceHitItem;
        }
    }
    else if (TraceHitItemLastFrame)
    {
        // Больше никаких элементов не перекрывается,
         // Последний кадр элемента не должен отображать виджет
        TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
    }
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
    // Проверить переменную TSubclassOf
    if (DefaultWeaponClass)
    {
        // Spawn оружия
        return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
    }

    return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
    if (WeaponToEquip)
    {
        // Получить сокет руки
        const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

        if (HandSocket)
        {
            // Прикрепите оружие к сокету руки RightHandSocket
            HandSocket->AttachActor(WeaponToEquip, GetMesh());
        }

        // Установите EquippedWeapon на недавно созданное оружие.
        EquippedWeapon = WeaponToEquip;

        EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
    }
}

void AShooterCharacter::DropWeapon()
{
    if (EquippedWeapon)
    {
        FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
        EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

        EquippedWeapon->SetItemState(EItemState::EIS_Falling);
        EquippedWeapon->ThrowWeapon();
    }
}

void AShooterCharacter::SelectButtonPressed()
{
    if (TraceHitItem)
    {
        TraceHitItem->StartItemCurve(this);
    }
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
    DropWeapon();
    EquipWeapon(WeaponToSwap);
    TraceHitItem = nullptr;
    TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
    AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
    AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
    if (EquippedWeapon == nullptr) return false;

    return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
    if (FireSound)
    {
        UGameplayStatics::PlaySound2D(this, FireSound);
    }
}

void AShooterCharacter::SendBullet()
{
    const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");

    if (BarrelSocket)
    {
        const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
        }

        FVector BeamEnd;

        bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

        if (bBeamEnd)
        {
            if (ImpactParticle)
            {
                // Создаем попадание частиц после обновления BeamEnd
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, BeamEnd);
            }

            if (BeamParticle)
            {
                UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, SocketTransform);

                if (Beam)
                {
                    Beam->SetVectorParameter(FName("Target"), BeamEnd);
                }
            }
        }
    }
}

void AShooterCharacter::PlayGunfireMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
    }

    // Запустить таймер стрельбы для прицела
    StartCrosshairBulletFire();
}

void AShooterCharacter::ReloadButtonPressed()
{
    ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
    if (CombatState != ECombatState::ECS_Unoccupied) return;
    if (!EquippedWeapon) return;

    // У нас есть боеприпасы правильного типа?
    if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
    {
        if (bAiming)
        {
            StopAiming();
        }

        CombatState = ECombatState::ECS_Reloading;
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && ReloadMontage)
        {
            AnimInstance->Montage_Play(ReloadMontage);
            AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
        }
    }
}

bool AShooterCharacter::CarryingAmmo()
{
    if (EquippedWeapon == nullptr) return false;

    auto AmmoType = EquippedWeapon->GetAmmoType();

    if (AmmoMap.Contains(AmmoType))
    {
        return AmmoMap[AmmoType] > 0;
    }

    return false;
}

void AShooterCharacter::GrabClip()
{
    if (EquippedWeapon == nullptr) return;
    if (HandSceneComponent == nullptr) return;

    // Индекс кости на экипированном оружии
    int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
    // Сохранить трансформацию магазина
    ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

    FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
    HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
    HandSceneComponent->SetWorldTransform(ClipTransform);

    EquippedWeapon->SetMovingClip(true);
}

void AShooterCharacter::ReleaseClip()
{
    EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::CrouchButtonPressed()
{
    if (!GetCharacterMovement()->IsFalling())
    {
        bCrouching = !bCrouching;
    }

    if (bCrouching)
    {
        GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
        GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
        GetCharacterMovement()->GroundFriction = BaseGroundFriction;
    }
}

void AShooterCharacter::Jump()
{
    if (bCrouching)
    {
        bCrouching = false;
        GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
    }
    else
    {
        ACharacter::Jump();
    }
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
    float TargetCapsuleHalfHeight{};
    if (bCrouching)
    {
        TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
    }
    else
    {
        TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
    }
    const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f) };

    // Отрицательное значение при приседании; Положительное значение, если стоит
    const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
    const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };
    GetMesh()->AddLocalOffset(MeshOffset);

    GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::Aim()
{
    bAiming = true;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AShooterCharacter::StopAiming()
{
    bAiming = false;

    if (!bCrouching)
    {
        GetCharacterMovement()->MaxWalkSpeed = 600.f;
    }
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
    // проверьте, содержит ли AmmoMap AmmoType Ammo
    if (AmmoMap.Find(Ammo->GetAmmoType()))
    {
        // Получите количество боеприпасов в нашей карте боеприпасов для типа боеприпасов
        int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
        AmmoCount += Ammo->GetItemCount();
        // Установите количество боеприпасов на Карте для этого типа
        AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
    }

    if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
    {
        // Проверьте, не пуст ли пистолет
        if (EquippedWeapon->GetAmmo() == 0)
        {
            ReloadWeapon();
        }
    }

    Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocations()
{
    FInterpLocation WeaponLocation{ WeaponInterpComp, 0 };
    InterpLocations.Add(WeaponLocation);

    FInterpLocation InterpLoc1{ InterpComp1, 0 };
    InterpLocations.Add(InterpLoc1);

    FInterpLocation InterpLoc2{ InterpComp2, 0 };
    InterpLocations.Add(InterpLoc2);

    FInterpLocation InterpLoc3{ InterpComp3, 0 };
    InterpLocations.Add(InterpLoc3);

    FInterpLocation InterpLoc4{ InterpComp4, 0 };
    InterpLocations.Add(InterpLoc4);

    FInterpLocation InterpLoc5{ InterpComp5, 0 };
    InterpLocations.Add(InterpLoc5);

    FInterpLocation InterpLoc6{ InterpComp6, 0 };
    InterpLocations.Add(InterpLoc6);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
    int32 LowestIndex = 1;
    int32 LowestCount = INT_MAX;

    for (int32 i = 0; i < InterpLocations.Num(); i++)
    {
        if (InterpLocations[i].ItemCount < LowestCount)
        {
            LowestIndex = i;
            LowestCount = InterpLocations[i].ItemCount;
        }
    }
    return LowestIndex;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
    if (Amount < -1 || Amount > 1) return;

    if (InterpLocations.Num() >= Index)
    {
        InterpLocations[Index].ItemCount += Amount;
    }
}

void AShooterCharacter::StartPickupSoundTimer()
{
    bShouldPlayPickupSound = false;
    GetWorldTimerManager().SetTimer(
        PickupSoundTimer,
        this,
        &AShooterCharacter::ResetPickupSoundTimer,
        PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
    bShouldPlayEquipSound = false;
    GetWorldTimerManager().SetTimer(
        PickupSoundTimer,
        this,
        &AShooterCharacter::ResetEquipSoundTimer,
        EquipSoundResetTime);
}
