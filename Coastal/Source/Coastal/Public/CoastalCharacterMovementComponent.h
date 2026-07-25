// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoastalCharacterMovementComponent.generated.h"

class ACoastalCharacter;

UENUM(BlueprintType)

enum ECustomMovementMode
{
    CMOVE_None UMETA(DisplayName = "Custom None"),
    CMOVE_Skate UMETA(DisplayName = "Custom Skate"),
    CMOVE_MAX UMETA(Hidden),
};

UCLASS(ClassGroup = ("Custom"), meta = (BlueprintSpawnableComponent))

class COASTAL_API UCoastalCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

    class FSavedMove_Coastal : public FSavedMove_Character
    {
    public:
        virtual void Clear() override;
        virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                FNetworkPredictionData_Client_Character& ClientData) override;
        // dictate whether new move is the same and does not need to be sent separately
        virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
        virtual void PrepMoveFor(ACharacter* C) override;
        virtual uint8 GetCompressedFlags() const override;

        // compressed flags
        uint8 Saved_bWantsToSprint : 1;
        uint8 Saved_bHasHitNormalCharacterEquipment : 1;

        // standard remote procedure calls (rpc) flags
        TEnumAsByte<EMovementMode> Saved_PreviousMovementMode;
        TEnumAsByte<ECustomMovementMode> Saved_PreviousCustomMovementMode;
    };

    class FNetworkPredictionData_Client_Coastal : public FNetworkPredictionData_Client_Character
    {
    public:
        FNetworkPredictionData_Client_Coastal(const UCharacterMovementComponent& ClientMovement);

        // Override saved move class with our custom class
        virtual FSavedMovePtr AllocateNewMove() override;
    };

public:
    UCoastalCharacterMovementComponent();

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void InitializeComponent() override;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

    virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

    virtual bool CanAttemptJump() const override;

    virtual float GetMaxSpeed() const override;
    virtual float GetMaxAcceleration() const override;
    virtual float GetMaxBrakingDeceleration() const override;

    virtual void SetDefaultMovementMode() override;

private:
    void EnterSkate() const;
    void ExitSkate();
    void PhysSkate(float DeltaTime, int32 Iterations);

    bool GetHitNormalCharacterEquipment(FVector& HitNormal) const;

public:
    UFUNCTION(BlueprintPure, Category = "Pawn|Components|CharacterMovement|Coastal")
    bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

    UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement|Coastal") void SprintPressed();
    UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement|Coastal") void SprintReleased();

    UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement|Coastal") void SkatePressed();

public:
    UPROPERTY(Category = "Character Movement: MovementMode", EditDefaultsOnly)
    TEnumAsByte<ECustomMovementMode> DefaultCustomMovementMode;

    UPROPERTY(Category = "Character Movement: Walking", EditDefaultsOnly) float MaxSpeedSprintWalking = 1000.f;

    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float MaxSpeedSkating = 1000.f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float MaxSpeedSprintSkating = 1500.f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float MaxAccelerationSkating = 300.f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float FrictionSkating = 0.5f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float BrakingDecelerationSkating = 100.f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float RotationMinDotProductSkating = 0.9998f;
    UPROPERTY(Category = "Character Movement: Skating", EditDefaultsOnly) float RotationMaxDegreesSkating = 60.f;

    // transient
    UPROPERTY(Transient) ACoastalCharacter* CoastalCharacterOwner;

    // compressed flags
    bool Safe_bWantsToSprint;

    // standard remote procedure calls (rpc) flags
    UPROPERTY(Category = "Character Movement: Safe Movement Data", BlueprintReadOnly)
    bool Safe_bHasHitNormalCharacterEquipment;

    UPROPERTY(Category = "Character Movement: Safe Movement Data", BlueprintReadOnly)
    TEnumAsByte<EMovementMode> Safe_PreviousMovementMode;
    UPROPERTY(Category = "Character Movement: Safe Movement Data", BlueprintReadOnly)
    TEnumAsByte<ECustomMovementMode> Safe_PreviousCustomMovementMode;

    static const float BRAKE_TO_STOP_VELOCITY_SQUARED;
};
