﻿//
//  GApplication.h
//  GX
//
//  Created by Gu Gengxin on 16/5/1.
//  Copyright © 2016年 Gengxin Gu. All rights reserved.
//

#ifndef GApplication_h
#define GApplication_h

#include "GXPrefix.h"
#include "GDataArray.h"
#include "GWindow.h"
#include "GXCWnd.h"
#if defined(GX_OS_ANDROID)
#include <android/input.h>
#include <android/native_activity.h>
#elif defined(GX_OS_QT)
#include <QTimer>
#endif
class GWindow;

class GApplication
#if defined(GX_OS_QT)
        : public QObject
#endif
{
#if defined(GX_OS_QT)
        Q_OBJECT
#endif
public:
	class InitData {
	public:
        InitData() {
            m_OSWindow=NULL;
        }
		InitData(void* osWin) {
			m_OSWindow = osWin;
		}
		~InitData() {

		}

		void* getOSWindow() {
			return m_OSWindow;
		}

	private:
		void* m_OSWindow;
	};

    class Delegate {
    public:
		virtual void appDidFinishLaunching(GApplication* application){GX_UNUSED(application)}
		virtual void appWillTerminate(GApplication* application){GX_UNUSED(application)}

		virtual void appCanCreateWindow(GApplication* application,void* osWindow) {
            GX_UNUSED(application)
            GX_UNUSED(osWindow)
		}
        virtual void appReceivedMemoryWarning(GApplication* application){GX_UNUSED(application)}

		virtual gint windowsSuggestedSamples() {
			return 4;
		}
		virtual gint windowsSuggestedDepth() {
			return 24;
		}
		virtual gint windowsSuggestedStencil() {
			return 8;
		}
    };
	
public:
    static GApplication* shared();
	static GApplication::Delegate* sharedDelegate();
	static void main(Delegate* dge, InitData* initData);
private:
    GApplication();
    ~GApplication();

#if defined(GX_OS_QT)
private slots:
    void idle();
private:
#else
    void idle();
#endif

	void eventDidFinishLaunching();
	void eventWillTerminate();

	void setCanCreateWindow(void* osWindow);

public:
    void addWindow(void* osWin);
    GWindow* firstWindow();
    gint getWindowCount();
    GWindow* getWindow(gint idx);
    void removeWindow(GWindow* win);

private:
    Delegate* m_Delegate;
    GPDArray<GWindow*> m_Windows;
	InitData m_InitData;
    
#if defined(GX_OS_APPLE)
    friend class _AppBridge;
    void* m_Helper;
#elif defined(GX_OS_WINDOWS)
	static LRESULT CALLBACK winMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void CALLBACK winTimerCallBack(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
	void createWinMsgWndAndStart();
	void destroyWinMsgWnd();

	GX::CWnd m_MsgWnd;
	UINT m_TimerID;
#elif defined(GX_OS_ANDROID)
	friend void Java_com_gxengine_gx_GAndroidJ_appIdle(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnCreate(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnStart(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnResume(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnPause(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnStop(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_activityOnDestroy(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnCreate(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnAttachedToWindow(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnDreamingStarted(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnDreamingStopped(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnDetachedFromWindow(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_dreamServiceOnDestroy(JNIEnv *, jclass, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_windowOnCreated(JNIEnv *, jclass, jobject, jobject, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_windowOnChanged(JNIEnv *, jclass, jobject, jobject, jint, jint, jobject);
	friend void Java_com_gxengine_gx_GAndroidJ_windowOnDestroyed(JNIEnv *, jclass, jobject, jobject, jobject);

	typedef enum __WinHolderType {
		_WinHolderType_Activity=0,
		_WinHolderType_DreamService,
	} _WinHolderType;

	class _WinData {
	public:
		_WinData() {
			holder=NULL;
			type=_WinHolderType_Activity;
		}
		jobject holder;
		_WinHolderType type;
	};
	
    void winHolderOnCreate(jobject holder,_WinHolderType type);

	GPDArray<_WinData> m_WinHolders;
#elif defined(GX_OS_QT)
    QTimer m_Timer;
#endif
};



#endif /* GApplication_h */
