#pragma once

#include "TextureResource.h"

NAME_SPACE_BEGIN_APOLLO

class Texture3dDX11 : public TextureResource
{
public:

	friend class ShaderParameterDX11;

	Texture3dDX11(const std::string& name);

	Texture3dDX11(const std::string& path, uint32_t handle);

	Texture3dDX11(const std::string& path, uint32_t handle, D3D11_TEXTURE3D_DESC desc, ID3D11Texture3D* texture3d,
		ShaderResourceViewComPtr srv);
	virtual ~Texture3dDX11();

	void	bind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

	void unBind(UINT slotID, ShaderType shaderType, ShaderParameterType parameterType);

	void		setTexture3D(ID3D11Texture3D* tex) { m_tex3dDx11 = tex; tex->GetDesc(&m_tex3dDesc); }

	void		setShaderResourceView(ID3D11ShaderResourceView* srv) { m_shaderResourceViewPtr = srv; }

	ID3D11Texture3D*	getTexture3D() { return m_tex3dDx11.Get(); }

	ID3D11ShaderResourceView* getShaderResourceView() { return m_shaderResourceViewPtr.Get(); }

protected:

	ShaderResourceViewComPtr		m_shaderResourceViewPtr;

	D3D11_TEXTURE3D_DESC			m_tex3dDesc;
	Texture3DComPtr						m_tex3dDx11;
};

typedef std::shared_ptr<Texture3dDX11>	Texture3dDX11Ptr;

NAME_SPACE_END
