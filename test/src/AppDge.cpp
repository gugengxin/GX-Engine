﻿#include "AppDge.h"


AppDge* AppDge::shared()
{
	static AppDge g_Obj;
	return &g_Obj;
}

AppDge::AppDge()
{
}


AppDge::~AppDge()
{
}

void AppDge::appCanCreateWindow(GApplication* application,void* osWindow)
{
	GString* str = GString::autoAlloc();
	str->format("%Hello!1234%").arg('A', 10).arg(L'新',3);
	GX_LOG_P1(PrioDEBUG, "AppDge", "%s", str->c_str());


	application->addWindow(osWindow);
}
