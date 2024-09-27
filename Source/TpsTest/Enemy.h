// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EState : uint8
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};

UCLASS()
class TPSTEST_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EState State = EState::IDLE;
	float HP = 3;
	class ATpsPlayer* Target;

	void TakeMyDamage();
	FTimerHandle damageHandle;
	
	void TickIdle();
	void TickMove();
	void TickAttack();
	FTimerHandle attackHandle;
	void TickDamage();
	void TickDie();
	FTimerHandle dieHandle;
};
