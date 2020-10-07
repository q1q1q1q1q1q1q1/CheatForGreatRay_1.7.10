#include <Windows.h>
#include <iostream>
#include "jni.h"
#include <iostream>
#include <string>
#include <thread>



HMODULE DllHandle;
typedef jint(*hJNI_GetCreatedJavaVMs)(JavaVM** vmBuf, jsize bufLen, jsize* nVMs);

hJNI_GetCreatedJavaVMs oJNI_GetCreatedJavaVMs;

HMODULE jvmHandle;
FARPROC func_JNI_GetCreatedJavaVMs;
JavaVM *jvm;
JNIEnv *jenv;
jobject mc;
jobject thePlayer;
jobject theWorld;



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
	jvmHandle = GetModuleHandleA("jvm.dll");
	func_JNI_GetCreatedJavaVMs = GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs");
	oJNI_GetCreatedJavaVMs = (hJNI_GetCreatedJavaVMs)func_JNI_GetCreatedJavaVMs;
	jint returnOF = oJNI_GetCreatedJavaVMs(&jvm, 1, NULL);
	jint returnOf1 = jvm->AttachCurrentThread((void **)&jenv, NULL);
	std::cout << "returnOf: 1" << returnOf1 << '\n';
	jstring name = jenv->NewStringUTF("net.minecraft.client.Minecraft");
	std::cout << "name: " << name << '\n';
	jclass launch = jenv->FindClass("net/minecraft/launchwrapper/Launch");
	std::cout << "Launch: " << launch << std::endl;
	jfieldID sfid = jenv->GetStaticFieldID(launch, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");
	std::cout << "sfid: " << sfid << std::endl;
	jobject classLoader = jenv->GetStaticObjectField(launch, sfid);
	std::cout << "classLoader: " << classLoader << std::endl;
	//jmethodID mid = jenv->GetMethodID(jenv->GetObjectClass(classLoader), "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	//std::cout << "mid: " << mid << std::endl;
	//jclass mcClass = (jclass)jenv->CallObjectMethod(classLoader, mid, name);
	//std::cout << "MCClass: " << mcClass << std::endl;
	//jmethodID smid = jenv->GetStaticMethodID(mcClass, "getMinecraft", "(Lnet/minecraft/client/Minecraft;)Ljava/lang/String;");
	//std::cout << "SMID: " << smid << std::endl;
	jenv->DeleteLocalRef(name);
	//mc = jenv->CallStaticObjectMethod(mcClass, smid);
	//std::cout << "mc: " << mc << '\n';

}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllHandle = hModule;
		Inject();


	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}