#include <Windows.h>
#include <iostream>
#include "jni.h"
#include <iostream>
#include <string>
#include <thread>

HMODULE moduleHandle;

#define getObject(classname) env->FindClass(classname)

typedef jint(JNICALL* GetCreatedJavaVMs)(JavaVM**, jsize, jsize*);



void CreateConsole()
{
	AllocConsole();
	FILE *f;
	freopen_s(&f, "CONOUT$", "w", stderr);
}


void Inject()
{
	CreateConsole();
	std::cerr << "Injected" << '\n';
	HMODULE jvmHandle = GetModuleHandleA("jvm.dll");

	GetCreatedJavaVMs JNI_GetCreatedJavaVMs = (GetCreatedJavaVMs)GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs");

	JavaVM* jvm;
	JNIEnv* env;

	JNI_GetCreatedJavaVMs(&jvm, 1, NULL);
	jvm->AttachCurrentThread((void**)&env, NULL);
	std::cerr << "jvm: " << jvm << '\n';
	std::cerr << "env: " << env << '\n';

	jclass launch = env->FindClass("net/minecraft/launchwrapper/Launch");
	std::cerr << "Launch: " << launch << '\n';
	jfieldID sfid = env->GetStaticFieldID(launch, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");
	std::cerr << "sfid: " << sfid << std::endl;

	jobject classLoader = env->GetStaticObjectField(launch, sfid);

	std::cerr << "classLoader: " << classLoader << std::endl;
	jmethodID mid = env->GetMethodID(env->GetObjectClass(classLoader), "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	std::cerr << "mid: " << mid << std::endl;
	jstring name = env->NewStringUTF("net.minecraft.client.Minecraft");
	std::cerr << "name: " << name << std::endl;
	jclass mcClass = (jclass)env->CallObjectMethod(classLoader, mid, name);
	std::cerr << "mcClass: " << mcClass << std::endl;
	jmethodID smid = env->GetStaticMethodID(mcClass, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");
	std::cerr << "smid: " << smid << std::endl;
	jobject mc = env->CallStaticObjectMethod(mcClass, smid);
	std::cerr << "mc: " << mc << std::endl;
	mc = env->NewGlobalRef(mc);

	jfieldID thePlayerf = env->GetFieldID(env->GetObjectClass(mc), "field_71439_g", "Lnet/minecraft/client/entity/EntityClientPlayerMP;");
	std::cerr << "thePlayerf: " << thePlayerf << std::endl;
	jobject thePlayer = env->GetObjectField(mc, thePlayerf);
	std::cerr << "thePlayer: " << thePlayer << std::endl;

	
	


	//Set player rotation to 10
	//jclass CEntityPlayerSP = env->GetObjectClass(localPlayer);
	//env->SetFloatField(localPlayer, env->GetFieldID(CEntityPlayerSP, "bY", "F"), 10);
	//env->DeleteLocalRef(localPlayer);
	//env->DeleteLocalRef(CEntityPlayerSP);
	//while (true)
	//{
	//	if (GetAsyncKeyState(VK_F4) & 1)
	//	{
	//		break;
	//	}
	//}
	//std::cerr << "Uninjected" << '\n';
	//FreeConsole();
	//FreeLibraryAndExitThread(moduleHandle, 0);
}	



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		moduleHandle = hModule;
		//CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(Inject), NULL, NULL, NULL);
		Inject();

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}