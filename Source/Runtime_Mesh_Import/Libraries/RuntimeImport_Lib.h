// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RuntimeImport_Lib.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FRTIMaterialStruct
{
	GENERATED_USTRUCT_BODY()

		bool isEmpty = true;
	FString textureName;
	UTexture2D* textureDiffuse = nullptr;
	UTexture2D* textureAmbient = nullptr;
	UTexture2D* textureSpecular = nullptr;
	FColor ambient;
	FColor diffuse;
	FColor specular;
	float specularExponent;
	float dissolved;
	float dissolvedInverted;
};

USTRUCT(BlueprintType)
struct FRTIMeshStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	bool isDefaultStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	FString meshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	TArray<int32> triangles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	bool hasMaterialData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	struct FRTIMaterialStruct materialData;

};

UENUM(BlueprintType)
enum class ERTIDirectoryType : uint8
{
	E_gd	UMETA(DisplayName = "Game directory"),
	E_ad 	UMETA(DisplayName = "Absolute directory")
};

UENUM(BlueprintType)
enum class ERTIMeshType : uint8
{
	E_obj	UMETA(DisplayName = "OBJ")
};

UCLASS()
class RUNTIME_MESH_IMPORT_API URuntimeImport_Lib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "RuntimeImportLib|Mesh")
		static void LoadMeshFile
		(
			ERTIMeshType fileType,
			FString filePath,
			bool& success,
			FString& successMessage,
			TArray<FVector>& vertices,
			TArray<FVector>& normals,
			TArray<struct FRTIMeshStruct>& meshStructs,
			TArray<FVector2D>& UV
		);

	static void LoadOBJFile(
		FString dir,
		bool& success,
		FString& successMessage,
		TArray<FVector>& vertices,
		TArray<FVector>& normals,
		TArray<FVector2D>& UV0,
		TArray<struct FRTIMeshStruct>& meshStructs);



	//UFUNCTION(BlueprintCallable, Category = "MeshLoader")
	//	static void LoadMesh();
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RealTimeImport")
	//	static URuntimeImport_Lib* getRealTimeImportTarget();


};

/*
UENUM(BlueprintType)
enum class EPathType : uint8
{
	Absolute,
	Relative
};


USTRUCT(BlueprintType)
struct FMeshInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FVector> Vertices;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<int32> Triangles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FVector2D> UV0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FLinearColor> VertexColors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		FTransform RelativeTransform;
};

USTRUCT(BlueprintType)
struct FReturnedData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		bool bSuccess;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		int32 NumMeshes;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReturnedData")
		TArray<FMeshInfo> meshInfo;


};
*/