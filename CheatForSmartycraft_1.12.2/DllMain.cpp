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

jfieldID pointedEntityF;


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

jobject getPointedEntity(JNIEnv* env)
{
	pointedEntityF = env->GetFieldID(env->GetObjectClass(mc), "field_147125_j", "Lnet/minecraft/entity/Entity;");
	std::cout << "pointedEntityF: " << pointedEntityF << std::endl;
	return env->GetObjectField(mc, thePlayerf);
}

void sendChatMessage(JNIEnv* env, const char* message)
{
	jmethodID chatFuncID = env->GetMethodID(env->GetObjectClass(thePlayer), "func_71165_d", "(Ljava/lang/String;)V");
	std::cout << "chatFuncID: " << chatFuncID << std::endl;

	env->CallObjectMethod(thePlayer, chatFuncID, env->NewStringUTF(message));
}

void setSprinting(JNIEnv* env, bool sprinting)
{
	jclass EntityClass = env->GetObjectClass(thePlayer);
	jmethodID sprintFuncID = env->GetMethodID(EntityClass, "func_70031_b", "(Z)V");
	env->CallObjectMethod(pointedEntity, sprintFuncID, sprinting);
}

BOOL IsSprinting(JNIEnv* env)
{
	jclass EntityClass = env->GetObjectClass(thePlayer);
	jmethodID sprintFuncID = env->GetMethodID(EntityClass, "func_70051_ag", "()Z");
	return env->CallBooleanMethod(pointedEntity, sprintFuncID);
}

void setStepHeight(JNIEnv* env, float stepHeight)// GetFieldID return NULL
{
	jclass EntityClass = env->GetObjectClass(thePlayer);
	jfieldID stepHeightID = env->GetFieldID(EntityClass, "field_70138_W", "W");
	std::cout << "stepHeightID: " << stepHeightID << std::endl;
	env->SetFloatField(pointedEntity, stepHeightID, stepHeight);
}

void postPreInit(JNIEnv* env) 
{
	mc = getMC(env);
	mc = env->NewGlobalRef(mc);
	thePlayer = getPlayer(env);
	thePlayer = env->NewGlobalRef(thePlayer);
	pointedEntity = getPointedEntity(env);
	pointedEntity = env->NewGlobalRef(pointedEntity);
	//setStepHeight(env, 10);

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
			postPreInit(jenv);

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