#include "jni.h"

void _lfi_jni_NewObject(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    (*env)->NewObjectV(env, clazz, methodID, args);
    va_end(args);
}

#define DEFINE_CallMethod(typename, jtype) \
    jtype _lfi_jni_Call##typename##Method(JNIEnv* env, jobject obj, jmethodID methodID, ...) { \
        va_list args; \
        va_start(args, methodID); \
        jtype ret = (*env)->Call##typename##MethodV(env, obj, methodID, args); \
        va_end(args); \
        return ret; \
    }

void _lfi_jni_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    (*env)->CallVoidMethodV(env, obj, methodID, args);
    va_end(args);
}

#define DEFINE_CallNonvirtualMethod(typename, jtype) \
    jtype _lfi_jni_CallNonvirtual##typename##Method(JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...) { \
        va_list args; \
        va_start(args, methodID); \
        jtype ret = (*env)->CallNonvirtual##typename##MethodV(env, obj, clazz, methodID, args); \
        va_end(args); \
        return ret; \
    }

void _lfi_jni_CallNonvirtualVoidMethod(JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    (*env)->CallNonvirtualVoidMethodV(env, obj, clazz, methodID, args);
    va_end(args);
}

#define DEFINE_CallStaticMethod(typename, jtype) \
    jtype _lfi_jni_CallStatic##typename##Method(JNIEnv* env, jclass clazz, jmethodID methodID, ...) { \
        va_list args; \
        va_start(args, methodID); \
        jtype ret = (*env)->CallStatic##typename##MethodV(env, clazz, methodID, args); \
        va_end(args); \
        return ret; \
    }

void _lfi_jni_CallStaticVoidMethod(JNIEnv* env, jclass clazz, jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    (*env)->CallStaticVoidMethodV(env, clazz, methodID, args);
    va_end(args);
}

DEFINE_CallMethod(Object, jobject)
DEFINE_CallMethod(Boolean, jboolean)
DEFINE_CallMethod(Byte, jbyte)
DEFINE_CallMethod(Char, jchar)
DEFINE_CallMethod(Short, jshort)
DEFINE_CallMethod(Int, jint)
DEFINE_CallMethod(Long, jlong)
DEFINE_CallMethod(Float, jfloat)
DEFINE_CallMethod(Double, jdouble)

DEFINE_CallNonvirtualMethod(Object, jobject)
DEFINE_CallNonvirtualMethod(Boolean, jboolean)
DEFINE_CallNonvirtualMethod(Byte, jbyte)
DEFINE_CallNonvirtualMethod(Char, jchar)
DEFINE_CallNonvirtualMethod(Short, jshort)
DEFINE_CallNonvirtualMethod(Int, jint)
DEFINE_CallNonvirtualMethod(Long, jlong)
DEFINE_CallNonvirtualMethod(Float, jfloat)
DEFINE_CallNonvirtualMethod(Double, jdouble)

DEFINE_CallStaticMethod(Object, jobject)
DEFINE_CallStaticMethod(Boolean, jboolean)
DEFINE_CallStaticMethod(Byte, jbyte)
DEFINE_CallStaticMethod(Char, jchar)
DEFINE_CallStaticMethod(Short, jshort)
DEFINE_CallStaticMethod(Int, jint)
DEFINE_CallStaticMethod(Long, jlong)
DEFINE_CallStaticMethod(Float, jfloat)
DEFINE_CallStaticMethod(Double, jdouble)
