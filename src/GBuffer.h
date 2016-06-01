﻿//
//  GBuffer.h
//  GX
//
//  Created by Gengxin Gu on 16/6/1.
//  Copyright © 2016年 Gengxin Gu. All rights reserved.
//

#ifndef GBuffer_h
#define GBuffer_h

#include "GXPrefix.h"
#include "GXOpenGL.h"
#include "GXDirectX.h"

class GIBuffer
{
public:
	virtual guint getOffset() = 0;
	virtual guint getStride() = 0;
#ifdef GX_OPENGL
	virtual void readyUse() = 0;
	virtual const GLvoid * getData() = 0;
	virtual void doneUse() = 0;

	inline const GLvoid* getData(gint offset) {
		return (const GLvoid*)(((guint8*)getData()) + getOffset() + offset);
	}
#elif defined(GX_DIRECTX)
	virtual ID3D10Buffer** getBufferPtr() = 0;
	virtual ID3D10Buffer*  getBuffer() = 0;
#endif
};

class GIIndexBuffer
{
public:
	virtual guint getFormat() = 0;
#ifdef GX_OPENGL
	virtual void readyUse() = 0;
	virtual const GLvoid * getData() = 0;
	virtual void doneUse() = 0;
#elif defined(GX_DIRECTX)
	virtual ID3D10Buffer** getBufferPtr() = 0;
	virtual ID3D10Buffer*  getBuffer() = 0;
#endif
};


#endif /* GBuffer_h */
