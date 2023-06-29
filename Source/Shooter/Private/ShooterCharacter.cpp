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

// Sets default values
AShooterCharacter::AShooterCharacter() :
    CameraDefaultFOV(0.f),
    CameraZoomedFOV(40.f),
    CameraCurrentFOV(0.f),
    ZoomInterpSpeed(20.f),
    // ������������ ���������������� ���� 
    MouseHipTurnRate(1.0f),
    MouseHipLookUpRate(1.0f),
    MouseAimingTurnRate(0.2f),
    MouseAimingLookUpRate(0.2f),
    // ������� �������� �������
    CrosshairSpreadMultiplier(0.f),
    CrosshairVelocityFactor(0.f),
    CrosshairInAirFactor(0.f),
    CrosshairAimFactor(0.f),
    CrosshairShootingFactor(0.f),
    // ���������� �������
    ShootTimeDuration(0.05f),
    bFiringBullet(false),
    // �������������� ����, ���������� 
    AutomaticFireRate(0.1f),
    bShouldFire(true),
    bFireButtonPressed(false),
    // �������� ����������, ���������� 
    bShouldTraceForItems(false),
    OverlappedItemCount(0),
    // �������������� �������������� � �������, ���������� 
    CameraInterpDistance(250.f),
    CameraInterpElevation(65.f),
    // ��������� ���������� �����������
    Starting9mmAmmo(85),
    StartingARAmmo(120),
    // ������ ����������
    CombatState(ECombatState::ECS_Unoccupied),
    bCrouching(false),
    BaseMovementSpeed(650.f),
    CrouchMovementSpeed(300.f)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // ������ ������ CameraBoom (������������� � ��������� ��� ������������)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f; // ����� �����-�����
    CameraBoom->bUsePawnControlRotation = true; // ���������� �������� �����
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // ������ �� ��������� ������������ CameraBoom

    // �������� �� ���������, ����� ��������� ����������. ���������� ������ ������ �� ������.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // ����������� �������� ���������
    GetCharacterMovement()->bOrientRotationToMovement = false; //(true) �������� �������� � ����������� �����...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... ��� ���� ������� ��������
    GetCharacterMovement()->JumpZVelocity = 600.f; // �������� ������
    GetCharacterMovement()->AirControl = 0.2f; // �������� ��������� � �������

    // Create Hand Scene Component 
    HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));
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

    // Spawn ���������� ������ � ������������ ��� � ����
    EquipWeapon(SpawnDefaultWeapon());

    InitializeAmmoMap();

    GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
    // �������� �� ��������� ���������� �� �����������
    FHitResult CrosshairHitResult;
    bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

    if (bCrosshairHit)
    {
        // ��������������� ������������ ���� - ��� ��� ����� ���������� �� ������ ������
        OutBeamLocation = CrosshairHitResult.Location;
    }
    else // ��� ��������� ���������� �� �����������
    {
        // OutBeamLocation �������� ��������� �������� ����������
    }

    // ��������� ������ �����������, �� ���� ��� �� ������ ������
    FHitResult WeaponTraceHit;
    const FVector WeaponTraceStart{ MuzzleSocketLocation };
    const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
    const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

    GetWorld()->LineTraceSingleByChannel(
        WeaponTraceHit,
        WeaponTraceStart,
        WeaponTraceEnd,
        ECollisionChannel::ECC_Visibility);

    if (WeaponTraceHit.bBlockingHit) // ������ ����� ������� � BeamEndPoint?
    {
        OutBeamLocation = WeaponTraceHit.Location;
        return true;
    }

    return false;
}

void AShooterCharacter::AimingButtonPressed()
{
    bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
    bAiming = false;
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
    // ���������� ������� ���� ������ ������
    if (bAiming)
    {
        // ������������ ��� ���������� FOV
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
    }
    else
    {
        // ������������ � FOV �� ���������
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
    }
    GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ��������� ������������ ��� ���� ��� ������������
    CameraInterpZoom(DeltaTime);

    // ���������� ��������� �������� �������
    CalculateCrosshairSpread(DeltaTime);

    // ���������� ������������ ���������
    TraceForItems();
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
    // �������� ������ ���������
    CombatState = ECombatState::ECS_Unoccupied;
    if (EquippedWeapon == nullptr) return;
    const auto AmmoType{ EquippedWeapon->GetAmmoType() };

    // �������� ����� �����������
    if (AmmoMap.Contains(AmmoType))
    {
        // ���������� �����������, ������� �������� ����� � �����, ��� ������������ ������.
        int32 CarriedAmmo = AmmoMap[AmmoType];

        // �������� ����� � �������� ������������ ������
        const int32 MagEmptySpace =
            EquippedWeapon->GetMagazineCapacity() -
            EquippedWeapon->GetAmmo();

        if (MagEmptySpace > CarriedAmmo)
        {
            // �������� ������� ����� ������������, ������� � ��� ����.
            EquippedWeapon->ReloadAmmo(CarriedAmmo);
            CarriedAmmo = 0;
        }
        else
        {
            // ��������� �������
            EquippedWeapon->ReloadAmmo(MagEmptySpace);
            CarriedAmmo -= MagEmptySpace;
        }

        AmmoMap.Add(AmmoType, CarriedAmmo);
    }
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
        // ���� ��������
        PlayFireSound();
        // �������� ����
        SendBullet();
        // ��������������� ������� �������� �� �����
        PlayGunfireMontage();
        // �������� ������� �� ���������� �������� � ������
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

    // ���������� ������� � �������
    if (GetCharacterMovement()->IsFalling()) // is in air?
    {
        // ��������� ���������� ������� � �������
        CrosshairInAirFactor = FMath::FInterpTo(
            CrosshairInAirFactor,
            2.25f,
            DeltaTime,
            2.25f);
    }
    else // �������� ��������� �� �����
    {
        // ������ �������� �������, �������� �� �����
        CrosshairInAirFactor = FMath::FInterpTo(
            CrosshairInAirFactor,
            0.f,
            DeltaTime,
            30.f);
    }

    // �������� �������������
    if (GetAiming())
    {
        // ������� ������� ��� ������������
        CrosshairAimFactor = FMath::FInterpTo(
            CrosshairAimFactor,
            0.5f,
            DeltaTime,
            2.25f);
    }
    else // �������� �� �������������
    {
        // ������� ������� � ���������� ���������
        CrosshairAimFactor = FMath::FInterpTo(
            CrosshairAimFactor,
            0.f,
            DeltaTime,
            30.f);
    }

    // True ����� 0,05 ������� ����� ��������
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

FVector AShooterCharacter::GetCameraInterpLocation()
{
    const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
    const FVector CameraForward{ FollowCamera->GetForwardVector() };
    // Desired = CameraWorldLocation + Forward * A + Up * B
    return CameraWorldLocation + CameraForward * CameraInterpDistance
        + FVector(0.f, 0.f, CameraInterpElevation);
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
    if (Item->GetEquipSound())
    {
        UGameplayStatics::PlaySound2D(this, Item->GetEquipSound());
    }

    auto Weapon = Cast<AWeapon>(Item);
    if (Weapon)
    {
        SwapWeapon(Weapon);
    }
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
        GEngine->GameViewport->GetViewportSize(ViewportSize); // �������� ������� ������ viewport
    }

    // �������� �������������� ������� � ������������ ������
    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    // �������� ��������� � ���� � ����������� �������
    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection);

    if (bScreenToWorld) // deprojection ������ �������?
    {
        FHitResult ScreenTraceHit;
        const FVector Start{ CrosshairWorldPosition };
        const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };
        OutHitLocation = End;

        // ����������� �� �������������� Crosshair ������
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
                // �������� ������ ��������� ��������
                TraceHitItem->GetPickupWidget()->SetVisibility(true);
            }

            // AItem � ������� �� ������ � ��������� ����� 
            if (TraceHitItemLastFrame)
            {
                if (TraceHitItem != TraceHitItemLastFrame)
                {
                    // ���� �� �� ������� �� AItem �� ����������� �����, �� ������� ���
                    // ��� AItem ����� �������� null.
                    TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
                }
            }
            // ��������� ������ �� HitItem ��� ���������� �����
            TraceHitItemLastFrame = TraceHitItem;
        }
    }
    else if (TraceHitItemLastFrame)
    {
        // ������ ������� ��������� �� �������������,
         // ��������� ���� �������� �� ������ ���������� ������
        TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
    }
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
    // ��������� ���������� TSubclassOf
    if (DefaultWeaponClass)
    {
        // Spawn ������
        return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
    }

    return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
    if (WeaponToEquip)
    {
        // �������� ����� ����
        const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

        if (HandSocket)
        {
            // ���������� ������ � ������ ���� RightHandSocket
            HandSocket->AttachActor(WeaponToEquip, GetMesh());
        }

        // ���������� EquippedWeapon �� ������� ��������� ������.
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

        if (TraceHitItem->GetPickupSound())
        {
            UGameplayStatics::PlaySound2D(this, TraceHitItem->GetPickupSound());
        }
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
                // ������� ��������� ������ ����� ���������� BeamEnd
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

    // ��������� ������ �������� ��� �������
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

    // � ��� ���� ���������� ����������� ����?
    if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
    {
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

    // ������ ����� �� ������������� ������
    int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
    // ��������� ������������� ��������
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
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
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
