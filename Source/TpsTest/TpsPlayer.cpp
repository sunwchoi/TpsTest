// Fill out your copyright notice in the Description page of Project Settings.


#include "TpsPlayer.h"

#include "Enemy.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATpsPlayer::ATpsPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("ArmComp"));
	ArmComp->SetupAttachment(GetMesh());

	CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
	CamComp->SetupAttachment(ArmComp);

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GetMesh());

}

// Called when the game starts or when spawned
void ATpsPlayer::BeginPlay()
{
	Super::BeginPlay();

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* sys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (sys)
			sys->AddMappingContext(IMC_Action, 0);
	}
}

// Called every frame
void ATpsPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATpsPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* ip = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	ip->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATpsPlayer::OnMove);
	ip->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATpsPlayer::OnLook);
	ip->BindAction(IA_Fire, ETriggerEvent::Started, this, &ATpsPlayer::OnFire);

}

void ATpsPlayer::OnMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	FRotator rot = FRotator(0, GetControlRotation().Yaw, 0);

	FVector x = UKismetMathLibrary::GetForwardVector(rot);
	FVector y = UKismetMathLibrary::GetRightVector(rot);
	
	AddMovementInput(x, v.X);
	AddMovementInput(y, v.Y);
}

void ATpsPlayer::OnLook(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	AddControllerPitchInput(v.Y);	
	AddControllerYawInput(v.X);
}

void ATpsPlayer::OnFire(const FInputActionValue& Value)
{
	FVector start = CamComp->GetComponentLocation();
	FVector end = start + CamComp->GetForwardVector() * 100000.f;
	FHitResult res;

	bool bHit = GetWorld()->LineTraceSingleByChannel(res, start, end, ECC_WorldStatic);
	if (bHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, res.ImpactPoint);
		auto* hitComp = res.GetComponent();
		if (hitComp->IsSimulatingPhysics())
		{
			FVector dir = res.ImpactPoint - GetActorLocation();
			hitComp->AddForce(dir * 100000.f);
		}
		auto enemy = Cast<AEnemy>(res.GetActor());
		if (enemy)
			enemy->TakeMyDamage();
	}
}

