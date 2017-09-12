#include "Shader.h"
#include "stdafx.h"
#include <d3dcompiler.h>
#include <cwchar>
#include <vector>

Shader::Shader():mType(SHADERTYPE_COUNT), mShader(NULL)
{

}
bool Shader::load(char* filepath, char* entryPoint, ShaderType type)
{
	std::wstring strins;
	strins.resize(256);
	WCHAR wsz[256];
	swprintf(wsz,256, L"%S", filepath);
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ThrowIfFailed(D3DCompileFromFile(wsz, nullptr, nullptr, entryPoint, Targetchars[type], compileFlags, 0, &mShader, nullptr));
	
	ThrowIfFailed(D3DReflect(mShader->GetBufferPointer(), mShader->GetBufferSize(), IID_PPV_ARGS(&mReflector)));

	ThrowIfFailed(mReflector->GetDesc(&mShaderDesc));
	//D3D12_SIGNATURE_PARAMETER_DESC inputdesc;
	//for (int i = 0; i < mShaderDesc.InputParameters; i++)
	//{
	//	
	//	mReflector->GetInputParameterDesc(i,&inputdesc);
	//}
	mType = type;
	return true;
	
}
std::vector<D3D12_INPUT_ELEMENT_DESC> Shader::getInputElements(VertexInputLayOutType layout)
{


	std::vector<D3D12_INPUT_ELEMENT_DESC> inputs;
	D3D12_SIGNATURE_PARAMETER_DESC inputdesc;
	D3D12_INPUT_ELEMENT_DESC inputelemt;
	unsigned int byteoffset = 0;
	UINT slot = 0;
	string indstancename("SV_InstanceID");
	string indexname("SV_VertexID");
	string primitivename("SV_PrimitiveID");
	for (int i = 0; i < mShaderDesc.InputParameters; i++)
	{
		
		mReflector->GetInputParameterDesc(i,&inputdesc);
		inputelemt.SemanticName = inputdesc.SemanticName;
		string t(inputelemt.SemanticName);
		if (t == indstancename || t == indexname || t == primitivename)
			continue;
	//	if(inputelemt.SemanticName == L"Test String")
		inputelemt.SemanticIndex = inputdesc.SemanticIndex;
		inputelemt.InputSlot = slot;
		if (layout == VERTEX_LAYOUT_TYPE_SPLIT_ALL)  // all input is in differnt buffer, byteoffset is always zero and change slot
		{
			++slot;
			byteoffset = 0;
		}
		inputelemt.AlignedByteOffset = byteoffset;
		inputelemt.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelemt.InstanceDataStepRate = 0;
		switch (inputdesc.Mask)
		{
		case 1 :
			if(inputdesc.ComponentType== D3D_REGISTER_COMPONENT_UINT32) inputelemt.Format = DXGI_FORMAT_R32_UINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputelemt.Format = DXGI_FORMAT_R32_SINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputelemt.Format = DXGI_FORMAT_R32_FLOAT;
			byteoffset += 4;
			inputs.push_back(inputelemt);
			break;
		case 3:
			if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputelemt.Format = DXGI_FORMAT_R32G32_UINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputelemt.Format = DXGI_FORMAT_R32G32_SINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputelemt.Format = DXGI_FORMAT_R32G32_FLOAT;
			byteoffset += 8;
			inputs.push_back(inputelemt);
			break;
		case 7:
			if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputelemt.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputelemt.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputelemt.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			byteoffset += 12;
			inputs.push_back(inputelemt);
			break;
		case 15:
			if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputelemt.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputelemt.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (inputdesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputelemt.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			byteoffset += 16;
			inputs.push_back(inputelemt);
			break;
		}
		
	}

	return inputs;
}