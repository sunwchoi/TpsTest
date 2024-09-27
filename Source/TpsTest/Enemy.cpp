// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "TpsPlayer.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Target = Cast<ATpsPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(State));
	DrawDebugString(GetWorld(), GetActorLocation(), *UEnum::GetValueAsString(State), nullptr, FColor::Red, 0);
	switch (State)
	{
	case EState::IDLE:
		TickIdle();
		break;
	case EState::MOVE:
		TickMove();
		break;
	case EState::ATTACK:
		TickAttack();
		break;
	case EState::DAMAGE:
		TickDamage();
		break;
	case EState::DIE:
		TickDie();
		break;
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TakeMyDamage()
{
	if (--HP == 0)
		State = EState::DIE;
	else
		State = EState::DAMAGE;
	GetWorld()->GetTimerManager().SetTimer(damageHandle, [this]()
	{
		State = EState::MOVE;
	}, 1.f, false);		
}

void AEnemy::TickIdle()
{
	if (FVector::Dist(Target->GetActorLocation(), GetActorLocation()) <= 1000.f)
	{
		State = EState::MOVE;
	}
}

void AEnemy::TickMove()
{
	FVector dir = Target->GetActorLocation() - GetActorLocation();
	dir.Normalize();

	SetActorLocation(GetActorLocation() + dir * GetWorld()->GetDeltaSeconds() * 600.f);
	if (FVector::Dist(Target->GetActorLocation(), GetActorLocation()) <= 200.f)
		State = EState::ATTACK;
}

void AEnemy::TickAttack()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(attackHandle))
		return ;
	GetWorld()->GetTimerManager().SetTimer(attackHandle, [this]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack!"));
		if (FVector::Dist(Target->GetActorLocation(), GetActorLocation()) > 200.f)
			State = EState::MOVE;
	}, 1.f, false);
}

void AEnemy::TickDamage()
{
}

void AEnemy::TickDie()
{
	SetActorEnableCollision(false);
	SetActorLocation(GetActorLocation() + FVector::DownVector * 100.f * GetWorld()->GetDeltaSeconds());
	if (GetWorld()->GetTimerManager().IsTimerActive(dieHandle))
		return ;
	GetWorld()->GetTimerManager().SetTimer(dieHandle, [this]() { Destroy();}, 1.f, false);
}
