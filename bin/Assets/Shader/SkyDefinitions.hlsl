/*
定义6个基本物理单位
米(m)，纳米(nm),弧度(rad)，球面度(sr)，瓦(watt)，流明(lm)
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
从中我们可以推导出某些派生物理量的单位，以及一些衍生单位（公里km，kilocandela kcd，度deg）
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
大气层参数

使用上面的类型，我们可以定义大气层模型的参数。
我们由定义density profiles开始，它的参数依赖海拔：
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
一个大气层desity profile由几个层组成（从下到上）。最有一层的width被忽略，因为它总是延生到大气层的顶部边界。
*/
struct DensityProfile
{
	DensityProfileLayer layers[2];
};

struct AtmosphereParameters 
{
	// The solar irradiance at the top of the atmosphere.
	//在大气层顶部的太阳辐射度(单位面积的)
	IrradianceSpectrum solar_irradiance;
	// The sun's angular radius. Warning: the implementation uses approximations
	// that are valid only if this angle is smaller than 0.1 radians.
	//太阳角度半径。Warming:这是个近似值，只有当角度小于0.1弧度时有效。
	Angle sun_angular_radius;
	//地心到大气层底部边界的距离,在这里是地球半径
	Length bottom_radius;
	//地心到大气层顶部边界的距离
	Length top_radius;
	//空气分子的density profile
	//从0（零密度）和1（最大密度）的关于高度的无量纲值函数。
	DensityProfile rayleigh_density;
	// The scattering coefficient of air molecules at the altitude where their
	// density is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The scattering coefficient at altitude h is equal to
	// 'rayleigh_scattering' times 'rayleigh_density' at this altitude.
	/*
	在空子分子密度最大的地方的散射系数（一般在大气层的底部），是一个关于波长的函数。
	在海拔h的散射系数 = rayleigh_scattering * rayleigh_density
	*/
	ScatteringSpectrum rayleigh_scattering;
	// The density profile of aerosols, i.e. a function from altitude to
	// dimensionless values between 0 (null density) and 1 (maximum density).
	//气溶胶的density profile
	//从0（零密度）和1（最大密度）的关于高度的无量纲值函数。
	DensityProfile mie_density;
	// The scattering coefficient of aerosols at the altitude where their density
	// is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The scattering coefficient at altitude h is equal to
	// 'mie_scattering' times 'mie_density' at this altitude.
	/*
	在气溶胶密度最大的地方的散射系数（一般在大气层的底部），是一个关于波长的函数。
	在海拔h的散射系数 = mie_scattering * mie_density
	*/
	ScatteringSpectrum mie_scattering;
	// The extinction coefficient of aerosols at the altitude where their density
	// is maximum (usually the bottom of the atmosphere), as a function of
	// wavelength. The extinction coefficient at altitude h is equal to
	// 'mie_extinction' times 'mie_density' at this altitude.
	/*
	在气溶胶密度最大的地方的消光系数（一般在大气层的底部），是一个关于波长的函数。
	在海拔h的消光系数 = mie_extinction * mie_density
	*/
	ScatteringSpectrum mie_extinction;
	// The asymetry parameter for the Cornette-Shanks phase function for the
	// aerosols.
	//关于气溶胶Cornette-Shanks相位函数的不对称参数
	Number mie_phase_function_g;
	// The density profile of air molecules that absorb light (e.g. ozone), i.e.
	// a function from altitude to dimensionless values between 0 (null density)
	// and 1 (maximum density).
	//空气分子（比如臭氧）吸收光的density profile
	//从0（零密度）和1（最大密度）的关于高度的无量纲值函数
	DensityProfile absorption_density;
	// The extinction coefficient of molecules that absorb light (e.g. ozone) at
	// the altitude where their density is maximum, as a function of wavelength.
	// The extinction coefficient at altitude h is equal to
	// 'absorption_extinction' times 'absorption_density' at this altitude.
	/*
	在空气分子密度最大的地方的消光系数（比如臭氧），是一个关于波长的函数。
	在海拔h的消光系数 = absorption_extinction * absorption_density
	*/
	ScatteringSpectrum absorption_extinction;
	// The average albedo of the ground.
	//地表的平均albedo
	DimensionlessSpectrum ground_albedo;
	// The cosine of the maximum Sun zenith angle for which atmospheric scattering
	// must be precomputed (for maximum precision, use the smallest Sun zenith
	// angle yielding negligible sky light radiance values. For instance, for the
	// Earth case, 102 degrees is a good choice - yielding mu_s_min = -0.2).
	/*
	必须预先计算大气散射的最大太阳天顶角的余弦
	（为了获得最大精度，使用最小的太阳天顶角产生可忽略不计的天光辐射值。例如，对于地球情况，102度是一个不错的选择 - 产生mu_s_min = -0.2）。
	*/
	Number mu_s_min;
};