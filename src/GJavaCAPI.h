﻿//
// Created by Gengxin Gu on 16/5/5.
//

#ifndef AS_GJAVACAPI_H
#define AS_GJAVACAPI_H

#include "GXPrefix.h"
#if defined(GX_OS_ANDROID)
#include "GJavaAPI.h"
#include "GJavaClass.h"


class GJavaCAPI : public GJavaClass<6> {
public:
	static GJavaCAPI* Shared();

	float AppGetDefaultWindowScale(JNIEnv* jniEnv);
	void AppTerminate(JNIEnv* jniEnv);
	void UUIDCreate(JNIEnv* jniEnv,guint8* uuidOut);
};


#endif


#endif //AS_GJAVACAPI_H
