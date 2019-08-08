#include "stdafx.h"
#include "Environment/Atmosphere/Reference/Model.h"
#include "Environment/Atmosphere/Reference/Functions.h"
using namespace std;

/*
该文件在CPU上实现了我们的大气模型。
它的主要作用是预先计算透射率，散射和辐照度纹理。 
functions.h中提供了预计算它们的C ++函数，但它们还不够。 
必须在预先计算的纹理的每个纹素上调用它们，并且必须以正确的顺序计算这些纹理，并使用正确的输入输出纹理，以按顺序预先计算每个散射顺序，如本文算法4.1中所述。 
这是以下代码的作用。
*/

NAME_SPACE_BEGIN_APOLLO
NAME_SPACE_BEGIN_ATMOSPHERE
NAME_SPACE_BEGIN_REFERENCE

Model::Model(const AtmosphereParameters& atmosphere,const std::string& cache_directory) 
	: atmosphere_(atmosphere),
	cache_directory_(cache_directory) 
{
	transmittance_texture_.reset(new TransmittanceTexture());
	scattering_texture_.reset(new ReducedScatteringTexture());
	single_mie_scattering_texture_.reset(new ReducedScatteringTexture());
	irradiance_texture_.reset(new IrradianceTexture());
}

//# define TryLoadData

//初始化是通过以下方法完成的，如果它们已经预先计算，它首先尝试从磁盘加载纹理。
void Model::Init(unsigned int num_scattering_orders) 
{
#ifdef TryLoadData
	std::ifstream file;
	file.open(cache_directory_ + "transmittance.dat");
	if (file.good())
	{
		file.close();
		transmittance_texture_->Load(cache_directory_ + "transmittance.dat");
		scattering_texture_->Load(cache_directory_ + "scattering.dat");
		single_mie_scattering_texture_->Load(
			cache_directory_ + "single_mie_scattering.dat");
		irradiance_texture_->Load(cache_directory_ + "irradiance.dat");
		return;
	}
#endif

	/*
	由于计算阶段需要几分钟，因此我们会显示一个进度条以向用户提供反馈。
	以下常数粗略地表示每个计算阶段的相对持续时间，并用于显示与经过时间大致成比例的进度值。
	*/

	//constexpr unsigned int kTransmittanceProgress = 1;
	//constexpr unsigned int kDirectIrradianceProgress = 1;
	//constexpr unsigned int kSingleScatteringProgress = 10;
	//constexpr unsigned int kScatteringDensityProgress = 100;
	//constexpr unsigned int kIndirectIrradianceProgress = 10;
	//constexpr unsigned int kMultipleScatteringProgress = 10;
	//const unsigned int kTotalProgress =
	//TRANSMITTANCE_TEXTURE_WIDTH * TRANSMITTANCE_TEXTURE_HEIGHT *
	//kTransmittanceProgress +
	//IRRADIANCE_TEXTURE_WIDTH * IRRADIANCE_TEXTURE_HEIGHT * (
	//	kDirectIrradianceProgress +
	//	kIndirectIrradianceProgress * (num_scattering_orders - 1)) +
	//	SCATTERING_TEXTURE_WIDTH * SCATTERING_TEXTURE_HEIGHT *
	//	SCATTERING_TEXTURE_DEPTH * (
	//		kSingleScatteringProgress +
	//		(kScatteringDensityProgress + kMultipleScatteringProgress) *
	//		(num_scattering_orders - 1));

	//ProgressBar progress_bar(kTotalProgress);

	//原始代码使用了多线程，我们这里使用单线程

	/*
	如果它们尚未预先计算，我们必须在此计算它们。
	此计算需要一些临时纹理，特别是存储一个散射顺序的贡献，这是计算下一个散射顺序所需的（最终预计算纹理存储所有散射顺序的总和）。
	我们在这里分配这些纹理（它们在此方法结束时自动销毁）。
	*/
	std::unique_ptr<IrradianceTexture> delta_irradiance_texture(new IrradianceTexture());
	std::unique_ptr<ReducedScatteringTexture> delta_rayleigh_scattering_texture(new ReducedScatteringTexture());
	ReducedScatteringTexture* delta_mie_scattering_texture = single_mie_scattering_texture_.get();
	std::unique_ptr<ScatteringDensityTexture> delta_scattering_density_texture(new ScatteringDensityTexture());
	std::unique_ptr<ScatteringTexture> delta_multiple_scattering_texture(new ScatteringTexture());

	//透射率纹理
	for (unsigned int j = 0; j < TRANSMITTANCE_TEXTURE_HEIGHT; ++j)
	{
		for (unsigned int i = 0; i < TRANSMITTANCE_TEXTURE_WIDTH; ++i)
		{
			transmittance_texture_->Set(i, j, ComputeTransmittanceToTopAtmosphereBoundaryTexture(atmosphere_, float2(i + 0.5, j + 0.5)));
		}
	}

	transmittance_texture_->Save(cache_directory_ + "transmittance.dat");

	//计算直接辐射度，存储在delta_irradiance_texture
	//用0初始化irradiance_texture（我们想要的直接辐射度不在irradiance_Texture里，只是来自天空的辐射度）
	for (unsigned int j = 0; j < IRRADIANCE_TEXTURE_HEIGHT; ++j)
	{
		for (unsigned int i = 0; i < IRRADIANCE_TEXTURE_WIDTH; ++i)
		{
			delta_irradiance_texture->Set(i, j, ComputeDirectIrradianceTexture(atmosphere_, *transmittance_texture_, float2(i + 0.5, j + 0.5)));
			irradiance_texture_->Set(i, j, IrradianceSpectrum(0.0 * watt_per_square_meter_per_nm));
		}
	}

	//计算rayleigh和Mie单次散射并储存它们在delta_rayleigh_scatting_texture和delta_mie_scatting_texture还有scatting_texture
	for (unsigned int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
	{
		for (unsigned int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
		{
			for (unsigned int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
			{
				IrradianceSpectrum rayleigh;
				IrradianceSpectrum mie;
				ComputeSingleScatteringTexture(atmosphere_, *transmittance_texture_, float3(i + 0.5, j + 0.5, k + 0.5), rayleigh, mie);
				delta_rayleigh_scattering_texture->Set(i, j, k, rayleigh);
				delta_mie_scattering_texture->Set(i, j, k, mie);
				scattering_texture_->Set(i, j, k, rayleigh);
			}
		}
	}

	//依次计算2阶，3阶，4阶的散射
	for (unsigned int scattering_order = 2; scattering_order <= num_scattering_orders; ++scattering_order)
	{
		//计算散射密度并储存在delta_scattting_density_texture
		for (unsigned int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
		{
			for (unsigned int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
			{
				for (unsigned int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
				{
					RadianceDensitySpectrum scattering_density;
					scattering_density = ComputeScatteringDensityTexture(atmosphere_,
						*transmittance_texture_, *delta_rayleigh_scattering_texture,
						*delta_mie_scattering_texture,
						*delta_multiple_scattering_texture, *delta_irradiance_texture,
						float3(i + 0.5, j + 0.5, k + 0.5), scattering_order);
					delta_scattering_density_texture->Set(i, j, k, scattering_density);
				}
			}
		}

		//计算间接辐射度，储存在delta_irradiance_texture并在irradiance_texture_中累加
		for (unsigned int j = 0; j < IRRADIANCE_TEXTURE_HEIGHT; ++j)
		{
			for (unsigned int i = 0; i < IRRADIANCE_TEXTURE_WIDTH; ++i)
			{
				IrradianceSpectrum delta_irradiance;
				delta_irradiance = ComputeIndirectIrradianceTexture(
					atmosphere_, *delta_rayleigh_scattering_texture,
					*delta_mie_scattering_texture, *delta_multiple_scattering_texture,
					float2(i + 0.5, j + 0.5), scattering_order - 1);

				delta_irradiance_texture->Set(i, j, delta_irradiance);
			}
		}

		(*irradiance_texture_) += *delta_irradiance_texture;

		//计算多次散射，并储存在delta_multiple_scattering_texture,并且在scattering_texture_中累加
		for (unsigned k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
		{
			for (unsigned int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
			{
				for (unsigned int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
				{
					RadianceSpectrum delta_multiple_scattering;
					Number nu;
					delta_multiple_scattering = ComputeMultipleScatteringTexture(
						atmosphere_, *transmittance_texture_,
						*delta_scattering_density_texture,
						float3(i + 0.5, j + 0.5, k + 0.5), nu);

					delta_multiple_scattering_texture->Set(i, j, k, delta_multiple_scattering);
					scattering_texture_->Set(i, j, k, scattering_texture_->Get(i, j, k) + delta_multiple_scattering * (1.0 / RayleighPhaseFunction(nu)));
				}
			}
		}
	}

	
	scattering_texture_->Save(cache_directory_ + "scattering.dat");
	single_mie_scattering_texture_->Save(cache_directory_ + "single_mie_scattering.dat");
	irradiance_texture_->Save(cache_directory_ + "irradiance.dat");
}

/*
一旦从缓存计算或加载纹理，它们就可用于计算天空辐射度和太阳和天空辐照度。 
这样做的函数在functions.h中提供，我们只需要将它们包装在相应的方法中（除了可以直接从模型参数计算的太阳辐射）：
*/
RadianceSpectrum Model::GetSolarRadiance() const
{
	SolidAngle sun_solid_angle = 2.0 * PI * (1.0 - cos(atmosphere_.sun_angular_radius)) * sr;
	return atmosphere_.solar_irradiance * (1.0 / sun_solid_angle);
}

RadianceSpectrum Model::GetSkyRadiance(Position camera, Direction view_ray,
	Length shadow_length, Direction sun_direction,
	DimensionlessSpectrum* transmittance) const 
{
	return Apollo::Atmosphere::Reference::GetSkyRadiance(atmosphere_, *transmittance_texture_,
		*scattering_texture_, *single_mie_scattering_texture_,
		camera, view_ray, shadow_length, sun_direction, *transmittance);
}

RadianceSpectrum Model::GetSkyRadianceToPoint(Position camera, Position point,
	Length shadow_length, Direction sun_direction,
	DimensionlessSpectrum* transmittance) const 
{
	return Apollo::Atmosphere::Reference::GetSkyRadianceToPoint(atmosphere_, *transmittance_texture_,
		*scattering_texture_, *single_mie_scattering_texture_,
		camera, point, shadow_length, sun_direction, *transmittance);
}

IrradianceSpectrum Model::GetSunAndSkyIrradiance(Position point,
	Direction normal, Direction sun_direction,
	IrradianceSpectrum* sky_irradiance) const 
{
	return Apollo::Atmosphere::Reference::GetSunAndSkyIrradiance(atmosphere_, *transmittance_texture_,
		*irradiance_texture_, point, normal, sun_direction, *sky_irradiance);
}


NAME_SPACE_END
NAME_SPACE_END
NAME_SPACE_END