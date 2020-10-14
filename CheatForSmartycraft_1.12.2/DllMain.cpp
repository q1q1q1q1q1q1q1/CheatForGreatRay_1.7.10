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

jobject mc;
jobject thePlayer;
jobject theWorld;
jobject renderViewEntity;
jobject pointedEntity;
jobject boundingBox;
jobject hitVec;
jobject ridingEntity;

jdouble xCoord;
jdouble yCoord;
jdouble zCoord;

jfieldID thePlayerf;

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

jobject getMC(JNIEnv* env)
{
	jclass mcClass = getObject(env, "net.minecraft.client.Minecraft");
	std::cout << "MCClass: " << mcClass << std::endl;
	jmethodID smid = env->GetStaticMethodID(mcClass, "func_71410_x", "()Lnet/minecraft/client/Minecraft;");
	std::cout << "SMID: " << smid << std::endl;
	return env->CallStaticObjectMethod(mcClass, smid);
}

jobject getPlayer(JNIEnv* env)
{
	thePlayerf = env->GetFieldID(env->GetObjectClass(mc), "field_71439_g", "Lnet/minecraft/client/entity/EntityClientPlayerMP;");
	return env->GetObjectField(mc, thePlayerf);
}


void sendChatMessage(JNIEnv* env, char* message)
{
	jmethodID chatFuncID = env->GetMethodID(env->GetObjectClass(thePlayer), "func_71165_d", "(Ljava/lang/String;)V");
	env->CallObjectMethod(thePlayer, chatFuncID, env->NewStringUTF(message));
}


void postPreInit(JNIEnv* env) 
{
	mc = getMC(env);
	mc = env->NewGlobalRef(mc);
	thePlayer = getPlayer(env);
	thePlayer = env->NewGlobalRef(thePlayer);

	
	
	

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