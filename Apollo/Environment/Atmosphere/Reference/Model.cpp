#include "stdafx.h"
#include "Environment/Atmosphere/Reference/Model.h"
#include "Environment/Atmosphere/Reference/Functions.h"
using namespace std;

/*
���ļ���CPU��ʵ�������ǵĴ���ģ�͡�
������Ҫ������Ԥ�ȼ���͸���ʣ�ɢ��ͷ��ն����� 
functions.h���ṩ��Ԥ�������ǵ�C ++�����������ǻ������� 
������Ԥ�ȼ���������ÿ�������ϵ������ǣ����ұ�������ȷ��˳�������Щ������ʹ����ȷ��������������԰�˳��Ԥ�ȼ���ÿ��ɢ��˳���籾���㷨4.1�������� 
�������´�������á�
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

//��������Ϊrgb��ʽ
void Model::saveTextureRGB()
{
	constexpr Wavelength kLambdaR = 680.0 * nm;
	constexpr Wavelength kLambdaG = 550.0 * nm;
	constexpr Wavelength kLambdaB = 440.0 * nm;

	//save transmittance_texture_
	int count = transmittance_texture_->size_x() * transmittance_texture_->size_y();
	double* transmittance_buffer = new double[count * 3];
	for (int y = 0; y < transmittance_texture_->size_y(); ++y)
	{
		for (int x = 0; x < transmittance_texture_->size_x(); ++x)
		{
			DimensionlessSpectrum& ds = transmittance_texture_->Get(x,y);
			double R = ds(kLambdaR).to(1);
			double G = ds(kLambdaG).to(1);
			double B = ds(kLambdaB).to(1);

			int index = y * transmittance_texture_->size_x() + x;
			transmittance_buffer[index * 3] = R;
			transmittance_buffer[index * 3 + 1] = G;
			transmittance_buffer[index * 3 + 2] = B;
		}		
	}

	string transmittance_texture_savePath = "f:\\transmittance_texture.dat";
	std::ofstream file0(transmittance_texture_savePath, std::ofstream::binary);
	file0.write(reinterpret_cast<const char*>(transmittance_buffer), count * 3 * sizeof(double));
	file0.close();

	delete[] transmittance_buffer;

	//save irradiance_texture_
	count = irradiance_texture_->size_x() * transmittance_texture_->size_y();
	double* irradiance_buffer = new double[count * 3];
	for (int y = 0; y < irradiance_texture_->size_y(); ++y)
	{
		for (int x = 0; x < irradiance_texture_->size_x(); ++x)
		{
			IrradianceSpectrum&  is = irradiance_texture_->Get(x, y);
			double R = is(kLambdaR).to(watt_per_square_meter_per_nm);
			double G = is(kLambdaG).to(watt_per_square_meter_per_nm);
			double B = is(kLambdaB).to(watt_per_square_meter_per_nm);

			int index = y * irradiance_texture_->size_x() + x;
			irradiance_buffer[index * 3] = R;
			irradiance_buffer[index * 3 + 1] = G;
			irradiance_buffer[index * 3 + 2] = B;
		}
	}
	string irradiance_texture_savePath = "f:\\irradiance_texture.dat";
	std::ofstream file1(irradiance_texture_savePath, std::ofstream::binary);
	file1.write(reinterpret_cast<const char*>(irradiance_buffer), count * 3 * sizeof(double));
	file1.close();
	
	delete[] irradiance_buffer;

	//save scattering_texture_
	count = scattering_texture_->size_x() * scattering_texture_->size_y() * scattering_texture_->size_z();
	double* scattering_buffer = new double[count * 3];
	for (int z = 0; z < scattering_texture_->size_z(); ++z)
	{
		for (int y = 0; y < scattering_texture_->size_y(); ++y)
		{
			for (int x = 0; x < scattering_texture_->size_x(); ++x)
			{
				const IrradianceSpectrum&  is = scattering_texture_->Get(x, y, z);
				double R = is(kLambdaR).to(watt_per_square_meter_per_nm);
				double G = is(kLambdaG).to(watt_per_square_meter_per_nm);
				double B = is(kLambdaB).to(watt_per_square_meter_per_nm);

				int index = (z * scattering_texture_->size_y() * scattering_texture_->size_x() + y * scattering_texture_->size_x() + x);
				scattering_buffer[index * 3] = R;
				scattering_buffer[index * 3 + 1] = G;
				scattering_buffer[index * 3 + 2] = B;
			}
			
		}
	}
	string scattering_texture_savePath = "f:\\scattering_texture.dat";
	std::ofstream file2(scattering_texture_savePath, std::ofstream::binary);
	file2.write(reinterpret_cast<const char*>(scattering_buffer), count * 3 * sizeof(double));
	file2.close();

	delete[] scattering_buffer;

	//save single_mie_scattering_texture_
	count = single_mie_scattering_texture_->size_x() * single_mie_scattering_texture_->size_y() * single_mie_scattering_texture_->size_z();
	double* single_mie_scattering_buffer = new double[count * 3];
	for (int z = 0; z < single_mie_scattering_texture_->size_z(); ++z)
	{
		for (int y = 0; y < single_mie_scattering_texture_->size_y(); ++y)
		{
			for (int x = 0; x < single_mie_scattering_texture_->size_x(); ++x)
			{
				const IrradianceSpectrum&  is = single_mie_scattering_texture_->Get(x, y, z);
				double R = is(kLambdaR).to(watt_per_square_meter_per_nm);
				double G = is(kLambdaG).to(watt_per_square_meter_per_nm);
				double B = is(kLambdaB).to(watt_per_square_meter_per_nm);

				int index = (z * single_mie_scattering_texture_->size_y() * single_mie_scattering_texture_->size_x() + y * single_mie_scattering_texture_->size_x() + x);
				single_mie_scattering_buffer[index * 3] = R;
				single_mie_scattering_buffer[index * 3 + 1] = G;
				single_mie_scattering_buffer[index * 3 + 2] = B;
			}

		}
	}
	string single_mie_scattering_texture_savePath = "f:\\single_mie_scattering_texture.dat";
	std::ofstream file3(single_mie_scattering_texture_savePath, std::ofstream::binary);
	file3.write(reinterpret_cast<const char*>(single_mie_scattering_buffer), count * 3 * sizeof(double));
	file3.close();

	delete[] single_mie_scattering_buffer;
	
}

# define TryLoadData
//#define SaveData
//#define SaveRGBData
//��ʼ����ͨ�����·�����ɵģ���������Ѿ�Ԥ�ȼ��㣬�����ȳ��ԴӴ��̼�������
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
#ifdef SaveRGBData
		saveTextureRGB();
#endif
		return;
	}
#endif

	/*
	���ڼ���׶���Ҫ�����ӣ�������ǻ���ʾһ�������������û��ṩ������
	���³������Եر�ʾÿ������׶ε���Գ���ʱ�䣬��������ʾ�뾭��ʱ����³ɱ����Ľ���ֵ��
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

	//ԭʼ����ʹ���˶��̣߳���������ʹ�õ��߳�

	/*
	���������δԤ�ȼ��㣬���Ǳ����ڴ˼������ǡ�
	�˼�����ҪһЩ��ʱ�����ر��Ǵ洢һ��ɢ��˳��Ĺ��ף����Ǽ�����һ��ɢ��˳������ģ�����Ԥ��������洢����ɢ��˳����ܺͣ���
	���������������Щ���������ڴ˷�������ʱ�Զ����٣���
	*/
	std::unique_ptr<IrradianceTexture> delta_irradiance_texture(new IrradianceTexture());
	std::unique_ptr<ReducedScatteringTexture> delta_rayleigh_scattering_texture(new ReducedScatteringTexture());
	ReducedScatteringTexture* delta_mie_scattering_texture = single_mie_scattering_texture_.get();
	std::unique_ptr<ScatteringDensityTexture> delta_scattering_density_texture(new ScatteringDensityTexture());
	std::unique_ptr<ScatteringTexture> delta_multiple_scattering_texture(new ScatteringTexture());

	//͸��������
	for (unsigned int j = 0; j < TRANSMITTANCE_TEXTURE_HEIGHT; ++j)
	{
		for (unsigned int i = 0; i < TRANSMITTANCE_TEXTURE_WIDTH; ++i)
		{
			transmittance_texture_->Set(i, j, ComputeTransmittanceToTopAtmosphereBoundaryTexture(atmosphere_, float2(i + 0.5, j + 0.5)));
		}
	}	

	//����ֱ�ӷ���ȣ��洢��delta_irradiance_texture
	//��0��ʼ��irradiance_texture��������Ҫ��ֱ�ӷ���Ȳ���irradiance_Texture�ֻ��������յķ���ȣ�
	for (unsigned int j = 0; j < IRRADIANCE_TEXTURE_HEIGHT; ++j)
	{
		for (unsigned int i = 0; i < IRRADIANCE_TEXTURE_WIDTH; ++i)
		{
			delta_irradiance_texture->Set(i, j, ComputeDirectIrradianceTexture(atmosphere_, *transmittance_texture_, float2(i + 0.5, j + 0.5)));
			irradiance_texture_->Set(i, j, IrradianceSpectrum(0.0 * watt_per_square_meter_per_nm));
		}
	}

	//����rayleigh��Mie����ɢ�䲢����������delta_rayleigh_scatting_texture��delta_mie_scatting_texture����scatting_texture
	for (unsigned int k = 0; k < SCATTERING_TEXTURE_DEPTH; ++k)
	{
		for (unsigned int j = 0; j < SCATTERING_TEXTURE_HEIGHT; ++j)
		{
			for (unsigned int i = 0; i < SCATTERING_TEXTURE_WIDTH; ++i)
			{
				IrradianceSpectrum rayleigh;
				IrradianceSpectrum mie;
				ComputeSingleScatteringTexture(atmosphere_, *transmittance_texture_, float3(i + 0.5, j + 0.5, k + 0.5), rayleigh, mie);
				//for debug
				/*constexpr Wavelength kLambdaR = 680.0 * nm;
				constexpr Wavelength kLambdaG = 550.0 * nm;
				constexpr Wavelength kLambdaB = 440.0 * nm;
				double R = mie(kLambdaR).to(watt_per_square_meter_per_nm);
				double G = mie(kLambdaG).to(watt_per_square_meter_per_nm);
				double B = mie(kLambdaB).to(watt_per_square_meter_per_nm);*/
				delta_rayleigh_scattering_texture->Set(i, j, k, rayleigh);
				delta_mie_scattering_texture->Set(i, j, k, mie);
				scattering_texture_->Set(i, j, k, rayleigh);
			}
		}
	}

	//���μ���2�ף�3�ף�4�׵�ɢ��
	for (unsigned int scattering_order = 2; scattering_order <= num_scattering_orders; ++scattering_order)
	{
		//����ɢ���ܶȲ�������delta_scattting_density_texture
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

		//�����ӷ���ȣ�������delta_irradiance_texture����irradiance_texture_���ۼ�
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

		//������ɢ�䣬��������delta_multiple_scattering_texture,������scattering_texture_���ۼ�
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

#ifdef SaveData
	transmittance_texture_->Save(cache_directory_ + "transmittance.dat");
	scattering_texture_->Save(cache_directory_ + "scattering.dat");
	single_mie_scattering_texture_->Save(cache_directory_ + "single_mie_scattering.dat");
	irradiance_texture_->Save(cache_directory_ + "irradiance.dat");
#endif
#ifdef SaveRGBData
	saveTextureRGB();
#endif	
}

/*
һ���ӻ�����������������ǾͿ����ڼ�����շ���Ⱥ�̫������շ��նȡ� 
�������ĺ�����functions.h���ṩ������ֻ��Ҫ�����ǰ�װ����Ӧ�ķ����У����˿���ֱ�Ӵ�ģ�Ͳ��������̫�����䣩��
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