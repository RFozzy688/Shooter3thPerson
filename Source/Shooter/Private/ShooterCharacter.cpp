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
    bFireButtonPressed(false)
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

    FHitResult ItemTraceResult;
    FVector HitLocation;

    TraceUnderCrosshairs(ItemTraceResult, HitLocation);

    if (ItemTraceResult.bBlockingHit)
    {
        AItem* HitItem = Cast<AItem>(ItemTraceResult.Actor);
        if (HitItem && HitItem->GetPickupWidget())
        {
            // �������� ������ ��������� ��������
            HitItem->GetPickupWidget()->SetVisibility(true);
        }
    }
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

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
    PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

    PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
    PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
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
    if (FireSound)
    {
        UGameplayStatics::PlaySound2D(this, FireSound);
    }

    const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

    if (BarrelSocket)
    {
        const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

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

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
    }

    // ��������� ������ �������� ��� �������
    StartCrosshairBulletFire();
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
    StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
    if (bShouldFire)
    {
        FireWeapon();
        bShouldFire = false;
        GetWorldTimerManager().SetTimer(
            AutoFireTimer,
            this,
            &AShooterCharacter::AutoFireReset,
            AutomaticFireRate);
    }
}

void AShooterCharacter::AutoFireReset()
{
    bShouldFire = true;
    if (bFireButtonPressed)
    {
        StartFireTimer();
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
