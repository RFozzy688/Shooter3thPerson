// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // создаёт объект CameraBoom (подтягивается к персонажу при столкновении)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f; // длина селфи-палки
    CameraBoom->bUsePawnControlRotation = true; // использует вращение пешки

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // Камера не вращается относительно CameraBoom
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
    Super::BeginPlay();

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

}

void AShooterCharacter::MoveForward(float Amount)
{
    if (Controller && Amount)
    {
        AddMovementInput(GetActorForwardVector(), Amount);
    }
}

void AShooterCharacter::MoveRight(float Amount)
{
    if (Controller && Amount)
    {
        AddMovementInput(GetActorRightVector(), Amount);
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

