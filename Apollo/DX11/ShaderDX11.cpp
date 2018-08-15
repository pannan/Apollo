#include "stdafx.h"
#include "ShaderDX11.h"
#include "LogManager.h"
#include "RendererDX11.h"
#include "ShaderParameterDX11.h"
#include "HLSLDX11Factory.h"

using namespace Apollo;
using namespace std;

void ShaderDX11::release()
{
}

bool ShaderDX11::loadShaderFromString(	ShaderType shaderType, 
																const std::string& source, 
																const std::string& fileName,
																const ShaderMacros& shaderMacros, 
																const std::string& entryPoint, 
																const std::string& profile)
{
	m_hlslResourcePtr = HLSLDX11Factory::getInstance().createShaderFromFile(shaderType, fileName, shaderMacros, entryPoint, profile);
	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription = m_hlslResourcePtr->getShaderDesc();

	//	// After the shader recompiles, try to restore the shader parameters.
	ParameterMap shaderParameters = m_shaderParameters;
	// Query Resources that are bound to the shader.
	for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc = m_hlslResourcePtr->getBindingDesc(i);
		//m_pReflector->GetResourceBindingDesc(i, &bindDesc);
		std::string resourceName = bindDesc.Name;

		ShaderParameterType parameterType = ShaderParameterType::Invalid;

		switch (bindDesc.Type)
		{
		case D3D_SIT_TEXTURE:
			parameterType = ShaderParameterType::Texture;
			break;
		case D3D_SIT_SAMPLER:
			parameterType = ShaderParameterType::Sampler;
			break;
		case D3D_SIT_CBUFFER:
		case D3D_SIT_STRUCTURED:
			parameterType = ShaderParameterType::Buffer;
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			parameterType = ShaderParameterType::RWBuffer;
			break;
		case D3D_SIT_UAV_RWTYPED:
			parameterType = ShaderParameterType::RWTexture;
			break;
		case  D3D_SIT_UAV_APPEND_STRUCTURED:
			parameterType = ShaderParameterType::AppendBuffer;
			break;
		}

		// Create an empty shader parameter that should be filled-in by the application.
		std::shared_ptr<ShaderParameterDX11> shaderParameter = std::make_shared<ShaderParameterDX11>(resourceName, bindDesc.BindPoint, shaderType, parameterType);
		m_shaderParameters.insert(ParameterMap::value_type(resourceName, shaderParameter));
	}

	// Now try to restore the original shader parameters (if there were any)
	for (auto shaderParameter : shaderParameters)
	{
		ParameterMap::iterator iter = m_shaderParameters.find(shaderParameter.first);
		if (iter != m_shaderParameters.end())
		{
			iter->second = shaderParameter.second;
		}
	}

	return true;
}

bool ShaderDX11::loadShaderFromFile(	ShaderType shaderType, 
															const std::string& fileName,
															const ShaderMacros& shaderMacros,
															const std::string& entryPoint,
															const std::string& profile)
{
	bool result = false;

	const std::string pathName = /*"../bin/Assets/Shader/" +*/ fileName;

	std::ifstream inputFile(pathName);

	if (inputFile.is_open() == false)
	{
		LogManager::getInstance().log(pathName + " Open False!");
		return  false;
	}

	
	std::string source((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

	result =loadShaderFromString(shaderType, source, fileName, shaderMacros, entryPoint, profile);

	return result;
}

void ShaderDX11::bin()
{
	for (ParameterMap::value_type value : m_shaderParameters)
	{
		value.second->bind();
	}

	if (m_hlslResourcePtr)
		m_hlslResourcePtr->bind();
}

void ShaderDX11::unBind()
{
	//这里不用做了
	//1 是效率太低，去掉后场景fps 72->112
	//2 现在的方式是渲染前多有状态都设置，没有默认状态一说，所以渲染完后不用设置
	for (ParameterMap::value_type value : m_shaderParameters)
	{
		value.second->unBind();
	}

	if (m_hlslResourcePtr)
		m_hlslResourcePtr->unBind();
}

ShaderParameterPtr	ShaderDX11::getShaderParameter(std::string name)
{
	ParameterMap::iterator it = m_shaderParameters.find(name);
	if (it != m_shaderParameters.end())
		return it->second;

	LogManager::getInstance().log("ShaderParameter:" + name + "not find!");
	return nullptr;
}

void ShaderDX11::setConstantBuffer(std::string name, ConstantBufferDX11Ptr buffer)
{
	ShaderParameterPtr parameter = getShaderParameter(name);
	if (parameter)
	{
		parameter->setConstantBuffer(buffer);
	}
}

void ShaderDX11::setStructuredBuffer(std::string name, StructuredBufferDX11Ptr buffer)
{
	ShaderParameterPtr parameter = getShaderParameter(name);
	if (parameter)
	{
		parameter->setStructuredBuffer(buffer);
	}
}

void ShaderDX11::setTexture2d(std::string name, Texture2dDX11* tex2dDX11)
{
	ShaderParameterPtr parameter = getShaderParameter(name);
	if (parameter)
	{
		parameter->setTexture2d(tex2dDX11);
	}
}