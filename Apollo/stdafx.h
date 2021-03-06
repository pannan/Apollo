// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <wrl/client.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <list>
#include <string>
#include <assert.h>
#include <memory>
#include <limits>
#include <unordered_map>
//#include <hash_map>
#include <iostream>
#include <strstream>
#include <sstream>
#include <memory>
#include <map>
#include <fstream>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <DirectXMath.h>
#include <utility>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXMathVector.inl>
#include <DirectXMathMatrix.inl>
#include <random>
#include "imgui.h"
#include <d3d9.h>
#include <algorithm>


#if defined(_WIN32_WINNT_WIN10) 
#   include <d3d12.h>
#   include <dxgi1_4.h>
#	include <d3d12sdklayers.h>
#	include <d3d12shader.h>
#	pragma comment(lib, "d3d12.lib")
#else
#   include <dxgi1_3.h>
#endif

#include <d3d11_2.h> // <d3d11_3.h> is only supported on Windows 10 (but then I'd rather use DX12!?)
#include <d3dcompiler.h>
#pragma warning( disable: 4838 )
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "freetype263MTd.lib")
#pragma comment(lib, "boost_context.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "gdi32.lib")
#pragma warning(disable: 4267)
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)

#ifdef uint32
#define typedef std::uint32_t uint32;
#endif

#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define SAFE_DELETE_ARRAY(x) {if(x) delete [] x; x = NULL;}

#define GET_RESOURCE_TYPE(handle) (handle & 0x000000ff);

#define GET_RESOURCE_INDEX(handle) ((handle >> 8) & 0x00ffffff);

#define APOLLO_PI			3.14159265358979323846

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#ifndef _max
#define _max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef _min
#define _min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1DComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2DComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3DComPtr;

typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceViewComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilViewComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetViewComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UnorderedAccessViewComPtr;


typedef Microsoft::WRL::ComPtr<ID3D11VertexShader> VSComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11HullShader> HSComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11DomainShader> DSComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11GeometryShader> GSComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11PixelShader> PSComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11ComputeShader> CSComPtr;
typedef Microsoft::WRL::ComPtr<ID3DBlob> BlobComPtr;
typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayoutComPtr;

typedef std::vector< Microsoft::WRL::ComPtr<ID3D11Query> > QueryBufferComPtr;

typedef std::map< std::string, std::string > ShaderMacros;

enum class ShaderType : uint8_t
{
	//UnknownShaderType = 0,
	VertexShader,
	TessellationControlShader,      // Hull Shader in DirectX
	TessellationEvaluationShader,   // Domain Shader in DirectX
	GeometryShader,
	PixelShader,
	ComputeShader,
	ShaderTypeCount,
};

enum class ShaderParameterType
{
	Invalid,    // Invalid parameter. Doesn't store a type.
	Texture,    // Texture.
	Sampler,    // Texture sampler.
	Buffer,     // Buffers, ConstantBuffers, StructuredBuffers.
	RWTexture,  // Texture that can be written to in a shader (using Store operations).
	RWBuffer,   // Read/write structured buffers.
	AppendBuffer,//append struct buffer
};

//#ifdef DEBUG
const float MAX_FLOAT = std::numeric_limits<float>::max();
const float MIN_FLOAT = std::numeric_limits<float>::min();
//#else
//const float MAX_FLOAT = 10000000000;// std::numeric_limits<float>::max();
//const float MIN_FLOAT = -10000000000;// std::numeric_limits<float>::min();
//#endif // DEBUG



enum PlaneAABBIntersectType
{
	PAIT_INSIDE,
	PAIT_OUTSIDE,
	PAIT_INTERSECT
};

enum class ClearFlags : uint8_t
{
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2,
	DepthStencil = Depth | Stencil,
	All = Color | Depth | Stencil,
};

enum class RenderPipelineType : uint8_t
{
	ForwardRender,
	DeferredRender,
	RenderPipelineCount
};

#define NAME_SPACE_BEGIN_APOLLO namespace Apollo {
#define NAME_SPACE_BEGIN_ATMOSPHERE namespace Atmosphere {
#define NAME_SPACE_BEGIN_REFERENCE namespace Reference {
#define NAME_SPACE_END }

// TODO:  在此处引用程序需要的其他头文件
