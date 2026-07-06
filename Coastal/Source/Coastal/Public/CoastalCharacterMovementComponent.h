// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoastalCharacterMovementComponent.generated.h"

class ACoastalCharacter;

UENUM(BlueprintType)

enum ECustomMovementMode
{
    CMOVE_None UMETA(Hidden),
    CMOVE_Skate UMETA(DisplayName = "Skate"),
    CMOVE_MAX UMETA(Hidden),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class COASTAL_API UCoastalCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

    class FSavedMove_Coastal : public FSavedMove_Character
    {
    public:
        uint8 Saved_bWantsToSprint : 1u;

        // dictate whether new move is the same and does not need to be sent separately
        virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                    float MaxDelta) const override;
        virtual void Clear() override;
        virtual uint8 GetCompressedFlags() const override;
        virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                FNetworkPredictionData_Client_Character& ClientData) override;
        virtual void PrepMoveFor(ACharacter* C) override;
    };

    class FNetworkPredictionData_Client_Coastal : public FNetworkPredictionData_Client_Character
    {
    public:
        FNetworkPredictionData_Client_Coastal(const UCharacterMovementComponent& ClientMovement);

        virtual FSavedMovePtr AllocateNewMove() override;
    };

    // parameters
    UPROPERTY(EditDefaultsOnly) float MaxSpeed_Walk = 600.f;
    UPROPERTY(EditDefaultsOnly) float MaxSpeed_Sprint = 1000.f;

    UPROPERTY(EditDefaultsOnly) float MinSpeed_Skate = 900.f;
    UPROPERTY(EditDefaultsOnly) float EnterImpulse_Skate = 400.f;
    UPROPERTY(EditDefaultsOnly) float GravityForce_Skate = 4000.f;
    UPROPERTY(EditDefaultsOnly) float Friction_Skate = 1.3;

    // transient
    UPROPERTY(Transient) ACoastalCharacter* CoastalCharacterOwner;

    // flags
    bool Safe_bWantsToSprint;

public:
    UCoastalCharacterMovementComponent();

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void InitializeComponent() override;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
    UFUNCTION(BlueprintCallable)
    void SprintPressed();
    UFUNCTION(BlueprintCallable)
    void SprintReleased();

    UFUNCTION(BlueprintCallable)
    void CrouchPressed();

    void EnterSkate(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
    void ExitSkate();
    bool CanSkate() const;
    void PhysSkate(float deltaTime, int32 Iterations);
    bool GetHitResultCharacter(FHitResult& HitResult) const;
    bool GetHitResultCharacterEquipment(FVector& HitNormal) const;
};
