#include "ComputeShaderRunnerModule.h"

#include "Modules/ModuleManager.h"

#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "GlobalShader.h"

IMPLEMENT_MODULE(FComputeShaderRunnerModule, ComputeShaderRunner)

void FComputeShaderRunnerModule::StartupModule()
{
	const FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(FString("/CustomShaders"), ShaderDirectory);
}

void FComputeShaderRunnerModule::ShutdownModule()
{
	
}

