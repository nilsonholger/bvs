

#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "BvsA.h"
 #include <opencv2/core/core.hpp>
 
static JavaVM *gJavaVM;
static jobject gInterfaceObject;	

void shutdownFunction()
{

	LOG(1,"daemon exit caused by bvs shutdown request!");

	//bvs->quit();

	alarm(1); // SIGALRM after 1 second so all framework/module threads have a chance to quit

}

cv::Mat* getNativeAddress()
{
	return javaMat;
}



void drawToAndroid()
{
/*
	if(!javaMat)
	{
		LOG(0,"ExampleCV no javaMat ADDR");
		javaMat=getNativeAddress();
	
	}
	*javaMat=img;
	
	*/
		LOG(0,"JNI drawToAndroid");
		
		int status;
		JNIEnv *env;
		bool isAttached =false;

		status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_6);
		LOG(0,"STATUS: " << status);


		if(status < 0)
		{
			LOG(1, "callback_handler: failed to get JNI environment, assuming native thread");
			status = gJavaVM->AttachCurrentThread(&env, NULL);
			if(status <0)
			{
				LOG(1, "callback_handler: failed to attach current thread");
				return;
			}
		}

		jclass javaClass = env-> GetObjectClass(javaObj);
		if (javaClass == 0) 
		{
			LOG(0,"FindClass error");
			return;
		}
		
		LOG(0,"Setting methodID");
		//Method takes a cv::Mat and returns nothing, void
		//jmethodID javaMethodID= env->GetMethodID(javaClass, "drawToDisplay", "(Lorg/opencv/core/Mat;)V");
		//jmethodID javaMethodID= env->GetMethodID(javaClass, "drawToDisplay", "([Ljava/lang/Integer;)V");
		jmethodID javaMethodID= env->GetMethodID(javaClass, "drawToDisplay", "()V");
		if (javaMethodID == 0) 
		{
			LOG(0,"GetMethodID error");
			return;
		}
	LOG(0,"methodID set");
		isAttached=true;

	
		env->CallVoidMethod(javaObj, javaMethodID);
		
		if(isAttached)
		{
			gJavaVM->DetachCurrentThread();
		}

}
	
extern "C" 
{

	jint JNI_OnLoad(JavaVM* vm, void* reserved)
	{
		gJavaVM=vm;
		JNIEnv *env;
		if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
			return -1;

		LOG(0,"JNI INIT");

		return JNI_VERSION_1_6;
	}





	JNIEXPORT void JNICALL Java_com_cvhci_bvsa_MainActivity_bvsAndroid(JNIEnv *env,	jobject  obj, jstring configFilePath,  jlong addrMat)
	{
		LOGD("Java_com_cvhci_bvsa_MainActivity_bvsAndroid enter");
		javaMat = (cv::Mat*)addrMat;
		LOG(0,"JNI bvsAndroid Mat Adress " << javaMat);
		javaObj = env->NewGlobalRef(obj);
	
		const char* configStrChars = env->GetStringUTFChars(configFilePath, (jboolean *)0);

		LOGD(configStrChars);

		char* bvsconfig[] ={"v",const_cast<char*>(configStrChars)};

		bvs = new BVS::BVS(2,bvsconfig);
		bvs->loadModules();
		LOGD("connecting modules!");
		bvs->connectAllModules();
		LOG(2, "starting!");
		bvs->start();

		bvs->run();





	}

}



