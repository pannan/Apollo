#include "stdafx.h"
#include "TextureDX11ResourceFactory.h"
#include "ResourceManager.h"
#include "TextureResource.h"
#include "LogManager.h"
#include "DirectXTex.h"
#include "CharacterTools.h"
#include "RendererDX11.h"
#include "Texture2dDX11.h"
#include "TextureCubeMapDX11.h"

using namespace Apollo;
using namespace std;
using namespace DirectX;

TextureDX11ResourceFactory::TextureDX11ResourceFactory()
{
	ResourceManager::getInstance().registerResourceFactory("dds", this);
	ResourceManager::getInstance().registerResourceFactory("tga", this);
	ResourceManager::getInstance().registerResourceFactory("exr", this);
}

TextureDX11ResourceFactory::~TextureDX11ResourceFactory()
{
	ResourceManager::getInstance().unRegisterResourceFactory("dds", this);
	ResourceManager::getInstance().unRegisterResourceFactory("tga", this);
	ResourceManager::getInstance().unRegisterResourceFactory("exr", this);

	for each (TextureResource* var in m_textureResourceList)
	{
		SAFE_DELETE(var);
	}
}

TextureResource* TextureDX11ResourceFactory::loadDDS(const std::string& path, uint32_t handle)
{
	TexMetadata mdata;

	wstring wpath;
	StringToWString(path, wpath);
	HRESULT hr = GetMetadataFromDDSFile(wpath.c_str(), DDS_FLAGS_NONE, mdata);
	if (hr != S_OK)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] GetMetadataFromDDSFile error! file:" + path);
		return nullptr;
	}

	ScratchImage image;
	hr = LoadFromDDSFile(wpath.c_str(), DDS_FLAGS_NONE, &mdata, image);
	if (FAILED(hr))
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] LoadFromDDSFile error! file:" + path);
		return nullptr;
	}

	// Special case to make sure Texture cubes remain arrays
	//mdata.miscFlags &= ~TEX_MISC_TEXTURECUBE;
	ID3D11ShaderResourceView* srv = nullptr;
	hr = CreateShaderResourceView(	RendererDX11::getInstance().getDevice(), 
															image.GetImages(), 
															image.GetImageCount(), 
															mdata, 
															&srv);

	if (hr != S_OK)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::loadDDS] LoadFromDDSFile error! file:" + path);
		return nullptr;
	}
	
	if (mdata.IsCubemap())
	{
		TextureCubeMapDX11* texCubeMap = new TextureCubeMapDX11(path, handle, srv);
		return texCubeMap;
	}
	else
	{
		if (mdata.dimension == TEX_DIMENSION_TEXTURE1D)
		{
			return nullptr;
		}
		else if (mdata.dimension == TEX_DIMENSION_TEXTURE2D)
		{
			Texture2dDX11* tex2d = new Texture2dDX11(path, handle, srv);
			return tex2d;
		}
		else
		{
			return nullptr;
		}
	}
	
	return nullptr;
}

uint32_t TextureDX11ResourceFactory::createTexture2D(const std::string& name, Texture2dConfigDX11& config)
{
	uint32_t index = m_textureResourceList.size();
	uint32_t handle = RT_TEXTURE;
	handle |= (index << 8);

	ID3D11Texture2D * tex2d = nullptr;
	HRESULT hr = RendererDX11::getInstance().getDevice()->CreateTexture2D(&config.GetTextureDesc(), nullptr, &tex2d);
	if (hr != S_OK)
	{
		LogManager::getInstance().log(name + " create failed!");
		return 0;
	}

	DepthStencilViewComPtr			depthStencilView;
	ShaderResourceViewComPtr		shaderResourceView;
	RenderTargetViewComPtr			renderTargetView;
	UnorderedAccessViewComPtr	unorderedAccessView;

	D3D11_TEXTURE2D_DESC& desc = config.GetTextureDesc();
	 if ( (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL ) != 0 )
        {
            // Create the depth/stencil view for the texture.
            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			depthStencilViewDesc.Format = desc.Format;
            depthStencilViewDesc.Flags = 0; 

            if (desc.ArraySize > 1 )
            {
                if ( desc.SampleDesc.Count > 1 )
                {
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                    depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = 0;
                    depthStencilViewDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
                }
                else
                {
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
                    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
                    depthStencilViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
                }
            }
            else
            {
                if (desc.SampleDesc.Count > 1 )
                {
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                    depthStencilViewDesc.Texture2D.MipSlice = 0;
                }
            }

            if ( FAILED(RendererDX11::getInstance().getDevice()->CreateDepthStencilView(tex2d, &depthStencilViewDesc, depthStencilView.GetAddressOf()) ) )
            {
                LogManager::getInstance().log( "Failed to create depth/stencil view." );
            }
        }
        
        if ( ( desc.BindFlags & D3D11_BIND_SHADER_RESOURCE ) != 0 )
        {
            // Create a Shader resource view for the texture.
            D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
            resourceViewDesc.Format = desc.Format;

            if ( desc.ArraySize > 1 )
            {
                if ( desc.SampleDesc.Count > 1 )
                {
                    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                    resourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
                    resourceViewDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
                }
                else
                {
                    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    resourceViewDesc.Texture2DArray.FirstArraySlice = 0;
                    resourceViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
					resourceViewDesc.Texture2DArray.MipLevels = desc.MipLevels;
                    resourceViewDesc.Texture2DArray.MostDetailedMip = 0;
                }
            }
            else
            {
                if ( desc.SampleDesc.Count > 1 )
                {
                    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    resourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
					resourceViewDesc.Texture2D.MipLevels = desc.MipLevels;
                    resourceViewDesc.Texture2D.MostDetailedMip = 0;
                }
            }

            if ( FAILED(RendererDX11::getInstance().getDevice()->CreateShaderResourceView(tex2d, &resourceViewDesc, shaderResourceView.GetAddressOf()) ) )
            {
                LogManager::getInstance().log( "Failed to create texture resource view." );
            } 
            else if ( desc.MipLevels == 0 )
            {
				RendererDX11::getInstance().getDeviceContex()->GenerateMips(shaderResourceView.Get() );
            }
        }

        if ( ( desc.BindFlags & D3D11_BIND_RENDER_TARGET ) != 0 )
        {
            // Create the render target view for the texture.
            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            renderTargetViewDesc.Format = desc.Format;

            if ( desc.ArraySize > 1 )
            {
                if ( desc.SampleDesc.Count > 1 )
                {
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
                    renderTargetViewDesc.Texture2DArray.ArraySize = desc.ArraySize;

                }
                else
                {
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
                    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
                    renderTargetViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
                }
            }
            else
            {
                if ( desc.SampleDesc.Count > 1 )
                {
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    renderTargetViewDesc.Texture2D.MipSlice = 0;
                }
            }

            if ( FAILED(RendererDX11::getInstance().getDevice()->CreateRenderTargetView( tex2d, &renderTargetViewDesc, renderTargetView.GetAddressOf()) ) )
            {
                LogManager::getInstance().log( "Failed to create render target view." );
            }
        }

        if ( ( desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) != 0 )
        {
            // UAVs cannot be multi sampled.
            assert( desc.SampleDesc.Count == 1 );

            // Create a Shader resource view for the texture.
            D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
            unorderedAccessViewDesc.Format = desc.Format;

            if ( desc.ArraySize > 1 )
            {
                unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
                unorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
                unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
                unorderedAccessViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
            }
            else
            {
                unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
                unorderedAccessViewDesc.Texture2D.MipSlice = 0;
            }

            if ( FAILED(RendererDX11::getInstance().getDevice()->CreateUnorderedAccessView( tex2d, &unorderedAccessViewDesc, unorderedAccessView.GetAddressOf()) ) )
            {
                LogManager::getInstance().log( "Failed to create unordered access view." );
            }
        }

	Texture2dDX11* tex2dDX11 = new Texture2dDX11(name, handle, shaderResourceView.Get(),depthStencilView,renderTargetView,unorderedAccessView);

	m_textureResourceList.push_back(tex2dDX11);

	return handle;
}

uint32_t TextureDX11ResourceFactory::createResource(const std::string& path, const std::string& name, const std::string& type)
{
	uint32_t index = m_textureResourceList.size();
	uint32_t handle = RT_TEXTURE;
	handle |= (index << 8);

	//ÅÐ¶Ïºó×ºÃû
	int pos = name.find_last_not_of('.');
	if (pos == std::string::npos)
	{
		LogManager::getInstance().log("[TextureDX11ResourceFactory::createResource] name error!");
		return 0;
	}
	
	string suffixName = type;// name.substr(pos + 1, name.size() - pos);

	TextureResource* tex = nullptr;
	if (suffixName == "dds")
	{
		tex = loadDDS(path, handle);
	}


	m_textureResourceList.push_back(tex);

	return handle;
}

Resource* TextureDX11ResourceFactory::getResource(uint32_t handle)
{
	uint32_t index = GET_RESOURCE_INDEX(handle);
	if (m_textureResourceList.size() <= index)
	{
		stringstream ss;
		ss << "[TextureDX11ResourceFactory::getResource] Index:" << index << "Over Index!";
		return nullptr;
	}

	return m_textureResourceList[index];
}