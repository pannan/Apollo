// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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
//#include <unordered_map>
#include <hash_map>
#include <iostream>
#include <strstream>
#include <sstream>



#include "imgui.h"

#ifdef uint32
#define typedef std::uint32_t uint32;
#endif

#define GLYPH_PI 3.14159265f

#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define GET_RESOURCE_TYPE(handle) (handle & 0x000000ff);

#define GET_RESOURCE_INDEX(handle) ((handle >> 8) & 0x00ffffff);

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


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
