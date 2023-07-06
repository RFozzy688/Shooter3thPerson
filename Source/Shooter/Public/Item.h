// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    EIR_Damaged UMETA(DisplayName = "Damaged"),
    EIR_Common UMETA(DisplayName = "Common"),
    EIR_Uncommon UMETA(DisplayName = "Uncommon"),
    EIR_Rare UMETA(DisplayName = "Rare"),
    EIR_Legendary UMETA(DisplayName = "Legendary"),

    EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup UMETA(DisplayName = "Pickup"),
    EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
    EIS_PickedUp UMETA(DisplayName = "PickedUp"),
    EIS_Equipped UMETA(DisplayName = "Equipped"),
    EIS_Falling UMETA(DisplayName = "Falling"),

    EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    EIT_Ammo UMETA(DisplayName = "Ammo"),
    EIT_Weapon UMETA(DisplayName = "Weapon"),

    EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    AItem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /** Вызывается при перекрытии AreaSphere */
    UFUNCTION()
    void OnSphereOverlap(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

    /** Вызывается при завершении перекрытия AreaSphere */
    UFUNCTION()
    void OnSphereEndOverlap(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);

    /** Устанавливает массив логических значений ActiveStars в зависимости от редкости */
    void SetActiveStars();

    // устанавливает свойства компонента предмета в зависимости от базового состояния
    virtual void SetItemProperties(EItemState State);

    /** Вызывается, когда ItemInterpTimer завершен */
    void FinishInterping();

    /** Обрабатывает интерполяцию элементов в состоянии EquipInterping. */
    void ItemInterp(float DeltaTime);

    /** Получить местоположение взаимодействия на основе типа элемента */
    FVector GetInterpLocation();

    void PlayPickupSound();

    virtual void InitializeCustomDepth();

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called in AShooterCharacter::GetPickupItem
    void PlayEquipSound();

private:
    /** Skeletal Mesh для предмета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    /** Трассировка линии сталкивается с полем для отображения виджетов HUD */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CollisionBox;

    /** Всплывающий виджет, когда игрок смотрит на предмет */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* PickupWidget;

    /** Включает отслеживание элементов при прересечении */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* AreaSphere;

    /** Имя, которое появляется на Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FString ItemName;

    /** Количество предметов (патроны, итд.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 ItemCount;

    /** Item rarity - определяет количество звезд в Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemRarity ItemRarity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    TArray<bool> ActiveStars;

    /** Состояние предмета */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemState ItemState;

    /** Асет кривой, используемый для расположения элемента по оси Z при интерполяции. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UCurveFloat* ItemZCurve;

    /** Исходное местоположение, когда начинается интерполяция */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector ItemInterpStartLocation;
    /** точка взаимодействия перед камерой */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector CameraTargetLocation;
    /** true при интерполяции */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    bool bInterping;

    /** таймер интерполяции */
    FTimerHandle ItemInterpTimer;
    /** Продолжительность кривой и таймера */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float ZCurveTime;

    /** Указатель на character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class AShooterCharacter* Character;

    /** X и Y для элемента во время интерполяции в состоянии EquipInterping */
    float ItemInterpX;
    float ItemInterpY;

    /** Начальное смещение рыскания между камерой и промежуточным элементом */
    float InterpInitialYawOffset;

    /** Кривая, используемая для масштабирования элемента при вставке */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* ItemScaleCurve;

    /** Звук воспроизводится при поднятии предмета */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USoundCue* PickupSound;

    /** Звук, воспроизводимый при экипировке предмета */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USoundCue* EquipSound;

    /** Enum для типа элемента, которым является этот элемент */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemType ItemType;

    /** Индекс места интерполяции, в который интерпретируется этот элемент */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 InterpLocIndex;

public:

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
    FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    void SetItemState(EItemState State);
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
    FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
    FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
    FORCEINLINE int32 GetItemCount() const { return ItemCount; }

    /** Вызывается из класса AShooterCharacter */
    void StartItemCurve(AShooterCharacter* Char);

    virtual void EnableCustomDepth();
    virtual void DisableCustomDepth();
};
