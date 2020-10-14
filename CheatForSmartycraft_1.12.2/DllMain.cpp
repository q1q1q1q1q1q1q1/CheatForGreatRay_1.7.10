#include <Windows.h>
#include <iostream>
#include "jni.h"
#include <iostream>
#include <string>
#include <thread>

HMODULE moduleHandle;

typedef jint(*hJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs);

hJNI_GetCreatedJavaVMs oJNI_GetCreatedJavaVMs;

HMODULE jvmHandle;
FARPROC func_JNI_GetCreatedJavaVMs;
JavaVM *jvm;
JNIEnv *jenv;

jobject entityObject;
jclass Entity;

void CreateConsole()
{
	AllocConsole();
	FILE *f;
	freopen_s(&f, "CONOUT$", "w", stdout);
}

jobject getClassLoader(JNIEnv* env)
{
	jclass launch = env->FindClass("net/minecraft/launchwrapper/Launch");
	std::cout << "launch: " << launch << std::endl;
	jfieldID sfid = env->GetStaticFieldID(launch, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");
	std::cout << "sfid: " << sfid << std::endl;
	jobject classLoader = env->GetStaticObjectField(launch, sfid);
	std::cout << "classLoader: " << classLoader << std::endl;
	return classLoader;
}


jclass getObject(JNIEnv* env, const char* className)
{
	jstring name = env->NewStringUTF(className);
	jobject classLoader = getClassLoader(env);
	jmethodID mid = env->GetMethodID(env->GetObjectClass(classLoader), "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	std::cout << "mid: " << mid << std::endl;

	return (jclass)env->CallObjectMethod(classLoader, mid, name);
	env->DeleteLocalRef(name);
}


void postPreInit(JNIEnv* env) 
{
	jclass MinecraftClass = getObject(env, "net.minecraft.client.Minecraft");
	std::cout << "MinecraftClass: " << MinecraftClass << std::endl;

	jmethodID GetMinecraft = env->GetStaticMethodID(MinecraftClass, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");
	std::cout << "GetMinecraft: " << GetMinecraft << std::endl;

	jobject Minecraft = env->CallStaticObjectMethod(MinecraftClass, GetMinecraft);
	std::cout << "Minecraft: " << Minecraft << std::endl;

	jobject pointedEntity = env->GetObjectField(Minecraft, env->GetFieldID(env->GetObjectClass(Minecraft), "field_147125_j", "j"));
	std::cout << "pointedEntity: " << pointedEntity << std::endl;
	


	//jclass entityClass = env->GetObjectClass(pointedEntity);
	//std::cout << "entityClass: " << entityClass << std::endl;

	//jmethodID setSprinting = env->GetMethodID(env->GetObjectClass(pointedEntity), "func_70031_b", "(Z)V");
	//std::cout << "setSprinting: " << setSprinting << std::endl;


}


DWORD WINAPI Inject(LPVOID lpParam)
{
	CreateConsole();
	std::cout << "Injected" << '\n';
	
	jvmHandle = GetModuleHandleA("jvm.dll");
	func_JNI_GetCreatedJavaVMs = GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs");
	oJNI_GetCreatedJavaVMs = (hJNI_GetCreatedJavaVMs)func_JNI_GetCreatedJavaVMs;
	jint returnOF = oJNI_GetCreatedJavaVMs(&jvm, 1, NULL);
	jint returnOf1 = jvm->AttachCurrentThread((void **)&jenv, NULL);
	




	while (true) {
		if (GetAsyncKeyState(VK_F4) & 1)
		{
			if (jenv != nullptr) {
				postPreInit(jenv);
			}
			else {
				std::cout << "jenv NULL" << std::endl;
			}

		}
		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			break;
		}
	}
	std::cout << "Uninjected" << '\n';
	FreeConsole();
	FreeLibraryAndExitThread(moduleHandle, 0);
}	


void init() {
	CreateThread(NULL, NULL, &Inject, NULL, NULL, NULL);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		moduleHandle = hModule;
		init();

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}