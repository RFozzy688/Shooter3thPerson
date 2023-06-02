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
AShooterCharacter::AShooterCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // создаЄт объект CameraBoom (подт€гиваетс€ к персонажу при столкновении)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f; // длина селфи-палки
    CameraBoom->bUsePawnControlRotation = true; // использует вращение пешки
    CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; //  амера не вращаетс€ относительно CameraBoom

    // персонаж не вращаетс€, когда вращаетс€ контроллер.  онтроллер вли€ет только на камеру.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Ќастраиваем движение персонажа
    GetCharacterMovement()->bOrientRotationToMovement = false; //(true) ѕерсонаж движетс€ в направлении ввода...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... при этой частоте вращени€
    GetCharacterMovement()->JumpZVelocity = 600.f; // скорость прыжка
    GetCharacterMovement()->AirControl = 0.2f; // контроль персонажа в воздухе
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
    Super::BeginPlay();

}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
    FVector2D ViewportSize;

    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize); // ѕолучить текущий размер viewport
    }

    // ѕолучаем местоположение прицела в пространстве экрана
    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    // ѕолучить положение в мире и направление прицела
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

        // ”станавливаем конечную точку луча в конечную точку трассировки
        OutBeamLocation = EndScreenTrace;

        // “рассировка с местоположени€ перекрести€ прицела
        GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, StartScreenTrace, EndScreenTrace, ECollisionChannel::ECC_Visibility);

        if (ScreenTraceHit.bBlockingHit) // было ли попадание
        {
            OutBeamLocation = ScreenTraceHit.Location; // ”станавливаем конечную точку попадани€

            //DrawDebugLine(GetWorld(), StartScreenTrace, EndScreenTrace, FColor::Red, false, 5.f);
            //DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 5.f);
        }

        // ¬ыполн€ем вторую трассировку, на этот раз от ствола оруди€
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

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
        AddControllerYawInput(Amount);
    }
}

void AShooterCharacter::LookUp(float Amount)
{
    if (Controller && Amount)
    {
        AddControllerPitchInput(Amount);
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
                // —оздаем попадание частиц после обновлени€ BeamEnd
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

