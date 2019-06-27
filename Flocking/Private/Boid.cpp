// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "Avoidable.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ABoid::ABoid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	//GM = (AFPS_GameMode*)GetWorld()->GetAuthGameMode();
	Velocity = FVector(FMath::FRandRange(0.0f, 1.0f) * 2 - 1, FMath::FRandRange(0.0f, 1.0f) * 2 - 1, FMath::FRandRange(0.0f, 1.0f) * 2 - 1);
	UGameplayStatics::GetAllActorsOfClass((UObject*)GetWorld(), ABoid::StaticClass(), Boids);
	if (IsGrounded) {
		FVector grounded_loc(GetActorLocation().X, GetActorLocation().Y, ZValue);
		SetActorLocation(grounded_loc);
	}
	time = TargetTimeAround;
	/*if (AM) {
		AM->OnAvoidablesChangeNum.Add()
	}*/
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	location = GetActorLocation();
	GetNeighbours();
	FVector acc = Flock(Neighbours, DeltaTime);
	FRotator dir = acc.ToOrientationRotator();
	FVector new_loc = location + acc * Speed * DeltaTime;
	if (IsGrounded) {
		new_loc = FVector(new_loc.X, new_loc.Y, ZValue);
	}
	SetActorLocation(new_loc);
	SetActorRotation(dir);
}

void ABoid::GetNeighbours() {
	Neighbours.Empty();
	for (AActor* a : Boids) {
		if (a != this) {
			float distance = (a->GetActorLocation() - location).Size();
			if (distance <= NeighboursRadius && distance > 0.0f) {
				Neighbours.Add(a);
			}
		}
	}
}

FVector ABoid::Flock(TArray<AActor*> Neighbours, float deltaTime) {

	FVector separation = Separate(Neighbours) * SeparationWeight;
	FVector alignement = Align(Neighbours) * AlignementWeight;
	FVector cohesion = Cohere(Neighbours) * CohesionWeight;
	FVector avoid = Avoid() * AvoidWeight;
	FVector target_dir = MoveToTarget(deltaTime);

	/*float delta_mul = GetDistanceMultiplier()*/;

	float mul = 1;
	if (IsStraying) {
		mul = -1;
	}
	if (!IsAvoiding) {
		Lerping = 0.0f;
	}
	else {
		Lerping += deltaTime * mul;
	}
	if (Lerping >= 1)
		Lerping = 1;
	if (Lerping <= 0)
		Lerping = 0;

	FVector moving_dir = separation + alignement + cohesion + target_dir;
	FVector avoiding_dir = avoid;
	FVector lerp_vector = FMath::Lerp(moving_dir, avoiding_dir, Lerping);
	return lerp_vector;
}

FVector ABoid::Separate(TArray<AActor*> Neighbours) {
	FVector sum = FVector::ZeroVector;
	int count = 0;
	float distance = 0;
	FVector boid_location;
	for (AActor* boid : Neighbours) {
		boid_location = boid->GetActorLocation();
		distance = (boid_location - location).Size();
		if (distance < SeparationRadius) {
			FVector loc_to_boid = location - boid_location;
			loc_to_boid.Normalize();
			sum += loc_to_boid / distance;
			count++;
		}
	}
	if (count > 0)
		return sum / count;
	return sum;
}

FVector ABoid::Align(TArray<AActor*> Neighbours) {
	FVector sum = FVector::ZeroVector;
	int count = 0;
	for (AActor* boid : Neighbours) {
		sum += boid->GetVelocity();
		count++;
	}
	if (count > 0)
		return sum / count;
	return sum;
}

FVector ABoid::Cohere(TArray<AActor*> Neighbours) {
	FVector sum = FVector::ZeroVector;
	int count = 0;
	for (AActor* boid : Neighbours) {
		sum += boid->GetActorLocation();
		count++;
	}
	if (count > 0)
		return SteerTo(sum / count);
	else return sum;
}

FVector ABoid::SteerTo(FVector Target) {
	FVector desired = Target - location;
	float distance = desired.Size();
	FVector steer = FVector::ZeroVector;
	if (distance > 0) {
		desired.Normalize();
		if (distance < 100)
			desired = desired * SteerSpeed * (distance / 100.0f);
		else
			desired = desired * SteerSpeed;
		steer = desired - Velocity;

	}
	return steer;
}

FVector ABoid::Avoid()
{
	FVector new_dir = FVector::ZeroVector;
	if (AM) {
		if (!IsAvoiding) {
			forward = GetActorForwardVector();
			right = GetActorRightVector();
			up = GetActorUpVector();
		}
		if (AM->Avoidables.Num() > 0) {
			float mul = 1;
			IsAvoiding = false;
			int count = 0;
			for (AActor* a : AM->Avoidables) {
				right = a->GetActorRightVector();
				FVector a_dir = a->GetActorLocation() - location;
				float distance = a_dir.Size();
				a_dir.Normalize();
				float a_angle = acos(FVector::DotProduct(a_dir, GetActorForwardVector()));
				FVector n_dir;
				if (distance > DistanceToAvoidableObjects) {
					FVector dir = location - a->GetActorLocation();
					n_dir = FVector(a->GetActorLocation().X, dir.Y, dir.Z);
					n_dir.Normalize();
					old_distance = FLT_MAX;
					IsStraying = false;
					/*casual_angle = FMath::FRandRange(0, 90);
					randomic = FVector::ZeroVector;*/
				}

				if (distance < DistanceToAvoidableObjects && a_angle < AvoidAngle) {
					//randomic = FVector::CrossProduct(a_dir, up) * sin(casual_angle);
					mul = 1 - distance / DistanceToAvoidableObjects;
					if (LeftTest(a, forward, up)) {

						new_dir -= right;
						count++;
					}
					else {
						new_dir += right;
						count++;
					}
					if (UpTest(a)) {
						new_dir += a->GetActorUpVector();
					}
					else {
						new_dir -= a->GetActorUpVector();
					}
					//new_dir += randomic;
					//new_dir.Normalize();
					IsStraying = IsObstacleStraying(a, old_distance);
					old_distance = (a->GetActorLocation() - location).Size();
					IsAvoiding = true;
				}
			}
			if (count > 0)
				return (new_dir / count) * mul;
			else return new_dir;
		}
		return new_dir;
	}
	return new_dir;
}

bool ABoid::LeftTest(AActor* obstacle, FVector forward, FVector up) {
	FVector dir = location - obstacle->GetActorLocation();
	FVector perp = FVector::CrossProduct(obstacle->GetActorForwardVector(), dir);
	float dot = FVector::DotProduct(perp, obstacle->GetActorUpVector());
	if (dot > 0.0f) {
		return false;
	}
	else if (dot <= 0.0f) {
		return true;
	}
	return false;
}

bool ABoid::UpTest(AActor* obstacle) {
	FVector dir = location - obstacle->GetActorLocation();
	dir.Normalize();
	FVector forward = obstacle->GetActorForwardVector();
	if (dir.Z > forward.Z) {
		return true;
	}
	return false;
}

bool ABoid::IsObstacleStraying(AActor* obs, float old_distance) {
	float distance = (obs->GetActorLocation() - location).Size();
	if (distance < old_distance) {
		return false;
	}
	return true;
}
FVector ABoid::MoveToTarget(float deltaTime) {
	if (Targets.Num() > 0) {
		if (timing) {
			time -= deltaTime;
		}
		FVector target_dir = Targets[target_index]->GetActorLocation() - location;
		float distance = target_dir.Size();
		target_dir.Normalize();
		if (distance <= TargetAvoidanceRadius && back_mul == 1) {
			timing = true;
			back_mul = -1;
			casual_angle = FMath::FRandRange(-MaxRandomicAngle, MaxRandomicAngle);
			randomic = FVector::CrossProduct(target_dir, GetActorUpVector()) * sin(casual_angle);
		}
		if (distance >= TargetAvoidanceRadius + AvoidanceDistanceFromTarget) {
			back_mul = 1;
			randomic = FVector::ZeroVector;
		}
		if (time <= 0) {
			back_mul = 1;
			target_index++;
			if (target_index >= Targets.Num())
				target_index = 0;
			timing = false;
			time = TargetTimeAround;
			OnTargetTimeExpire.Broadcast();
		}
		target_dir += randomic;
		target_dir *= back_mul;
		target_dir.Normalize();
		target_dir *= TargetWeight;
		return target_dir;
	}
	return FVector::ZeroVector;
}