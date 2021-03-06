#include "stdafx.h"
#include "GPUNoiseTextureGenerator.h"
#define NoiseTextureBLockSize 16u

GPUNoiseTextureGenerator::GPUNoiseTextureGenerator()
{

}
GPUNoiseTextureGenerator::~GPUNoiseTextureGenerator() {

}
void GPUNoiseTextureGenerator::initialize(Render& render)
{

	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters.resize(2);
	
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[0].mType = PARAMETERTYPE_ROOTCONSTANT;
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[0].mBindSlot = 0;
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[0].mResCounts = 5;
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[1].mType = PARAMETERTYPE_UAV; 
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[1].mBindSlot = 0;
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].mParameters[1].mResCounts = 1;
	mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY].initialize(render.mDevice);

	std::string shaderpath("Shaders/GraphicsUtility/");
	ShaderSet noiseshaders[GPU_NOISE_TEXTURE_TYPE_COUNT];

	for (int i = 0; i < GPU_NOISE_TEXTURE_TYPE_COUNT; ++i)
	{
		noiseshaders[i].shaders[CS].load((shaderpath + NoiseTextureShadersName[i]).c_str(), "CSMain", CS);
		mNoisePipeline[i].createComputePipeline(render.mDevice, mNoiseRootSignature[i], noiseshaders[i]);
	}
}
void GPUNoiseTextureGenerator::release()
{
	for (int i = 0; i < GPU_NOISE_TEXTURE_TYPE_COUNT; ++i)
	{
		mNoisePipeline[i].release();
		mNoiseRootSignature[i].realease();
	}
}
void GPUNoiseTextureGenerator::recordPureGenerateWorleyNoise2D(CommandList &recordlist, Texture& texture,bool reverse, float scale, float time, unsigned int offsetx, unsigned int offsety, int miplevel)
{
	if (texture.mSRVType != TEXTURE_SRV_TYPE_2D && texture.textureDesc.DepthOrArraySize != 1)
		return;
	NoiseConstData data;
	data.offsetx = offsetx;
	data.offsety = offsety;
	data.scale = scale;
	data.time = time;
	data.reverse = reverse;
	recordlist.bindComputeRootSigature(mNoiseRootSignature[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY]);
	recordlist.bindPipeline(mNoisePipeline[GPU_NOISE_TEXTURE_TYPE_2D_WORLEY]);
	recordlist.bindComputeConstant(0,&data);
	recordlist.bindComputeResource(1, texture, miplevel);
	unsigned int width = texture.mLayouts[miplevel].Footprint.Width; 
	unsigned int height = texture.mLayouts[miplevel].Footprint.Height;
	recordlist.dispatch((width + NoiseTextureBLockSize - 1) / NoiseTextureBLockSize, (height + NoiseTextureBLockSize - 1) / NoiseTextureBLockSize, 1);
}