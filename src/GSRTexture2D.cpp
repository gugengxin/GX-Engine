﻿//
//  GSRTexture2D.cpp
//  GX
//
//  Created by Gu Gengxin on 16/8/7.
//  Copyright © 2016年 Gengxin Gu. All rights reserved.
//

#include "GSRTexture2D.h"
#include "GSRTexture2D.gxsl"
#include "GContext.h"
#include "GXMath.h"

/*//GX_SL
vs {
	layout {
		high vec4 position:hl_POSITION,m_attribute0;
        medi vec2 texCoord:hl_TEXCOORD0,m_attribute1;
#if MI_MASKMODE_NONE
#else
        medi vec2 texCoordMask:hl_TEXCOORD1,m_attribute2;
#end
	}
	buffer {
		high mat4 mvp_mat;
	}
	bridge {
        medi vec2 b_texCoord:hl_TEXCOORD0;
#if MI_MASKMODE_NONE
#else
        medi vec2 b_texCoordMask:hl_TEXCOORD1;
#end
	}
	main {
		gx_Position = mul(buffer.mvp_mat,layout.position);
        bridge.b_texCoord=layout.texCoord;
#if MI_MASKMODE_NONE
#else
        bridge.b_texCoordMask=layout.texCoordMask;
#end
 
	}
}
fp {
	buffer {
#if MI_COLORMUL
        lowp vec4 color_mul;
#end
	}
    texture {
        lowp tex2d[0] texBase;
#if MI_MASKMODE_NONE
#else
        lowp tex2d[1] texMask;
#end
    }
	main {
#if MI_ALPHA
#if MI_COLORMUL
        float4 fragColor=float4(buffer.colorMul.rgb,tex2d(texture.texBase,bridge.b_texCoord).a*buffer.colorMul.a);
#else
        float  texA=tex2d(texture.texBase,bridge.b_texCoord).a;
        float4 fragColor=float4(texA,texA,texA,1.0);
#end
#else
#if MI_COLORMUL
        float4 fragColor=tex2d(texture.texBase,bridge.b_texCoord)*buffer.colorMul;
#else
        float4 fragColor=tex2d(texture.texBase,bridge.b_texCoord);
#end
#end
 
#if MI_MASKMODE_NONE
//#elif
#end
        gx_FragColor=fragColor;
	}
}
*///GX_SL



GSRTexture2D::GSRTexture2D(GContext* ctx,bool alphaOnly,bool colorMul,MaskMode mm)
: GShaderBase(ctx, (guint8)alphaOnly,(guint8)colorMul,(guint8)mm, 0)
{
    GX_SHADER_INPUT_INIT();
    static Macro g_Macros[] = {
        { "MI_ALPHA", NULL },
        { "MI_COLORMUL", NULL },
        { "MI_MASKMODE_NONE", NULL },
    };
    Macro macros[] = { {NULL,NULL}, {NULL,NULL}, {NULL,NULL}, {NULL,NULL} };
    gint i=0;
    if (alphaOnly) {
        macros[i]=g_Macros[0];
        i++;
    }
    if (colorMul) {
        macros[i]=g_Macros[1];
        i++;
    }
    macros[i]=g_Macros[2+mm];
    
    load(g_SrcVS, g_SrcFP, macros);
}

GSRTexture2D::~GSRTexture2D()
{

    GX_SHADER_INPUT_FINA();
}



#if defined(GX_OPENGL)
void GSRTexture2D::bindAttribLocations()
{
}
void GSRTexture2D::bindUniformLocations()
{
}
#elif defined(GX_DIRECTX)
bool GSRTexture2D::createInputLayout(ID3D10Device* device, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength)
{
}
bool GSRTexture2D::createConstantBuffer(ID3D10Device* device)
{
}
#elif defined(GX_METAL)

enum {
    UB_mvp_mat,
    UB_color_mul,
};

void GSRTexture2D::deployPLState(gint inputType,void* plStateDescriptor)
{
#define M_PSD() GX_CAST_R(MTLRenderPipelineDescriptor*, plStateDescriptor)
    switch (inputType) {
        case IT_Float_UShort:
        {
            MTLVertexDescriptor* vd=[[MTLVertexDescriptor alloc] init];
            
            // 设置attributes
            // pos
            vd.attributes[0].format = MTLVertexFormatFloat3;
            vd.attributes[0].bufferIndex = 0;
            vd.attributes[0].offset = 0;
            // texCoord
            vd.attributes[1].format = MTLVertexFormatUShort2Normalized;
            vd.attributes[1].bufferIndex = 0;
            vd.attributes[1].offset = 3 * sizeof(float);
            
            // mask
            if (getMaskMode()!=MaskMode::MM_None) {
                vd.attributes[2].format = MTLVertexFormatUShort2Normalized;
                vd.attributes[2].bufferIndex = 0;
                vd.attributes[2].offset = 3*sizeof(float) + 2*sizeof(unsigned short);
                
                // 设置layouts
                vd.layouts[0].stride = 3*sizeof(float) + 2*sizeof(unsigned short) + 2*sizeof(unsigned short);
            }
            else {
                // 设置layouts
                vd.layouts[0].stride = 3*sizeof(float) + 2*sizeof(unsigned short);
            }
            vd.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
            vd.layouts[0].stepRate=1;
            
            M_PSD().vertexDescriptor=vd;
            [vd release];
        }
            break;
        case IT_Float_Float:
        {
            MTLVertexDescriptor* vd=[[MTLVertexDescriptor alloc] init];
            
            // 设置attributes
            // pos
            vd.attributes[0].format = MTLVertexFormatFloat3;
            vd.attributes[0].bufferIndex = 0;
            vd.attributes[0].offset = 0;
            // texCoord
            vd.attributes[1].format = MTLVertexFormatFloat2;
            vd.attributes[1].bufferIndex = 0;
            vd.attributes[1].offset = 3 * sizeof(float);
            
            // mask
            if (getMaskMode()!=MaskMode::MM_None) {
                vd.attributes[2].format = MTLVertexFormatFloat2;
                vd.attributes[2].bufferIndex = 0;
                vd.attributes[2].offset = 3*sizeof(float) + 2*sizeof(float);
                
                // 设置layouts
                vd.layouts[0].stride = 3*sizeof(float) + 2*sizeof(float) + 2*sizeof(float);
            }
            else {
                // 设置layouts
                vd.layouts[0].stride = 3*sizeof(float) + 2*sizeof(float);
            }
            vd.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
            vd.layouts[0].stepRate=1;
            
            M_PSD().vertexDescriptor=vd;
            [vd release];
        }
            break;
        default:
            break;
    }
#undef M_PSD
}
void GSRTexture2D::createUniformBuffer(void* device)
{
    setUniformBuffer(UB_mvp_mat, device, GX_MATRIX_SIZE);
    if (isColorMul()) {
        setUniformBuffer(UB_color_mul, device, sizeof(GColor4F));
    }
}

#endif




void GSRTexture2D::draw(GPainter& painter,
                        GIBuffer* buffer,GSRTexture2D::InputType inputType,
                        GTexture2D* texBase,
                        gint mode,gint first,gint count,
                        GTexture2D* texMask)
{
#if defined(GX_OPENGL)
    
#elif defined(GX_DIRECTX)
    
#elif defined(GX_METAL)
    
    id<MTLRenderCommandEncoder>rce=GX_CAST_R(id<MTLRenderCommandEncoder>, currentRenderEncoder());
    
    [rce setRenderPipelineState:GX_CAST_R(id<MTLRenderPipelineState>,getPLStates()[inputType])];
    
    [rce setVertexBuffer:GX_CAST_R(id<MTLBuffer>, buffer->getBuffer()) offset:buffer->getOffset() atIndex:0];
    
    void* pMap=[GX_CAST_R(id<MTLBuffer>, getUBuffers()[UB_mvp_mat]) contents];
    const float* mvp = painter.updateMVPMatrix();
    ((GMatrix4*)mvp)->transpose();
    memcpy(pMap, mvp, GX_MATRIX_SIZE);
    [rce setVertexBuffer:GX_CAST_R(id<MTLBuffer>, getUBuffers()[UB_mvp_mat]) offset:0 atIndex:1];
    
    if (isColorMul()) {
        pMap=[GX_CAST_R(id<MTLBuffer>, getUBuffers()[UB_color_mul]) contents];
        const float* clrMul = painter.updateColorMul();
        memcpy(pMap, clrMul, sizeof(GColor4F));
        [rce setFragmentBuffer:GX_CAST_R(id<MTLBuffer>, getUBuffers()[UB_color_mul]) offset:0 atIndex:0];
    }
    
    setFragmentTexture(rce, texBase, 0);
    if (getMaskMode()!=MM_None) {
        setFragmentTexture(rce, texMask, 1);
    }
    
    [rce drawPrimitives:(MTLPrimitiveType)mode vertexStart:(NSUInteger)first vertexCount:(NSUInteger)count];
    
#endif
}


