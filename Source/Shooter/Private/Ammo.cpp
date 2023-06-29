// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

AAmmo::AAmmo()
{
    // Создать компонент AmmoMesh и установите его в качестве корня.
    AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
    SetRootComponent(AmmoMesh);

    GetCollisionBox()->SetupAttachment(GetRootComponent());
    GetPickupWidget()->SetupAttachment(GetRootComponent());
    GetAreaSphere()->SetupAttachment(GetRootComponent());
}

void AAmmo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
    Super::BeginPlay();
}
