#include "stdafx.h"
#include "ShaderDX11.h"
#include "LogManager.h"
#include "RendererDX11.h"
#include "ShaderParameterDX11.h"

using namespace Apollo;
using namespace std;

// Inspired by: http://takinginitiative.net/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
{
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	if (paramDesc.Mask == 1) // 1 component
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 3) // 2 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 7) // 3 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32B32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32B32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		break;
		}
	}
	else if (paramDesc.Mask <= 15) // 4 components
	{
		switch (paramDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_UINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_SINT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_SINT;
		}
		break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
		{
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		break;
		}
	}

	return format;
}

std::string ShaderDX11::getLatestProfile(ShaderType type)
{
	// Query the current feature level:
	D3D_FEATURE_LEVEL featureLevel = RendererDX11::getInstance().getDevice()->GetFeatureLevel();

	switch (type)
	{
	case VertexShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "vs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "vs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "vs_4_0";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			return "vs_4_0_level_9_3";
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
			return "vs_4_0_level_9_1";
			break;
		}
		break;
	case TessellationControlShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "ds_5_0";
			break;
		}
		break;
	case TessellationEvaluationShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "hs_5_0";
			break;
		}
		break;
	case GeometryShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "gs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "gs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "gs_4_0";
			break;
		}
		break;
	case PixelShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "ps_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "ps_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "ps_4_0";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			return "ps_4_0_level_9_3";
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
			return "ps_4_0_level_9_1";
			break;
		}
		break;
	case ComputeShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "cs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "cs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "cs_4_0";
			break;
		}
	} // switch( type )

	  // Throw an exception?
	return "";
}

void ShaderDX11::release()
{
	m_pixelShader.Reset();
	m_domainShader.Reset();
	m_hullShader.Reset();
	m_geometrySHader.Reset();
	m_vertexShader.Reset();
	m_computeShader.Reset();
	m_inputLayoutPtr.Reset();
}

bool ShaderDX11::loadShaderFromString(	ShaderType shaderType, 
																const std::string& source, 
																const std::string& fileName,
																const ShaderMacros& shaderMacros, 
																const std::string& entryPoint, 
																const std::string& profile)
{
	HRESULT hr;
	const std::string pathName = /*"../bin/Assets/Shader/" +*/ fileName;
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

		std::string _profile = profile;
		if (profile == "latest")
		{
			_profile = getLatestProfile(shaderType);
			if (_profile.empty())
			{
				LogManager::getInstance().log("Invalid shader type for feature level!");
				return false;
			}
		}

		std::vector<D3D_SHADER_MACRO> macros;
		for (auto macro : shaderMacros)
		{
			// The macro definitions passed to this function only store temporary string objects.
			// I need to copy the temporary strings into the D3D macro type 
			// in order for it to persist outside of this for loop.
			std::string name = macro.first;
			std::string definition = macro.second;

			char* c_name = new char[name.size() + 1];
			char* c_definition = new char[definition.size() + 1];

			strncpy_s(c_name, name.size() + 1, name.c_str(), name.size());
			strncpy_s(c_definition, definition.size() + 1, definition.c_str(), definition.size());

			macros.push_back({ c_name, c_definition });
		}
		macros.push_back({ 0, 0 });


		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( _DEBUG )
		flags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif

		
		hr = D3DCompile((LPCVOID)source.c_str(), 
									source.size(), 
									pathName.c_str(), 
									macros.data(), 
									D3D_COMPILE_STANDARD_FILE_INCLUDE, 
									entryPoint.c_str(), 
									_profile.c_str(), 
									flags, 
									0, 
									&pShaderBlob, 
									&pErrorBlob);

		// We're done compiling.. Delete the macro definitions.
		for (D3D_SHADER_MACRO macro : macros)
		{
			delete[] macro.Name;
			delete[] macro.Definition;
		}
		macros.clear();

		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				//OutputDebugStringA(static_cast<char*>(pErrorBlob->GetBufferPointer()));
				//ReportError(static_cast<char*>(pErrorBlob->GetBufferPointer()));
				LogManager::getInstance().log(static_cast<char*>(pErrorBlob->GetBufferPointer()));
			}
			return false;
		}

		m_shaderBlob = pShaderBlob;
	}

	// After the shader recompiles, try to restore the shader parameters.
	ParameterMap shaderParameters = m_shaderParameters;

	// Destroy the last shader as we are now loading a new one.
	release();

	m_shaderType = shaderType;

	ID3D11Device* pdevice = RendererDX11::getInstance().getDevice();

	switch (m_shaderType)
	{
	case VertexShader:
		hr = pdevice->CreateVertexShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
		break;
	case TessellationControlShader:
		hr = pdevice->CreateHullShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_hullShader.GetAddressOf());
		break;
	case TessellationEvaluationShader:
		hr = pdevice->CreateDomainShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_domainShader.GetAddressOf());
		break;
	case GeometryShader:
		hr = pdevice->CreateGeometryShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_geometrySHader.GetAddressOf());
		break;
	case PixelShader:
		hr = pdevice->CreatePixelShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
		break;
	case ComputeShader:
		hr = pdevice->CreateComputeShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, m_computeShader.GetAddressOf());
		break;
	default:
		LogManager::getInstance().log("Error Shader Type!");
		break;
	}

	if (FAILED(hr))
	{
		LogManager::getInstance().log("Failed to create shader.");
		return false;
	}

	// Reflect the parameters from the shader.
	// Inspired by: http://members.gamedev.net/JasonZ/Heiroglyph/D3D11ShaderReflection.pdf
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
	hr = D3DReflect(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);

	if (FAILED(hr))
	{
		LogManager::getInstance().log("Failed to reflect shader.");
		return false;
	}

	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription;
	hr = pReflector->GetDesc(&shaderDescription);

	if (FAILED(hr))
	{
		LogManager::getInstance().log("Failed to get shader description from shader reflector.");
		return false;
	}

	//m_InputSemantics.clear();

	if (m_shaderType == VertexShader)
	{
		UINT numInputParameters = shaderDescription.InputParameters;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		for (UINT i = 0; i < numInputParameters; ++i)
		{
			D3D11_INPUT_ELEMENT_DESC inputElement;
			D3D11_SIGNATURE_PARAMETER_DESC parameterSignature;

			pReflector->GetInputParameterDesc(i, &parameterSignature);

			inputElement.SemanticName = parameterSignature.SemanticName;
			inputElement.SemanticIndex = parameterSignature.SemanticIndex;
			inputElement.InputSlot = 0;// i; // TODO: If using interleaved arrays, then the input slot should be 0.  If using packed arrays, the input slot will vary.
			inputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO: Figure out how to deal with per-instance data? .. Don't. Just use structured buffers to store per-instance data and use the SV_InstanceID as an index in the structured buffer.
			inputElement.InstanceDataStepRate = 0;
			inputElement.Format = GetDXGIFormat(parameterSignature);

			assert(inputElement.Format != DXGI_FORMAT_UNKNOWN);

			inputElements.push_back(inputElement);

			//m_InputSemantics.insert(SemanticMap::value_type(BufferBinding(inputElement.SemanticName, inputElement.SemanticIndex), i));
		}

		if (inputElements.size() > 0)
		{
			hr = pdevice->CreateInputLayout(inputElements.data(), (UINT)inputElements.size(), m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), m_inputLayoutPtr.GetAddressOf());
			if (FAILED(hr))
			{
				LogManager::getInstance().log("Failed to create input layout.");
				return false;
			}
		}
	}
	
	// Query Resources that are bound to the shader.
	for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		pReflector->GetResourceBindingDesc(i, &bindDesc);
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

	// Store data necessary to reload the shader if it changes on disc.
	m_shaderFileName = fileName;
	m_shaderMacros = shaderMacros;
	m_entryPoint = entryPoint;
	m_profile = profile;

	
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

	ID3D11DeviceContext* deviceContext = RendererDX11::getInstance().getDeviceContex();
	if (m_vertexShader.Get())
	{
		deviceContext->IASetInputLayout(m_inputLayoutPtr.Get());
		deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	}
	else if (m_hullShader)
	{
		deviceContext->HSSetShader(m_hullShader.Get(), nullptr, 0);
	}
	else if (m_domainShader)
	{
		deviceContext->DSSetShader(m_domainShader.Get(), nullptr, 0);
	}
	else if (m_geometrySHader)
	{
		deviceContext->GSSetShader(m_geometrySHader.Get(), nullptr, 0);
	}
	else if (m_pixelShader)
	{
		deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	}
	else if (m_computeShader)
	{
		deviceContext->CSSetShader(m_computeShader.Get(), nullptr, 0);
	}
}

void ShaderDX11::unBin()
{
	for (ParameterMap::value_type value : m_shaderParameters)
	{
		value.second->unBind();
	}

	ID3D11DeviceContext* deviceContext = RendererDX11::getInstance().getDeviceContex();
	if (m_vertexShader.Get())
	{
		deviceContext->IASetInputLayout(nullptr);
		deviceContext->VSSetShader(nullptr, nullptr, 0);
	}
	else if (m_hullShader)
	{
		deviceContext->HSSetShader(nullptr, nullptr, 0);
	}
	else if (m_domainShader)
	{
		deviceContext->DSSetShader(nullptr, nullptr, 0);
	}
	else if (m_geometrySHader)
	{
		deviceContext->GSSetShader(nullptr, nullptr, 0);
	}
	else if (m_pixelShader)
	{
		deviceContext->PSSetShader(nullptr, nullptr, 0);
	}
	else if (m_computeShader)
	{
		deviceContext->CSSetShader(nullptr, nullptr, 0);
	}
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