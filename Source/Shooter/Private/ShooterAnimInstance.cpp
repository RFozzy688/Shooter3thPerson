// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
    Speed(0.f),
    bIsInAir(false),
    bIsAccelerating(false),
    MovementOffsetYaw(0.f),
    LastMovementOffsetYaw(0.f),
    bAiming(false),
    CharacterYaw(0.f),
    CharacterYawLastFrame(0.f),
    RootYawOffset(0.f),
    Pitch(0.f),
    bReloading(false),
    OffsetState(EOffsetState::EOS_Hip)
{
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (ShooterCharacter == nullptr)
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    }

    if (ShooterCharacter)
    {
        bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

        // Получить горизонтальную скорость персонажа из скорости
        FVector Velocity{ ShooterCharacter->GetVelocity() };
        Velocity.Z = 0; // это не будет влиять на скорость если персонаж летит вверх или падает вниз
        Speed = Velocity.Size();

        // Персонаж в воздухе?
        bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

        // Персонаж ускоряется?
        if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
        {
            bIsAccelerating = true;
        }
        else
        {
            bIsAccelerating = false;
        }

        FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        bAiming = ShooterCharacter->GetAiming();

        if (bReloading)
        {
            OffsetState = EOffsetState::EOS_Reloading;
        }
        else if (bIsInAir)
        {
            OffsetState = EOffsetState::EOS_InAir;
        }
        else if (ShooterCharacter->GetAiming())
        {
            OffsetState = EOffsetState::EOS_Aiming;
        }
        else
        {
            OffsetState = EOffsetState::EOS_Hip;
        }
    }

    TurnInPlace();
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
    if (ShooterCharacter == nullptr) return;

    Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

    if (Speed > 0 || bIsInAir)
    {
        // Не поворачиваться на месте; Персонаж движется
        RootYawOffset = 0.f;
        CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
        CharacterYawLastFrame = CharacterYaw;
        RotationCurveLastFrame = 0.f;
        RotationCurve = 0.f;
    }
    else
    {
        CharacterYawLastFrame = CharacterYaw;
        CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
        const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

        // Смещение Root Yaw Offset, обновлено и ограничено на [-180, 180]
        RootYawOffset -= YawDelta;

        // 1.0 if turning, 0.0 if not
        const float Turning{ GetCurveValue(TEXT("Turning")) };
        if (Turning > 0)
        {
            RotationCurveLastFrame = RotationCurve;
            RotationCurve = GetCurveValue(TEXT("Rotation"));
            const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

            // RootYawOffset > 0, -> Поворот налево. RootYawOffset < 0, -> Поворот направо.
            RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

            const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
            if (ABSRootYawOffset > 90.f)
            {
                const float YawExcess{ ABSRootYawOffset - 90.f };
                RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
            }
        }

        if (GEngine) GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
    }
}
