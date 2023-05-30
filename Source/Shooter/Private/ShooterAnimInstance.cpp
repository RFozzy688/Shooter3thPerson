// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (ShooterCharacter == nullptr)
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    }
    if (ShooterCharacter)
    {
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
    }
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
