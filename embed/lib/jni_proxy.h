#pragma once

struct JNIProxy {
    void* res0;
    void* res1;
    void* res2;

    void* res3;

    void* jni_GetVersion;

    void* jni_DefineClass;
    void* jni_FindClass;

    void* jni_FromReflectedMethod;
    void* jni_FromReflectedField;

    void* jni_ToReflectedMethod;

    void* jni_GetSuperclass;
    void* jni_IsAssignableFrom;

    void* jni_ToReflectedField;

    void* jni_Throw;
    void* jni_ThrowNew;
    void* jni_ExceptionOccurred;
    void* jni_ExceptionDescribe;
    void* jni_ExceptionClear;
    void* jni_FatalError;

    void* jni_PushLocalFrame;
    void* jni_PopLocalFrame;

    void* jni_NewGlobalRef;
    void* jni_DeleteGlobalRef;
    void* jni_DeleteLocalRef;
    void* jni_IsSameObject;

    void* jni_NewLocalRef;
    void* jni_EnsureLocalCapacity;

    void* jni_AllocObject;
    void* jni_NewObject;
    void* jni_NewObjectV;
    void* jni_NewObjectA;

    void* jni_GetObjectClass;
    void* jni_IsInstanceOf;

    void* jni_GetMethodID;

    void* jni_CallObjectMethod;
    void* jni_CallObjectMethodV;
    void* jni_CallObjectMethodA;
    void* jni_CallBooleanMethod;
    void* jni_CallBooleanMethodV;
    void* jni_CallBooleanMethodA;
    void* jni_CallByteMethod;
    void* jni_CallByteMethodV;
    void* jni_CallByteMethodA;
    void* jni_CallCharMethod;
    void* jni_CallCharMethodV;
    void* jni_CallCharMethodA;
    void* jni_CallShortMethod;
    void* jni_CallShortMethodV;
    void* jni_CallShortMethodA;
    void* jni_CallIntMethod;
    void* jni_CallIntMethodV;
    void* jni_CallIntMethodA;
    void* jni_CallLongMethod;
    void* jni_CallLongMethodV;
    void* jni_CallLongMethodA;
    void* jni_CallFloatMethod;
    void* jni_CallFloatMethodV;
    void* jni_CallFloatMethodA;
    void* jni_CallDoubleMethod;
    void* jni_CallDoubleMethodV;
    void* jni_CallDoubleMethodA;
    void* jni_CallVoidMethod;
    void* jni_CallVoidMethodV;
    void* jni_CallVoidMethodA;

    void* jni_CallNonvirtualObjectMethod;
    void* jni_CallNonvirtualObjectMethodV;
    void* jni_CallNonvirtualObjectMethodA;
    void* jni_CallNonvirtualBooleanMethod;
    void* jni_CallNonvirtualBooleanMethodV;
    void* jni_CallNonvirtualBooleanMethodA;
    void* jni_CallNonvirtualByteMethod;
    void* jni_CallNonvirtualByteMethodV;
    void* jni_CallNonvirtualByteMethodA;
    void* jni_CallNonvirtualCharMethod;
    void* jni_CallNonvirtualCharMethodV;
    void* jni_CallNonvirtualCharMethodA;
    void* jni_CallNonvirtualShortMethod;
    void* jni_CallNonvirtualShortMethodV;
    void* jni_CallNonvirtualShortMethodA;
    void* jni_CallNonvirtualIntMethod;
    void* jni_CallNonvirtualIntMethodV;
    void* jni_CallNonvirtualIntMethodA;
    void* jni_CallNonvirtualLongMethod;
    void* jni_CallNonvirtualLongMethodV;
    void* jni_CallNonvirtualLongMethodA;
    void* jni_CallNonvirtualFloatMethod;
    void* jni_CallNonvirtualFloatMethodV;
    void* jni_CallNonvirtualFloatMethodA;
    void* jni_CallNonvirtualDoubleMethod;
    void* jni_CallNonvirtualDoubleMethodV;
    void* jni_CallNonvirtualDoubleMethodA;
    void* jni_CallNonvirtualVoidMethod;
    void* jni_CallNonvirtualVoidMethodV;
    void* jni_CallNonvirtualVoidMethodA;

    void* jni_GetFieldID;

    void* jni_GetObjectField;
    void* jni_GetBooleanField;
    void* jni_GetByteField;
    void* jni_GetCharField;
    void* jni_GetShortField;
    void* jni_GetIntField;
    void* jni_GetLongField;
    void* jni_GetFloatField;
    void* jni_GetDoubleField;

    void* jni_SetObjectField;
    void* jni_SetBooleanField;
    void* jni_SetByteField;
    void* jni_SetCharField;
    void* jni_SetShortField;
    void* jni_SetIntField;
    void* jni_SetLongField;
    void* jni_SetFloatField;
    void* jni_SetDoubleField;

    void* jni_GetStaticMethodID;

    void* jni_CallStaticObjectMethod;
    void* jni_CallStaticObjectMethodV;
    void* jni_CallStaticObjectMethodA;
    void* jni_CallStaticBooleanMethod;
    void* jni_CallStaticBooleanMethodV;
    void* jni_CallStaticBooleanMethodA;
    void* jni_CallStaticByteMethod;
    void* jni_CallStaticByteMethodV;
    void* jni_CallStaticByteMethodA;
    void* jni_CallStaticCharMethod;
    void* jni_CallStaticCharMethodV;
    void* jni_CallStaticCharMethodA;
    void* jni_CallStaticShortMethod;
    void* jni_CallStaticShortMethodV;
    void* jni_CallStaticShortMethodA;
    void* jni_CallStaticIntMethod;
    void* jni_CallStaticIntMethodV;
    void* jni_CallStaticIntMethodA;
    void* jni_CallStaticLongMethod;
    void* jni_CallStaticLongMethodV;
    void* jni_CallStaticLongMethodA;
    void* jni_CallStaticFloatMethod;
    void* jni_CallStaticFloatMethodV;
    void* jni_CallStaticFloatMethodA;
    void* jni_CallStaticDoubleMethod;
    void* jni_CallStaticDoubleMethodV;
    void* jni_CallStaticDoubleMethodA;
    void* jni_CallStaticVoidMethod;
    void* jni_CallStaticVoidMethodV;
    void* jni_CallStaticVoidMethodA;

    void* jni_GetStaticFieldID;

    void* jni_GetStaticObjectField;
    void* jni_GetStaticBooleanField;
    void* jni_GetStaticByteField;
    void* jni_GetStaticCharField;
    void* jni_GetStaticShortField;
    void* jni_GetStaticIntField;
    void* jni_GetStaticLongField;
    void* jni_GetStaticFloatField;
    void* jni_GetStaticDoubleField;

    void* jni_SetStaticObjectField;
    void* jni_SetStaticBooleanField;
    void* jni_SetStaticByteField;
    void* jni_SetStaticCharField;
    void* jni_SetStaticShortField;
    void* jni_SetStaticIntField;
    void* jni_SetStaticLongField;
    void* jni_SetStaticFloatField;
    void* jni_SetStaticDoubleField;

    void* jni_NewString;
    void* jni_GetStringLength;
    void* jni_GetStringChars;
    void* jni_ReleaseStringChars;

    void* jni_NewStringUTF;
    void* jni_GetStringUTFLength;
    void* jni_GetStringUTFChars;
    void* jni_ReleaseStringUTFChars;

    void* jni_GetArrayLength;

    void* jni_NewObjectArray;
    void* jni_GetObjectArrayElement;
    void* jni_SetObjectArrayElement;

    void* jni_NewBooleanArray;
    void* jni_NewByteArray;
    void* jni_NewCharArray;
    void* jni_NewShortArray;
    void* jni_NewIntArray;
    void* jni_NewLongArray;
    void* jni_NewFloatArray;
    void* jni_NewDoubleArray;

    void* jni_GetBooleanArrayElements;
    void* jni_GetByteArrayElements;
    void* jni_GetCharArrayElements;
    void* jni_GetShortArrayElements;
    void* jni_GetIntArrayElements;
    void* jni_GetLongArrayElements;
    void* jni_GetFloatArrayElements;
    void* jni_GetDoubleArrayElements;

    void* jni_ReleaseBooleanArrayElements;
    void* jni_ReleaseByteArrayElements;
    void* jni_ReleaseCharArrayElements;
    void* jni_ReleaseShortArrayElements;
    void* jni_ReleaseIntArrayElements;
    void* jni_ReleaseLongArrayElements;
    void* jni_ReleaseFloatArrayElements;
    void* jni_ReleaseDoubleArrayElements;

    void* jni_GetBooleanArrayRegion;
    void* jni_GetByteArrayRegion;
    void* jni_GetCharArrayRegion;
    void* jni_GetShortArrayRegion;
    void* jni_GetIntArrayRegion;
    void* jni_GetLongArrayRegion;
    void* jni_GetFloatArrayRegion;
    void* jni_GetDoubleArrayRegion;

    void* jni_SetBooleanArrayRegion;
    void* jni_SetByteArrayRegion;
    void* jni_SetCharArrayRegion;
    void* jni_SetShortArrayRegion;
    void* jni_SetIntArrayRegion;
    void* jni_SetLongArrayRegion;
    void* jni_SetFloatArrayRegion;
    void* jni_SetDoubleArrayRegion;

    void* jni_RegisterNatives;
    void* jni_UnregisterNatives;

    void* jni_MonitorEnter;
    void* jni_MonitorExit;

    void* jni_GetJavaVM;

    void* jni_GetStringRegion;
    void* jni_GetStringUTFRegion;

    void* jni_GetPrimitiveArrayCritical;
    void* jni_ReleasePrimitiveArrayCritical;

    void* jni_GetStringCritical;
    void* jni_ReleaseStringCritical;

    void* jni_NewWeakGlobalRef;
    void* jni_DeleteWeakGlobalRef;

    void* jni_ExceptionCheck;

    void* jni_NewDirectByteBuffer;
    void* jni_GetDirectBufferAddress;
    void* jni_GetDirectBufferCapacity;

    // New 1_6 features

    void* jni_GetObjectRefType;

    // Module features

    void* jni_GetModule;

    // Virtual threads

    void* jni_IsVirtualThread;
};
