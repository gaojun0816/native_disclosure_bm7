#include <jni.h>
#include <string>
#include <deque>

extern "C" JNIEXPORT jstring JNICALL
Java_lu_uni_jungao_bm7_NativeDelegator_sStringFromJNI(
        JNIEnv *env,
        jclass /* cls */) {
    std::string hello = "Hello from C++ static method";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_lu_uni_jungao_bm7_NativeDelegator_iStringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ instance method";
    return env->NewStringUTF(hello.c_str());
}

jclass findClass(JNIEnv *env, const char *clsName) {
    return env->FindClass(clsName);
}

void invokeSendSMS(JNIEnv *env, jclass cls, const char *methodName, const char *sig, jstring msg) {
    jmethodID sendPSMS = env->GetStaticMethodID(cls, methodName, sig);
    env->CallStaticVoidMethod(cls, sendPSMS, msg);
}

void sendSms(JNIEnv *env, jobject /*this*/, jstring msg) {
    jclass MiddleCls = findClass(env, "lu/uni/jungao/bm7/MiddleCls");
    invokeSendSMS(env, MiddleCls, "sendPseudoSMS", "(Ljava/lang/String;)V", msg);
}

jmethodID getImeiMethodID(JNIEnv *env, jobject tm, const char *methodName, const char *sig) {
    jclass cls = env->GetObjectClass(tm);
    jmethodID getImei = env->GetMethodID(cls, methodName, sig);
    return getImei;
}

jstring getImei(JNIEnv *env, jobject /*this*/, jobject teleManager) {
    jmethodID getImei = getImeiMethodID(env, teleManager, "getImei", "()Ljava/lang/String;");
    jstring imei = (jstring) env->CallObjectMethod(teleManager, getImei);
    return imei;
}

int registerNatives(JNIEnv *env, jclass cls, std::initializer_list<JNINativeMethod>methods, jint size) {
    return env->RegisterNatives(cls, methods.begin(), size);
}

int middleFunc(JNIEnv *env, jclass cls, std::initializer_list<JNINativeMethod> methods) {
    return registerNatives(env, cls, methods, methods.size());
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    // Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
    jclass c = findClass(env, "lu/uni/jungao/bm7/NativeDelegator");
    if (c == nullptr) return JNI_ERR;

    // Register your class' native methods.
    int rc = middleFunc(env, c, {
            {"nativeGetIMEI", "(Landroid/telephony/TelephonyManager;)Ljava/lang/String;", reinterpret_cast<jstring*>(getImei)},
            {"nativeSendSMS", "(Ljava/lang/String;)V", reinterpret_cast<void*>(sendSms)}
    });
    if (rc != JNI_OK) return rc;

    return JNI_VERSION_1_6;
}




