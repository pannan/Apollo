#pragma once

#include "Environment/Atmosphere/Reference/Definitions.h"

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

/*
���ļ���������CPU��ʹ�����ǵĴ���ģ�͵�API�� Ҫʹ������

	1������һ�������������������Modelʵ�����Լ�һ�����Ի���Ԥ���������Ŀ¼��
	2������InitԤ�ȼ������������������Ѿ�Ԥ�ȼ��㣬��ӻ���Ŀ¼�ж�ȡ���ǣ���
	3��������Ҫ����GetSolarRadiance��GetSkyRadiance��GetSkyRadianceToPoint��GetSunAndSkyIrradiance��
	4������������Ҫ��ʱɾ������ģ�ͣ������������ڴ���ɾ��Ԥ�ȼ����������
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