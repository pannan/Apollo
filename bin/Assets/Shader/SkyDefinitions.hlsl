/*
����6����������λ
��(m)������(nm),����(rad)�������(sr)����(watt)������(lm)
*/
const Length m = 1.0;
const Wavelength nm = 1.0;
const Angle rad = 1.0;
const SolidAngle sr = 1.0;
const Power watt = 1.0;
const LuminousPower lm = 1.0;

/*
<p>From which we can derive the units for some derived physical quantities,
as well as some derived units (kilometer km, kilocandela kcd, degree deg):
�������ǿ����Ƶ���ĳЩ�����������ĵ�λ���Լ�һЩ������λ������km��kilocandela kcd����deg��
*/

const float PI = 3.14159265358979323846;

const Length km = 1000.0 * m;
const Area m2 = m * m;
const Volume m3 = m * m * m;
const Angle pi = PI * rad;
const Angle deg = pi / 180.0;
const Irradiance watt_per_square_meter = watt / m2;
const Radiance watt_per_square_meter_per_sr = watt / (m2 * sr);
const SpectralIrradiance watt_per_square_meter_per_nm = watt / (m2 * nm);
const SpectralRadiance watt_per_square_meter_per_sr_per_nm =
watt / (m2 * sr * nm);
const SpectralRadianceDensity watt_per_cubic_meter_per_sr_per_nm =
watt / (m3 * sr * nm);
const LuminousIntensity cd = lm / sr;
const LuminousIntensity kcd = 1000.0 * cd;
const Luminance cd_per_square_meter = cd / m2;
const Luminance kcd_per_square_meter = kcd / m2;

// A generic function from Wavelength to some other type.
#define AbstractSpectrum float3
// A function from Wavelength to Number.
#define DimensionlessSpectrum float3
// A function from Wavelength to SpectralPower.
#define PowerSpectrum float3
// A function from Wavelength to SpectralIrradiance.
#define IrradianceSpectrum float3
// A function from Wavelength to SpectralRadiance.
#define RadianceSpectrum float3
// A function from Wavelength to SpectralRadianceDensity.
#define RadianceDensitySpectrum float3
// A function from Wavelength to ScaterringCoefficient.
#define ScatteringSpectrum float3

#define Length float
#define Wavelength float
#define Angle float
#define SolidAngle float
#define Power float
#define LuminousPower float

#define TransmittanceTexture sampler2D
#define AbstractScatteringTexture sampler3D
#define ReducedScatteringTexture sampler3D
#define ScatteringTexture sampler3D
#define ScatteringDensityTexture sampler3D
#define IrradianceTexture sampler2D



/*
���������

ʹ����������ͣ����ǿ��Զ��������ģ�͵Ĳ�����
�����ɶ���density profiles��ʼ�����Ĳ����������Σ�
*/

// An atmosphere layer of width 'width', and whose density is defined as
//   'exp_term' * exp('exp_scale' * h) + 'linear_term' * h + 'constant_term',
// clamped to [0,1], and where h is the altitude.
struct DensityProfileLayer 
{
	Length width;
	Number exp_term;
	InverseLength exp_scale;
	InverseLength linear_term;
	Number constant_term;
};

// An atmosphere density profile made of several layers on top of each other
// (from bottom to top). The width of the last layer is ignored, i.e. it always
// extend to the top atmosphere boundary. The profile values vary between 0
// (null density) to 1 (maximum density).
/*
һ��������desity profile�ɼ�������ɣ����µ��ϣ�������һ���width�����ԣ���Ϊ������������������Ķ����߽硣
*/
struct DensityProfile
{
	DensityProfileLayer layers[2];
};

struct AtmosphereParameters 
{
	// The solar irradiance at the top of the atmosphere.
	//�ڴ����㶥����̫�������(��λ�����)
	IrradianceSpectrum solar_irradiance;
	// The sun's angular radius. Warning: the implementation uses approximations
	// that are valid only if this angle is smaller than 0.1 radians.
	//̫���ǶȰ뾶��Warming:���Ǹ�����ֵ��ֻ�е��Ƕ�С��0.1����ʱ��Ч��
	Angle sun_angular_radius;
	//���ĵ�������ײ��߽�ľ���,�������ǵ���뾶
	Length bottom_radius;
	//���ĵ������㶥���߽�ľ���
	Length top_radius;
	//�������ӵ�density profile
	//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ������
	DensityProfile rayleigh_density;
	// The scattering coefficient of air molecules at the altitude where their
	// density is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The scattering coefficient at altitude h is equal to
	// 'rayleigh_scattering' times 'rayleigh_density' at this altitude.
	/*
	�ڿ��ӷ����ܶ����ĵط���ɢ��ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
	�ں���h��ɢ��ϵ�� = rayleigh_scattering * rayleigh_density
	*/
	ScatteringSpectrum rayleigh_scattering;
	// The density profile of aerosols, i.e. a function from altitude to
	// dimensionless values between 0 (null density) and 1 (maximum density).
	//���ܽ���density profile
	//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ������
	DensityProfile mie_density;
	// The scattering coefficient of aerosols at the altitude where their density
	// is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The scattering coefficient at altitude h is equal to
	// 'mie_scattering' times 'mie_density' at this altitude.
	/*
	�����ܽ��ܶ����ĵط���ɢ��ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
	�ں���h��ɢ��ϵ�� = mie_scattering * mie_density
	*/
	ScatteringSpectrum mie_scattering;
	// The extinction coefficient of aerosols at the altitude where their density
	// is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The extinction coefficient at altitude h is equal to
	// 'mie_extinction' times 'mie_density' at this altitude.
	/*
	�����ܽ��ܶ����ĵط�������ϵ����һ���ڴ�����ĵײ�������һ�����ڲ����ĺ�����
	�ں���h������ϵ�� = mie_extinction * mie_density
	*/
	ScatteringSpectrum mie_extinction;
	// The asymetry parameter for the Cornette-Shanks phase function for the
	// aerosols.
	//�������ܽ�Cornette-Shanks��λ�����Ĳ��ԳƲ���
	Number mie_phase_function_g;
	// The density profile of air molecules that absorb light (e.g. ozone), i.e.
	// a function from altitude to dimensionless values between 0 (null density)
	// and 1 (maximum density).
	//�������ӣ�������������չ��density profile
	//��0�����ܶȣ���1������ܶȣ��Ĺ��ڸ߶ȵ�������ֵ����
	DensityProfile absorption_density;
	// The extinction coefficient of molecules that absorb light (e.g. ozone) at
	// the altitude where their density is maximum, as a function of wavelength.
	// The extinction coefficient at altitude h is equal to
	// 'absorption_extinction' times 'absorption_density' at this altitude.
	/*
	�ڿ��������ܶ����ĵط�������ϵ�����������������һ�����ڲ����ĺ�����
	�ں���h������ϵ�� = absorption_extinction * absorption_density
	*/
	ScatteringSpectrum absorption_extinction;
	// The average albedo of the ground.
	//�ر��ƽ��albedo
	DimensionlessSpectrum ground_albedo;
	// The cosine of the maximum Sun zenith angle for which atmospheric scattering
	// must be precomputed (for maximum precision, use the smallest Sun zenith
	// angle yielding negligible sky light radiance values. For instance, for the
	// Earth case, 102 degrees is a good choice - yielding mu_s_min = -0.2).
	/*
	����Ԥ�ȼ������ɢ������̫���춥�ǵ�����
	��Ϊ�˻����󾫶ȣ�ʹ����С��̫���춥�ǲ����ɺ��Բ��Ƶ�������ֵ�����磬���ڵ��������102����һ�������ѡ�� - ����mu_s_min = -0.2����
	*/
	Number mu_s_min;
};