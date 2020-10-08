#include <Windows.h>
#include <iostream>
#include "jni.h"
#include <iostream>
#include <string>
#include <thread>



#define getObject(classname) env->FindClass(classname)

typedef jint(JNICALL* GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);



void CreateConsole()
{
	AllocConsole();
	FILE *f;
	freopen_s(&f, "CONOUT$", "w", stdout);
}


void Inject()
{
	CreateConsole();
	std::cout << "Injected" << '\n';
	HMODULE jvmHandle = GetModuleHandleA("jvm.dll");

	GetCreatedJavaVMs JNI_GetCreatedJavaVMs = (GetCreatedJavaVMs)GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs");

	JavaVM* jvm;
	JNIEnv* env;

	JNI_GetCreatedJavaVMs(&jvm, 1, NULL);
	jvm->AttachCurrentThread((void**)&env, NULL);
	std::cout << "jvm: " << jvm << '\n';
	std::cout << "env: " << env << '\n';

	//Get Minecraft
	jclass MinecraftClass = env->FindClass("bao");
	std::cout << "MinecraftClass: " << MinecraftClass << '\n';
	jmethodID GetMinecraft = env->GetStaticMethodID(MinecraftClass, "func_71410_x", "()Lbao;");
	std::cout << "GetMinecraft: " << GetMinecraft << '\n';
	jobject Minecraft = env->CallStaticObjectMethod(MinecraftClass, GetMinecraft);
	std::cout << "Minecraft: " << Minecraft << '\n';


	//Get Player
	//jobject localPlayer = env->GetObjectField(Minecraft, env->GetFieldID(MinecraftClass, "s", "Ldze;"));
	env->DeleteLocalRef(MinecraftClass);
	env->DeleteLocalRef(Minecraft);

	//Set player rotation to 10
	//jclass CEntityPlayerSP = env->GetObjectClass(localPlayer);
	//env->SetFloatField(localPlayer, env->GetFieldID(CEntityPlayerSP, "bY", "F"), 10);
	//env->DeleteLocalRef(localPlayer);
	//env->DeleteLocalRef(CEntityPlayerSP);

}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		Inject();


	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}