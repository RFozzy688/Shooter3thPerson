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
    CrosshairShootingFactor(0.f)
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
        const FVector StartScreenTrace{ CrosshairWorldPosition };
        const FVector EndScreenTrace{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

        // Устанавливаем конечную точку луча в конечную точку трассировки
        OutBeamLocation = EndScreenTrace;

        // Трассировка с местоположения перекрестия прицела
        GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, StartScreenTrace, EndScreenTrace, ECollisionChannel::ECC_Visibility);

        if (ScreenTraceHit.bBlockingHit) // было ли попадание
        {
            OutBeamLocation = ScreenTraceHit.Location; // Устанавливаем конечную точку попадания

            //DrawDebugLine(GetWorld(), StartScreenTrace, EndScreenTrace, FColor::Red, false, 5.f);
            //DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 5.f);
        }

        // Выполняем вторую трассировку, на этот раз от ствола орудия
        FHitResult WeaponTraceHit;
        const FVector WeaponTraceStart{ MuzzleSocketLocation };
        const FVector WeaponTraceEnd{ OutBeamLocation };
        GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

        if (WeaponTraceHit.bBlockingHit) // объект между стволом и BeamEndPoint?
        {
            OutBeamLocation = WeaponTraceHit.Location;

            //DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor::Green, false, 5.f);
            //DrawDebugPoint(GetWorld(), WeaponTraceHit.Location, 5.f, FColor::Green, false, 5.f);
        }
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

    PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

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

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
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

    CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
    return CrosshairSpreadMultiplier;
}

