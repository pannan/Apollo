#include "stdafx.h"
#include "ShaderDX11.h"
#include "LogManager.h"
#include "RendererDX11.h"

using namespace Apollo;
using namespace std;

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

bool ShaderDX11::loadShaderFromString(	ShaderType shaderType, 
																const std::string& source, 
																const std::string& fileName,
																const ShaderMacros& shaderMacros, 
																const std::string& entryPoint, 
																const std::string& profile)
{
	HRESULT hr;
	const std::string pathName = "Assets\\Shader\\" + fileName;
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
		flags |= D3DCOMPILE_DEBUG;
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
	ParameterMap shaderParameters = m_ShaderParameters;

	// Destroy the last shader as we are now loading a new one.
	Destroy();

	m_ShaderType = shaderType;

	switch (m_ShaderType)
	{
	case VertexShader:
		hr = m_pDevice->CreateVertexShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
		break;
	case TessellationControlShader:
		hr = m_pDevice->CreateHullShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pHullShader);
		break;
	case TessellationEvaluationShader:
		hr = m_pDevice->CreateDomainShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pDomainShader);
		break;
	case GeometryShader:
		hr = m_pDevice->CreateGeometryShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pGeometryShader);
		break;
	case PixelShader:
		hr = m_pDevice->CreatePixelShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
		break;
	case ComputeShader:
		hr = m_pDevice->CreateComputeShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader);
		break;
	default:
		ReportError("Invalid shader type.");
		break;
	}

	if (FAILED(hr))
	{
		ReportError("Failed to create shader.");
		return false;
	}

	// Reflect the parameters from the shader.
	// Inspired by: http://members.gamedev.net/JasonZ/Heiroglyph/D3D11ShaderReflection.pdf
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
	hr = D3DReflect(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);

	if (FAILED(hr))
	{
		ReportError("Failed to reflect shader.");
		return false;
	}

	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription;
	hr = pReflector->GetDesc(&shaderDescription);

	if (FAILED(hr))
	{
		ReportError("Failed to get shader description from shader reflector.");
		return false;
	}

	m_InputSemantics.clear();

	UINT numInputParameters = shaderDescription.InputParameters;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	for (UINT i = 0; i < numInputParameters; ++i)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement;
		D3D11_SIGNATURE_PARAMETER_DESC parameterSignature;

		pReflector->GetInputParameterDesc(i, &parameterSignature);

		inputElement.SemanticName = parameterSignature.SemanticName;
		inputElement.SemanticIndex = parameterSignature.SemanticIndex;
		inputElement.InputSlot = i; // TODO: If using interleaved arrays, then the input slot should be 0.  If using packed arrays, the input slot will vary.
		inputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO: Figure out how to deal with per-instance data? .. Don't. Just use structured buffers to store per-instance data and use the SV_InstanceID as an index in the structured buffer.
		inputElement.InstanceDataStepRate = 0;
		inputElement.Format = GetDXGIFormat(parameterSignature);

		assert(inputElement.Format != DXGI_FORMAT_UNKNOWN);

		inputElements.push_back(inputElement);

		m_InputSemantics.insert(SemanticMap::value_type(BufferBinding(inputElement.SemanticName, inputElement.SemanticIndex), i));
	}

	if (inputElements.size() > 0)
	{
		hr = m_pDevice->CreateInputLayout(inputElements.data(), (UINT)inputElements.size(), m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), &m_pInputLayout);
		if (FAILED(hr))
		{
			ReportError("Failed to create input layout.");
			return false;
		}
	}

	// Query Resources that are bound to the shader.
	for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		pReflector->GetResourceBindingDesc(i, &bindDesc);
		std::string resourceName = bindDesc.Name;

		ShaderParameter::Type parameterType = ShaderParameter::Type::Invalid;

		switch (bindDesc.Type)
		{
		case D3D_SIT_TEXTURE:
			parameterType = ShaderParameter::Type::Texture;
			break;
		case D3D_SIT_SAMPLER:
			parameterType = ShaderParameter::Type::Sampler;
			break;
		case D3D_SIT_CBUFFER:
		case D3D_SIT_STRUCTURED:
			parameterType = ShaderParameter::Type::Buffer;
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			parameterType = ShaderParameter::Type::RWBuffer;
			break;
		case D3D_SIT_UAV_RWTYPED:
			parameterType = ShaderParameter::Type::RWTexture;
			break;
		}

		// Create an empty shader parameter that should be filled-in by the application.
		std::shared_ptr<ShaderParameterDX11> shaderParameter = std::make_shared<ShaderParameterDX11>(resourceName, bindDesc.BindPoint, shaderType, parameterType);
		m_ShaderParameters.insert(ParameterMap::value_type(resourceName, shaderParameter));

	}

	// Now try to restore the original shader parameters (if there were any)
	for (auto shaderParameter : shaderParameters)
	{
		ParameterMap::iterator iter = m_ShaderParameters.find(shaderParameter.first);
		if (iter != m_ShaderParameters.end())
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

	const std::string pathName = "Assets\\Shader\\" + fileName;

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