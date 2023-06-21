// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Curves/CurveVector.h"

// Sets default values
AItem::AItem():
    ItemName(FString("Default")),
    ItemCount(0),
    ItemRarity(EItemRarity::EIR_Common),
    ItemState(EItemState::EIS_Pickup),
    // элементы иртерполяции, переменные
    ZCurveTime(0.7f),
    ItemInterpStartLocation(FVector(0.f)),
    CameraTargetLocation(FVector(0.f)),
    bInterping(false)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(ItemMesh);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(
        ECollisionChannel::ECC_Visibility,
        ECollisionResponse::ECR_Block);

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidget->SetupAttachment(GetRootComponent());

    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();
    
    // спрятать блюпринт виджета
    if (true)
    {
        PickupWidget->SetVisibility(false);
    }

    // Устанавливает массив ActiveStars на основе редкости предмета
    SetActiveStars();

    // устанавливает пересечение для сферы площади
    AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

    // Установить свойства элемента на основе ItemState
    SetItemProperties(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
        if (ShooterCharacter)
        {
            ShooterCharacter->IncrementOverlappedItemCount(1);
        }
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
        if (ShooterCharacter)
        {
            ShooterCharacter->IncrementOverlappedItemCount(-1);
        }
    }
}

void AItem::SetActiveStars()
{
    for (int32 i = 0; i < 5; i++)
    {
        ActiveStars.Add(false);
    }

    switch (ItemRarity)
    {
    case EItemRarity::EIR_Damaged:
        ActiveStars[0] = true;
        break;
    case EItemRarity::EIR_Common:
        ActiveStars[0] = true;
        ActiveStars[1] = true;
        break;
    case EItemRarity::EIR_Uncommon:
        ActiveStars[0] = true;
        ActiveStars[1] = true;
        ActiveStars[2] = true;
        break;
    case EItemRarity::EIR_Rare:
        ActiveStars[0] = true;
        ActiveStars[1] = true;
        ActiveStars[2] = true;
        ActiveStars[3] = true;
        break;
    case EItemRarity::EIR_Legendary:
        ActiveStars[0] = true;
        ActiveStars[1] = true;
        ActiveStars[2] = true;
        ActiveStars[3] = true;
        ActiveStars[4] = true;
        break;
    }
}

void AItem::SetItemProperties(EItemState State)
{
    switch (State)
    {
    case EItemState::EIS_Pickup:
        // устанавливаем свойства меша
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetEnableGravity(false);
        ItemMesh->SetVisibility(true);
        ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Установить свойства AreaSphere
        AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        // Установить свойства CollisionBox
        CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        CollisionBox->SetCollisionResponseToChannel(
            ECollisionChannel::ECC_Visibility,
            ECollisionResponse::ECR_Block);
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
    case EItemState::EIS_EquipInterping:
        PickupWidget->SetVisibility(false);
        // Set mesh properties
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetEnableGravity(false);
        ItemMesh->SetVisibility(true);
        ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Set AreaSphere properties
        AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Set CollisionBox properties
        CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    case EItemState::EIS_PickedUp:
        break;
    case EItemState::EIS_Equipped:
        PickupWidget->SetVisibility(false);
        // Set mesh properties
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetEnableGravity(false);
        ItemMesh->SetVisibility(true);
        ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Set AreaSphere properties
        AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Set CollisionBox properties
        CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    case EItemState::EIS_Falling:
        // Set mesh properties
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        ItemMesh->SetSimulatePhysics(true);
        ItemMesh->SetEnableGravity(true);
        ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionResponseToChannel(
            ECollisionChannel::ECC_WorldStatic,
            ECollisionResponse::ECR_Block);
        // Set AreaSphere properties
        AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // Set CollisionBox properties
        CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    case EItemState::EIS_MAX:
        break;
    default:
        break;
    }
}

void AItem::FinishInterping()
{
    bInterping = false;

    if (Character)
    {
        Character->GetPickupItem(this);
    }
}

void AItem::ItemInterp(float DeltaTime)
{
    if (!bInterping) return;

    UE_LOG(LogTemp, Warning, TEXT("Character: %d"), Character);
    UE_LOG(LogTemp, Warning, TEXT("ItemZCurve: %d"), ItemZCurve);

    if (Character && ItemZCurve)
    {
        // Прошедшее время с момента запуска ItemInterpTimer
        const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
        // Получить значение кривой, соответствующее ElapsedTime
        const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
        //UE_LOG(LogTemp, Warning, TEXT("CurveValue: %f"), CurveValue);
        // Получить начальное местоположение элемента, когда кривая начинается
        FVector ItemLocation = ItemInterpStartLocation;
        // Получить местоположение перед камерой
        const FVector CameraInterpLocation{ Character->GetCameraInterpLocation() };

        // Вектор от объекта к местоположению камеры, X и Y обнуляются
        const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
        // Масштабный коэффициент для умножения на CurveValue
        const float DeltaZ = ItemToCamera.Size();

        // Добавление значения кривой к компоненту Z начального местоположения (в масштабе DeltaZ)
        ItemLocation.Z += CurveValue * DeltaZ;
        SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);
    }
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Обработка Interping Item в состоянии EquipInterping
    ItemInterp(DeltaTime);
    //UE_LOG(LogTemp, Warning, TEXT("Character: %d"), 1);
}

void AItem::SetItemState(EItemState State)
{
    ItemState = State;
    SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
    // хранит дескриптор Character
    Character = Char;
    // Сохранить начальное местоположение Item
    ItemInterpStartLocation = GetActorLocation();
    bInterping = true;
    SetItemState(EItemState::EIS_EquipInterping);

    GetWorldTimerManager().SetTimer(
        ItemInterpTimer,
        this,
        &AItem::FinishInterping,
        ZCurveTime);
}

