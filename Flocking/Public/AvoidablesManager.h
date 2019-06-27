// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AvoidablesManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvoidablesDelegate);

UCLASS(Blueprintable)
class AAvoidablesManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAvoidablesManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
		FAvoidablesDelegate OnAvoidablesChangeNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
		TArray<AActor*> Avoidables;

	void ModifyAvoidablesArray();
};
