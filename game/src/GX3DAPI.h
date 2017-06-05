﻿//
//  GX3DAPI.h
//  GX
//
//  Created by Gengxin Gu on 2016/10/30.
//  Copyright © 2016年 Gengxin Gu. All rights reserved.
//

#ifndef GX3DAPI_h
#define GX3DAPI_h


#include "GX3DAPIType.h"

#include "GXOpenGL.h"
#include "GXDirectX.h"
#include "GXMetal.h"

namespace GX {
    gint DToplogyToDrawMode(DToplogy dt);
	DToplogy DToplogyFromDrawMode(gint mode);
}


#endif /* GX3DAPI_h */
