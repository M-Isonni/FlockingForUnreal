// Fill out your copyright notice in the Description page of Project Settings.


#include "AvoidablesManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Avoidable.h"

// Sets default values
AAvoidablesManager::AAvoidablesManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAvoidablesManager::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AAvoidablesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAvoidablesManager::ModifyAvoidablesArray() {
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAvoidable::StaticClass(), Avoidables);
}

