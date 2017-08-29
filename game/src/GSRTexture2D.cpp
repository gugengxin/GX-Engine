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
#include "GWindow.h"

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
#if MI_MASKMODE_NONE
 
#if MI_ALPHA
#if MI_COLORMUL
        lowp vec4 fragColor=vec4(buffer.color_mul.rgb,tex2d(texture.texBase,bridge.b_texCoord).a*buffer.color_mul.a);
#else
        lowp float texA=tex2d(texture.texBase,bridge.b_texCoord).a;
        lowp vec4 fragColor=vec4(texA,texA,texA,1.0);
#end
#else
#if MI_COLORMUL
        lowp vec4 fragColor=tex2d(texture.texBase,bridge.b_texCoord)*buffer.color_mul;
#else
        lowp vec4 fragColor=tex2d(texture.texBase,bridge.b_texCoord);
#end
#end
        gx_FragColor=fragColor;
 
#elif MI_MASKMODE_SSA_ADD_D1MSA
 
#if MI_ALPHA
#if MI_COLORMUL
        lowp vec4 fragColor=vec4(buffer.color_mul.rgb,tex2d(texture.texBase,bridge.b_texCoord).a*buffer.color_mul.a);
#else
        lowp float texA=tex2d(texture.texBase,bridge.b_texCoord).a;
        lowp vec4 fragColor=vec4(texA,texA,texA,1.0);
#end
#else
#if MI_COLORMUL
        lowp vec4 fragColor=tex2d(texture.texBase,bridge.b_texCoord)*buffer.color_mul;
#else
        lowp vec4 fragColor=tex2d(texture.texBase,bridge.b_texCoord);
#end
#end
 
#elif MI_MASKMODE_S1_ADD_D1
#end
 
	}
}
*///GX_SL


GSRTexture2D* GSRTexture2D::shared(bool alphaOnly,bool colorMul,GSRTexture2D::MaskMode mm)
{
    static GSRTexture2D* g_Shaders[2][2][MMCount]={{{NULL,},},};
    
    if (!g_Shaders[alphaOnly][colorMul][mm]) {
        ready();
        g_Shaders[alphaOnly][colorMul][mm] = new GSRTexture2D(alphaOnly,colorMul,mm);
        done();
    }
    return g_Shaders[alphaOnly][colorMul][mm];
}


GSRTexture2D::GSRTexture2D(bool alphaOnly,bool colorMul,MaskMode mm)
: GShaderBase((guint8)alphaOnly,(guint8)colorMul,(guint8)mm, 0)
{
    GX_SHADER_INPUT_INIT();
    static Macro g_Macros[] = {
        { "MI_ALPHA", NULL },
        { "MI_COLORMUL", NULL },
        { "MI_MASKMODE_NONE", NULL },
        { "MI_MASKMODE_SSA_ADD_D1MSA", NULL },
        { "MI_MASKMODE_S1_ADD_D1", NULL },
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

enum {
    A_position,
    A_texCoord,
    A_texCoordMask,
};

enum GXShaderU {
    U_mvp_mat,
    U_color_mul,
    U_texBase,
    U_texMask,

    UCount,
};

void GSRTexture2D::bindAttribLocations()
{
    setAttribLocation(A_position, "position");
    setAttribLocation(A_texCoord, "texCoord");
    if (getMaskMode()!=GSRTexture2D::MM_None) {
        setAttribLocation(A_texCoordMask, "texCoordMask");
    }
}

void GSRTexture2D::bindUniformLocations()
{
    setUniformLocation(U_mvp_mat, "mvp_mat");
    if (isColorMul()) {
        setUniformLocation(U_color_mul, "color_mul");
    }
    setUniformLocation(U_texBase, "texBase");
    if (getMaskMode()!=GSRTexture2D::MM_None) {
        setUniformLocation(U_texMask, "texMask");
    }
}


void GSRTexture2D::_InputBFunFloat_UShort(bool alphaOnly,bool colorMul,GSRTexture2D::MaskMode mm, GBuffer* buffer, guint offset, guint stride)
{
    GX_UNUSED(alphaOnly);
    GX_UNUSED(colorMul);

    readyUseBuffer(buffer);

    GX_glEnableVertexAttribArray(A_position);
    GX_glVertexAttribPointer(A_position, 3, GL_FLOAT, GL_FALSE, (GLsizei)stride, getBufferData(buffer, offset));
    GX_glEnableVertexAttribArray(A_texCoord);
    GX_glVertexAttribPointer(A_texCoord, 2, GL_UNSIGNED_SHORT, GL_TRUE, (GLsizei)stride, getBufferData(buffer, offset+3*sizeof(float)));

    if (mm!=GSRTexture2D::MM_None) {
        GX_glEnableVertexAttribArray(A_texCoordMask);
        GX_glVertexAttribPointer(A_texCoordMask, 2, GL_UNSIGNED_SHORT, GL_TRUE, (GLsizei)stride, getBufferData(buffer, offset+3*sizeof(float)+2*sizeof(unsigned short)));
    }

    doneUseBuffer(buffer);
}
void GSRTexture2D::_InputEFunFloat_UShort(bool alphaOnly,bool colorMul,GSRTexture2D::MaskMode mm)
{
    GX_UNUSED(alphaOnly);
    GX_UNUSED(colorMul);

    GX_glDisableVertexAttribArray(A_position);
    GX_glDisableVertexAttribArray(A_texCoord);
    if (mm!=GSRTexture2D::MM_None) {
        GX_glDisableVertexAttribArray(A_texCoordMask);
    }
}

void GSRTexture2D::_InputBFunFloat_Float(bool alphaOnly,bool colorMul,GSRTexture2D::MaskMode mm, GBuffer* buffer, guint offset, guint stride)
{
    GX_UNUSED(alphaOnly);
    GX_UNUSED(colorMul);

    readyUseBuffer(buffer);

    GX_glEnableVertexAttribArray(A_position);
    GX_glVertexAttribPointer(A_position, 3, GL_FLOAT, GL_FALSE, (GLsizei)stride, getBufferData(buffer, offset));
    GX_glEnableVertexAttribArray(A_texCoord);
    GX_glVertexAttribPointer(A_texCoord, 2, GL_FLOAT, GL_FALSE, (GLsizei)stride, getBufferData(buffer, offset+3*sizeof(float)));

    if (mm!=GSRTexture2D::MM_None) {
        GX_glEnableVertexAttribArray(A_texCoordMask);
        GX_glVertexAttribPointer(A_texCoordMask, 2, GL_FLOAT, GL_FALSE, (GLsizei)stride, getBufferData(buffer, offset+3*sizeof(float)+2*sizeof(float)));
    }

    doneUseBuffer(buffer);
}
void GSRTexture2D::_InputEFunFloat_Float(bool alphaOnly,bool colorMul,GSRTexture2D::MaskMode mm)
{
    GX_UNUSED(alphaOnly);
    GX_UNUSED(colorMul);

    GX_glDisableVertexAttribArray(A_position);
    GX_glDisableVertexAttribArray(A_texCoord);
    if (mm!=GSRTexture2D::MM_None) {
        GX_glDisableVertexAttribArray(A_texCoordMask);
    }
}

GSRTexture2D::InputBeginFunction GSRTexture2D::g_InputBFuns[] = {
    _InputBFunFloat_UShort,
    _InputBFunFloat_Float,
};
GSRTexture2D::InputEndFunction GSRTexture2D::g_InputEFuns[] = {
    _InputEFunFloat_UShort,
    _InputEFunFloat_Float,
};

#elif defined(GX_DIRECTX)

enum {
	CB_mvp_mat,
	CB_color_mul,
};

bool GSRTexture2D::createInputLayout(ID3D10Device* device, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength)
{
	{
		D3D10_INPUT_ELEMENT_DESC layouts[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R16G16_UNORM, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		setInputLayout(IT_Float_UShort, device, pShaderBytecodeWithInputSignature, BytecodeLength, layouts, sizeof(layouts) / sizeof(layouts[0])-(getMaskMode()==MM_None));
	}
	{
		D3D10_INPUT_ELEMENT_DESC layouts[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		setInputLayout(IT_Float_Float, device, pShaderBytecodeWithInputSignature, BytecodeLength, layouts, sizeof(layouts) / sizeof(layouts[0])-(getMaskMode()==MM_None));
	}
	return true;
}
bool GSRTexture2D::createConstantBuffer(ID3D10Device* device)
{
	D3D10_BUFFER_DESC cbDesc;
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(GMatrix4);
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;

	setConstantBuffer(CB_mvp_mat, device, &cbDesc, NULL);

	if (isColorMul()) {
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.ByteWidth = sizeof(GColor4F);
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		setConstantBuffer(CB_color_mul, device, &cbDesc, NULL);
	}
	return true;
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

#endif




void GSRTexture2D::draw(GCanvas* canvas,
	GBuffer* buffer, guint bufOffset, guint bufStride,
	InputType inputType,
	GTexture2D* texBase,
	gint mode, gint first, gint count,
	GTexture2D* texMask)
{
#if defined(GX_OPENGL)

    useProgram();

    g_InputBFuns[inputType](isAlphaOnly(),isColorMul(),getMaskMode(), buffer, bufOffset, bufStride);

    setUniformMatrix4fv(U_mvp_mat, 1, GL_FALSE, (const GLfloat*)canvas->updateMVPMatrix(0));
    if (isColorMul()) {
        setUniform4fv(U_color_mul, 1, (const GLfloat*)canvas->updateColorMul(0));
    }

    GX_glActiveTexture ( GL_TEXTURE0 );
    bindTexture( GL_TEXTURE_2D, texBase);
    setUniform1i ( U_texBase , 0 );

    if (getMaskMode()!=MM_None) {
        GX_glActiveTexture ( GL_TEXTURE1 );
        bindTexture( GL_TEXTURE_2D, texMask);
        setUniform1i ( U_texMask , 1 );
    }


    GX_glDrawArrays((GLenum)mode, (GLint)first, (GLsizei)count);

    if (getMaskMode()!=MM_None) {
        GX_glActiveTexture ( GL_TEXTURE1 );
        GX_glBindTexture ( GL_TEXTURE_2D, 0 );
    }

    GX_glActiveTexture ( GL_TEXTURE0 );
    GX_glBindTexture ( GL_TEXTURE_2D, 0 );

    g_InputEFuns[inputType](isAlphaOnly(),isColorMul(),getMaskMode());

    unuseProgram();
    
#elif defined(GX_DIRECTX)

	ID3D10Device* device = GX::direct3DDevice();

	UINT offset = (UINT)bufOffset;
	UINT stride = (UINT)bufStride;
	IASetVertexBuffers(device, 0, 1, buffer, &offset, &stride);
	device->IASetPrimitiveTopology((D3D10_PRIMITIVE_TOPOLOGY)mode);
	device->IASetInputLayout(m_Layouts[inputType]);

	ID3D10Buffer* cbToMapped;
	void* pMap;

	cbToMapped = m_ConstBuffers[CB_mvp_mat];
	cbToMapped->Map(D3D10_MAP_WRITE_DISCARD, 0, &pMap);
	const float* mvp = canvas->updateMVPMatrix(0);
	((GMatrix4*)mvp)->transposeCopyTo((GMatrix4*)pMap);
	cbToMapped->Unmap();
	device->VSSetConstantBuffers(0, 1, &cbToMapped);

	if (isColorMul()) {
		cbToMapped = m_ConstBuffers[CB_color_mul];
		cbToMapped->Map(D3D10_MAP_WRITE_DISCARD, 0, &pMap);
		memcpy(pMap, canvas->updateColorMul(0), sizeof(GColor4F));
		cbToMapped->Unmap();
		device->PSSetConstantBuffers(0, 1, &cbToMapped);
	}

	PSSetShaderResources(device, 0, 1, texBase);
	PSSetSamplers(device, 0, 1, texBase);

	if (getMaskMode()!=MM_None) {
		PSSetShaderResources(device, 1, 1, texMask);
		PSSetSamplers(device, 1, 1, texMask);
	}

	device->VSSetShader(getVertexShader());
	device->PSSetShader(getPixelShader());

	device->Draw((UINT)count, (UINT)first);
    
#elif defined(GX_METAL)
    
    id<MTLRenderCommandEncoder>rce=GX_CAST_R(id<MTLRenderCommandEncoder>, canvas->metalNewRCE());
    
    [rce setRenderPipelineState:GX_CAST_R(id<MTLRenderPipelineState>,getPLStates()[inputType*GX::_DBlendCount+canvas->metalBlendIndex()])];
    
    setVertexBuffer(rce, buffer, bufOffset, 0);
    
    const float* mvp = canvas->updateMVPMatrix(0);

    guint8 bufID[16]={0};
    (*GX_CAST_R(guint32*, bufID))=canvas->getMVPMatrixID(0);
    GX::MetalBufferCache::Buffer buf=GX::MetalBufferCache::shared()->requestBuffer(GX::MetalBufferCache::TypeMatrixMVP,bufID,GX_MATRIX_SIZE);
    ((GMatrix4*)mvp)->transposeCopyTo(GX_CAST_R(GMatrix4*,GX_CAST_R(guint8*, [GX_CAST_R(id<MTLBuffer>, buf.buffer) contents])+buf.offset));
    
    [rce setVertexBuffer:GX_CAST_R(id<MTLBuffer>, GX_CAST_R(id<MTLBuffer>, buf.buffer)) offset:buf.offset atIndex:1];
    
    if (isColorMul()) {
        const float* clrMul = canvas->updateColorMul(0);
        memcpy(bufID, clrMul, sizeof(GColor4F));
        buf=GX::MetalBufferCache::shared()->requestBuffer(GX::MetalBufferCache::TypeColorMul,bufID,sizeof(GColor4F));
        memcpy(GX_CAST_R(guint8*, [GX_CAST_R(id<MTLBuffer>, buf.buffer) contents])+buf.offset, clrMul, sizeof(GColor4F));
        
        [rce setFragmentBuffer:GX_CAST_R(id<MTLBuffer>, buf.buffer) offset:buf.offset atIndex:0];
    }
    
    setFragmentTexture(rce, texBase, 0);
    if (getMaskMode()!=MM_None) {
        setFragmentTexture(rce, texMask, 1);
    }
    
    [rce drawPrimitives:(MTLPrimitiveType)mode vertexStart:(NSUInteger)first vertexCount:(NSUInteger)count];
    
    [rce endEncoding];
    
    
#endif
}


