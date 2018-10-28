#pragma once

#include "Environment/Atmosphere/Reference/Definitions.h"

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

/*
该文件定义了在CPU上使用我们的大气模型的API。 要使用它：

	1）创建一个具有所需大气参数的Model实例，以及一个可以缓存预计算纹理的目录，
	2）调用Init预先计算大气纹理（如果它们已经预先计算，则从缓存目录中读取它们），
	3）根据需要调用GetSolarRadiance，GetSkyRadiance，GetSkyRadianceToPoint和GetSunAndSkyIrradiance，
	4）当您不再需要它时删除您的模型（析构函数从内存中删除预先计算的纹理）。
*/

class Model 
{
public:
	Model(const AtmosphereParameters& atmosphere,const std::string& cache_directory);

	void Init(unsigned int num_scattering_orders = 4);

	RadianceSpectrum GetSolarRadiance() const;

	RadianceSpectrum GetSkyRadiance(Position camera, Direction view_ray,
		Length shadow_length, Direction sun_direction,
		DimensionlessSpectrum* transmittance) const;

	RadianceSpectrum GetSkyRadianceToPoint(Position camera, Position point,
		Length shadow_length, Direction sun_direction,
		DimensionlessSpectrum* transmittance) const;

	IrradianceSpectrum GetSunAndSkyIrradiance(Position p, Direction normal,
		Direction sun_direction, IrradianceSpectrum* sky_irradiance) const;


private:
	const AtmosphereParameters atmosphere_;
	const std::string cache_directory_;
	std::unique_ptr<TransmittanceTexture> transmittance_texture_;
	std::unique_ptr<ReducedScatteringTexture> scattering_texture_;
	std::unique_ptr<ReducedScatteringTexture> single_mie_scattering_texture_;
	std::unique_ptr<IrradianceTexture> irradiance_texture_;
};

NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END