// Fill out your copyright notice in the Description page of Project Settings.

	
#include "..//Libraries/RuntimeImport_Lib.h"


void URuntimeImport_Lib::LoadMeshFile(
	ERTIMeshType fileType,
	FString filePath, bool& success,
	FString& successMessage,
	TArray<FVector>& vertices,
	TArray<FVector>& normals,
	TArray<struct FRTIMeshStruct>& meshStructs,
	TArray<FVector2D>& UV0)
{
	FString dir;
	dir = FPaths::ConvertRelativePathToFull(filePath);


	switch (fileType)
	{
	case ERTIMeshType::E_obj:

		LoadOBJFile(
			dir,
			success,
			successMessage,
			vertices,
			normals,
			UV0,
			meshStructs);

		break;
	}

}



void URuntimeImport_Lib::LoadOBJFile(
	FString dir, bool& success, FString& successMessage,
	TArray<FVector>& vertices, TArray<FVector>& normals,
	TArray<FVector2D>& UV0, TArray<struct FRTIMeshStruct>& meshStructs)
{
	FString stringFileData;
	if (!FFileHelper::LoadFileToString(stringFileData, *dir)) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Can't load file: %s"), *dir);
		success = false;
		successMessage = "Can't load file: " + dir;
		return;
	}
	
	TArray<FString> lines;
	int32 lineCount = stringFileData.ParseIntoArray(lines, TEXT("\n"), true);
	stringFileData.Empty();

	if (lineCount < 1) {
		UE_LOG(LogTemp, Error, TEXT("RuntimeImport: corrupted file (1): %s"), *dir);
		success = false;
		successMessage = "Corrupted file (1): " + dir;
		return;
	}



	struct FRTIMaterialStruct lastMaterialData;

	TMap<FString, struct FRTIMaterialStruct> materialStructsTemp;


	//we need a default meshstruct
	FRTIMeshStruct defaultMesh;
	defaultMesh.meshName = "RunTimeImport_No_Name";
	defaultMesh.isDefaultStruct = true;



	meshStructs.Add(defaultMesh);

	int faceMethod = -1;

	TArray<FVector> verticesLines;
	TArray<FVector2D> UV0Lines;
	TArray<FVector> normalsLines;

	TArray<FString> lineElements;
	TArray<FString> subElements;
	TArray<int32> tempVerticesArrayPosition;


	for (int32 i = 0; i < lineCount; i++) {
		FString line = lines[i].TrimStartAndEnd();
		if (line.StartsWith("#")) {
			continue;
		}




		//objects
		if (line.StartsWith("o ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				if (meshStructs.Last().isDefaultStruct) {
					meshStructs.Last().meshName = line.Replace(TEXT("o "), TEXT(""));
					meshStructs.Last().isDefaultStruct = false;
					/*	if (lastMaterialData.isEmpty == false) {
							meshStructs.Last().hasMaterialData = true;
							meshStructs.Last().materialData = lastMaterialData;
						}*/

				}
				else {
					FRTIMeshStruct meshStruct;
					meshStruct.meshName = lineElements[1];
					/*if (lastMaterialData.isEmpty == false) {
						meshStruct.hasMaterialData = true;
						meshStruct.materialData = lastMaterialData;
					}*/
					meshStructs.Add(meshStruct);
				}
				faceMethod = -1;
			}
			continue;
		}

		if (line.StartsWith("g ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				if (meshStructs.Last().isDefaultStruct) {
					meshStructs.Last().meshName = line.Replace(TEXT("g "), TEXT(""));
					meshStructs.Last().isDefaultStruct = false;
					if (lastMaterialData.isEmpty == false) {
						meshStructs.Last().hasMaterialData = true;
						meshStructs.Last().materialData = lastMaterialData;
					}
				}
				else {
					FRTIMeshStruct meshStruct;
					meshStruct.meshName = lineElements[1];
					if (lastMaterialData.isEmpty == false) {
						meshStruct.hasMaterialData = true;
						meshStruct.materialData = lastMaterialData;
					}
					meshStructs.Add(meshStruct);
				}
				faceMethod = -1;
			}
			continue;
		}

		//mtl files
		if (line.StartsWith("mtllib ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 2) {
				//createMaterialStruct(dir.Replace(*FPaths::GetCleanFilename(dir), *lineElements[1]), materialStructsTemp);
			}
			continue;
		}

		//vertices
		if (line.StartsWith("v ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				verticesLines.Add(FVector(FCString::Atof(*lineElements[1]), FCString::Atof(*lineElements[2]), FCString::Atof(*lineElements[3])));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Vertices line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}

		//normals
		if (line.StartsWith("vn ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount == 4) {
				normalsLines.Add(FVector(FCString::Atof(*lineElements[1]), FCString::Atof(*lineElements[2]), FCString::Atof(*lineElements[3])));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Normals line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}

		//UV0
		if (line.StartsWith("vt ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 3) {
				float y = FCString::Atof(*lineElements[2]);
				if (y > 0) {
					UV0Lines.Add(FVector2D(FCString::Atof(*lineElements[1]), 1 - y));
				}
				else {
					UV0Lines.Add(FVector2D(FCString::Atof(*lineElements[1]), -1 - y));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) UV line corrupted: %s"), (i + 1), *line);
			}
			continue;
		}


		if (line.StartsWith("usemtl ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);
			if (elementCount >= 2) {
				if (materialStructsTemp.Find(lineElements[1]) != nullptr) {
					lastMaterialData = meshStructs.Last().materialData = *materialStructsTemp.Find(lineElements[1]);
				}
			}
			continue;
		}

		//faces, create the mesh structure
		if (line.StartsWith("f ")) {
			lineElements.Empty();
			int32 elementCount = line.ParseIntoArray(lineElements, TEXT(" "), true);

			//set face method
			if (faceMethod == -1 && elementCount > 3) {

				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Method search: %s"), *lineElements[1]);

				if (lineElements[1].Contains("/") == false) {
					faceMethod = 1;
				}
				else {
					if (lineElements[1].Contains("//")) {
						faceMethod = 4;
					}
					else {
						TArray<FString> tmp;
						if (lineElements[1].ParseIntoArray(tmp, TEXT("/"), true) == 2) {
							faceMethod = 2;
						}
						else {
							faceMethod = 3;
						}
					}
				}
				//UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: Method: %i"), faceMethod);
			}

			//create
			if (elementCount > 3) {

				//struct FRTIMeshStruct& tmpMeshStruct = meshStructs.Last();
				tempVerticesArrayPosition.Empty();

				for (int32 j = 1; j < lineElements.Num(); j++) {
					subElements.Empty();

					switch (faceMethod)
					{
					case 1:
						if (verticesLines.Num() > FCString::Atoi(*lineElements[j]) - 1) {
							vertices.Add(verticesLines[FCString::Atoi(*lineElements[j]) - 1]);
							tempVerticesArrayPosition.Add(vertices.Num() - 1);
						}
						break;
					case 2:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("/"), true);
						if (elementCount == 2) {
							if (verticesLines.Num() > FCString::Atoi(*subElements[0]) - 1) {
								vertices.Add(verticesLines[FCString::Atoi(*subElements[0]) - 1]);

								if (UV0Lines.Num() > FCString::Atoi(*subElements[1]) - 1)
									UV0.Add(UV0Lines[FCString::Atoi(*subElements[1]) - 1]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					case 3:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("/"), true);
						if (elementCount == 3) {
							if (verticesLines.Num() > FCString::Atoi(*subElements[0]) - 1) {
								vertices.Add(verticesLines[FCString::Atoi(*subElements[0]) - 1]);
								if (UV0Lines.Num() > FCString::Atoi(*subElements[1]) - 1)
									UV0.Add(UV0Lines[FCString::Atoi(*subElements[1]) - 1]);

								if (normalsLines.Num() > FCString::Atoi(*subElements[2]) - 1)
									normals.Add(normalsLines[FCString::Atoi(*subElements[2]) - 1]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					case 4:
						elementCount = lineElements[j].ParseIntoArray(subElements, TEXT("//"), true);
						if (elementCount == 2) {
							if (verticesLines.Num() > FCString::Atoi(*subElements[0]) - 1) {
								vertices.Add(verticesLines[FCString::Atoi(*subElements[0]) - 1]);

								if (normalsLines.Num() > FCString::Atoi(*subElements[1]) - 1)
									normals.Add(normalsLines[FCString::Atoi(*subElements[1]) - 1]);
								tempVerticesArrayPosition.Add(vertices.Num() - 1);
							}
						}
						break;
					}
				}

				struct FRTIMeshStruct& tmpMeshStruct = meshStructs.Last();

				//Create polygons in the UE4 way
				for (int32 k = 1; k < tempVerticesArrayPosition.Num() - 1; k++) {
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[k + 1]);
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[k]);
					tmpMeshStruct.triangles.Add(tempVerticesArrayPosition[0]);
				}
				tempVerticesArrayPosition.Empty();



			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin:(%i) Cannot process this line. Only polygons and quads are possible. (Method %i): %s"), (i + 1), faceMethod, *line);
			}
		}
	}

	if (vertices.Num() < 3) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: Less than 3 vertices found in file. Cancel: %s"), *dir);
		success = false;
		successMessage = "Less than 3 vertices found in file. Cancel: " + dir;
		return;
	}

	/*if (triangles.Num() < 3) {
		UE_LOG(LogTemp, Error, TEXT("RealTimeImportPlugin: No triangles found in file. Cancel: %s"), *dir);
		return;
	}*/

	if (normals.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("RealTimeImportPlugin: No normals found in this file. Normals will be automatically created. This can lead to problems with the lighting.: %s"), *dir);
	}

	UE_LOG(LogTemp, Display, TEXT("RealTimeImportPlugin: Vertices: %i UV0: %i Normals: %i "), vertices.Num(), UV0.Num(), normals.Num());

	success = true;
	successMessage = "Vertices: " + FString::FromInt(vertices.Num()) + " UV0: " + FString::FromInt(UV0.Num()) + " Normals: " + FString::FromInt(normals.Num());

	lineElements.Empty();
	subElements.Empty();
	tempVerticesArrayPosition.Empty();
}
