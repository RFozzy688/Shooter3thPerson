// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Curves/CurveVector.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

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
    bInterping(false),
    ItemInterpX(0.f),
    ItemInterpY(0.f),
    InterpInitialYawOffset(0.f),
    ItemType(EItemType::EIT_MAX),
    InterpLocIndex(0),
    MaterialIndex(0),
    bCanChangeCustomDepth(true),
    // Dynamic Material Parameters
    GlowAmount(150.f),
    FresnelExponent(3.f),
    FresnelReflectFraction(4.f),
    PulseCurveTime(5.f),
    SlotIndex(0),
    bCharacterInventoryFull(false)
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

    // устанавливает пользовательскую глубину как выкл
    InitializeCustomDepth();

    StartPulseTimer();
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
            ShooterCharacter->UnHighlightInventorySlot();
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
        ItemMesh->SetVisibility(true);
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
    case EItemState::EIS_PickedUp:
        PickupWidget->SetVisibility(false);
        // Set mesh properties
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetEnableGravity(false);
        ItemMesh->SetVisibility(false);
        ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
        // Вычтите 1 из счетчика элементов структуры местоположения промежуточного звена.
        Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
        Character->GetPickupItem(this);

        Character->UnHighlightInventorySlot();
    }

    // Вернуть масштаб в нормальное состояние
    SetActorScale3D(FVector(1.f));

    DisableGlowMaterial();
    bCanChangeCustomDepth = true;
    DisableCustomDepth();
}

void AItem::ItemInterp(float DeltaTime)
{
    if (!bInterping) return;

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
        const FVector CameraInterpLocation{ GetInterpLocation() };

        // Вектор от объекта к местоположению камеры, X и Y обнуляются
        const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
        // Масштабный коэффициент для умножения на CurveValue
        const float DeltaZ = ItemToCamera.Size();

        const FVector CurrentLocation{ GetActorLocation() };
        // Интерполированное значение X
        const float InterpXValue = FMath::FInterpTo(
            CurrentLocation.X,
            CameraInterpLocation.X,
            DeltaTime,
            30.0f);
        // Интерполированное значение Y
        const float InterpYValue = FMath::FInterpTo(
            CurrentLocation.Y,
            CameraInterpLocation.Y,
            DeltaTime,
            30.f);

        // Установите для X и Y ItemLocation значения Interped.
        ItemLocation.X = InterpXValue;
        ItemLocation.Y = InterpYValue;

        // Добавление значения кривой к компоненту Z начального местоположения (в масштабе DeltaZ)
        ItemLocation.Z += CurveValue * DeltaZ;
        SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

        // Вращение камеры в этом кадре
        const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
        // Вращение камеры плюс начальное смещение рыскания
        FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
        SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

        if (ItemScaleCurve)
        {
            const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
            SetActorScale3D(FVector(ScaleCurveValue/*, ScaleCurveValue, ScaleCurveValue*/));
        }
    }
}

FVector AItem::GetInterpLocation()
{
    if (Character == nullptr) return FVector(0.f);

    switch (ItemType)
    {
    case EItemType::EIT_Ammo:
        return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
        break;
    
    case EItemType::EIT_Weapon:
        return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
        break;
    }
    
    return FVector();

    //if (ItemType == EItemType::EIT_Ammo)
    //{
    //    return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
    //}
    //else
    //{
    //    //return Character->GetCameraInterpLocation();
    //    return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
    //}
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
    if (Character)
    {
        if (bForcePlaySound)
        {
            if (PickupSound)
            {
                UGameplayStatics::PlaySound2D(this, PickupSound);
            }
        }
        else if (Character->ShouldPlayPickupSound())
        {
            Character->StartPickupSoundTimer();
            if (PickupSound)
            {
                UGameplayStatics::PlaySound2D(this, PickupSound);
            }
        }
    }
}

void AItem::InitializeCustomDepth()
{
    DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
    // Загрузите данные в таблицу данных о редкости предметов.

    // Путь к таблице данных о редкости предметов
    FString RarityTablePath(TEXT("DataTable'/Game/_Game/DataTable/ItemRarityDataTable.ItemRarityDataTable'"));
    UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));
    if (RarityTableObject)
    {
        FItemRarityTable* RarityRow = nullptr;
        switch (ItemRarity)
        {
        case EItemRarity::EIR_Damaged:
            RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
            break;
        case EItemRarity::EIR_Common:
            RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
            break;
        case EItemRarity::EIR_Uncommon:
            RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
            break;
        case EItemRarity::EIR_Rare:
            RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
            break;
        case EItemRarity::EIR_Legendary:
            RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
            break;
        }

        if (RarityRow)
        {
            GlowColor = RarityRow->GlowColor;
            LightColor = RarityRow->LightColor;
            DarkColor = RarityRow->DarkColor;
            NumberOfStars = RarityRow->NumberOfStars;
            IconBackground = RarityRow->IconBackground;

            if (GetItemMesh())
            {
                GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
            }
        }
    }

    if (MaterialInstance)
    {
        DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
        DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
        ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);

        EnableGlowMaterial();
    }
}

void AItem::EnableGlowMaterial()
{
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
    }
}

void AItem::UpdatePulse()
{
    float ElapsedTime{};
    FVector CurveValue{};

    switch (ItemState)
    {
    case EItemState::EIS_Pickup:
        if (PulseCurve)
        {
            ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
            CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
        }
        break;
    case EItemState::EIS_EquipInterping:
        if (InterpPulseCurve)
        {
            ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
            CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
        }
        break;
    }

    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
    }
}

void AItem::ResetPulseTimer()
{
    StartPulseTimer();
}

void AItem::StartPulseTimer()
{
    if (ItemState == EItemState::EIS_Pickup)
    {
        GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
    }
}

void AItem::DisableGlowMaterial()
{
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
    }
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Обработка Interping Item в состоянии EquipInterping
    ItemInterp(DeltaTime);

    // Получить значения кривой из PulseCurve и установить параметры динамического материала
    UpdatePulse();
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
    if (Character)
    {
        if (bForcePlaySound)
        {
            if (EquipSound)
            {
                UGameplayStatics::PlaySound2D(this, EquipSound);
            }
        }
        else if (Character->ShouldPlayEquipSound())
        {
            Character->StartEquipSoundTimer();
            if (EquipSound)
            {
                UGameplayStatics::PlaySound2D(this, EquipSound);
            }
        }
    }
}

void AItem::SetItemState(EItemState State)
{
    ItemState = State;
    SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
    // хранит дескриптор Character
    Character = Char;

    // Получить индекс массива в InterpLocations с наименьшим количеством элементов
    InterpLocIndex = Character->GetInterpLocationIndex();
    // Добавьте 1 к счетчику элементов для этой структуры местоположения промежуточного звена.
    Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

    PlayPickupSound(bForcePlaySound);

    // Сохранить начальное местоположение Item
    ItemInterpStartLocation = GetActorLocation();
    bInterping = true;
    SetItemState(EItemState::EIS_EquipInterping);
    GetWorldTimerManager().ClearTimer(PulseTimer);

    GetWorldTimerManager().SetTimer(
        ItemInterpTimer,
        this,
        &AItem::FinishInterping,
        ZCurveTime);

    // Получить начальный поворот камеры
    const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
    // Получить начальный поворот предмета
    const float ItemRotationYaw{ GetActorRotation().Yaw };
    // Начальное смещение рыскания между камерой и объектом
    InterpInitialYawOffset = 180; /*ItemRotationYaw - CameraRotationYaw;*/

    bCanChangeCustomDepth = false;
}

void AItem::EnableCustomDepth()
{
    if (bCanChangeCustomDepth)
    {
        ItemMesh->SetRenderCustomDepth(true);
    }
}

void AItem::DisableCustomDepth()
{
    if (bCanChangeCustomDepth)
    {
        ItemMesh->SetRenderCustomDepth(false);
    }
}

