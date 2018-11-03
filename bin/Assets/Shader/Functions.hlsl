#define OpticalLengthSampleCount 50//500

Number ClampCosine(Number mu)
{
	return clamp(mu, Number(-1.0), Number(1.0));
}

Length ClampDistance(Length d) 
{
	return max(d, 0.0 * m);
}

Length ClampRadius(_IN(AtmosphereParameters) atmosphere, Length r) 
{
	return clamp(r, atmosphere.bottom_radius, atmosphere.top_radius);
}

Length SafeSqrt(Area a) 
{
	return sqrt(max(a, 0.0 * m2));
}


/*
                          ----------
			    -----         |        ------                 
	  -----					|					--i----
----                          | cos(a)   q            ------atmosphere
				------      p
					| ------ |-------
			---	|			|      -----
	-----			r			|           -----
--					|			|                 -------earth
			-----------   O

有三个在一条线上的三个点p，q，i，点p，i之间的透过率是p，q之间的透过率和q，i之间透过率的乘积。
也就是T(p,i) = T(p,q) * T(q,i)
从上面可以推导出T(p,q) = T(p,i) / T(q,i)
另外，p，q和q，p之间的透过率相同  T(p,q) = T(q,p)

因此，为了计算任意两点的透射率，需要满足知道在大气层里点X和在大气层边缘上点i之间的透射率。
（
这里的意思是，要知道在大气层里任意两点p，q之间的透射率T(p,q)，需要知道沿着p，q发射射线得到和大气层边缘交点i，这样计算T(p,i)T(q,i)
就可以计算T(p,q) = T(p,i) / T(q,i)
）

透射率值依赖两个参数，点到地球中心的半径r=||op||和视线和向量op之间的夹角（view zenith angle）的cos值。
mu = cos(a) = op * pi / （||op|| * ||pi||) = dot(op,pi)（dot需要归一化向量）

为了计算这个，首先要计算长度||pi||，并且需要知道线段[p,i]是否和地面相交
*/


/*
点到顶部大气层边缘的距离
从点p沿着[p,i)距离d的点的坐标为[d*sqrt(1 - mu * mu),r + d*mu)
这里mu=cos(view zenith angle),所以sqrt(1 - mu * mu) = sin(view zenith angle)
d * sin(view zenith angle)=线段[p,i]在xz屏幕的长度
r + d *mu= 线段[O,i]投影到[O,p]的长度

由于i是在一个圆上，所以可以计算[O,i]的长度平方为：
pow(d*sqrt(1 - mu * mu),2) + pow(r + d*mu,2) = 
d*d * (1 - mu*mu) + r*r + d*d * mu*mu + 2 * r * d * mu = 
d*d - d*d * mu*mu + r*r + d*d + d*d * mu*mu + 2 * r * d * mu =
d*d + 2 *r *d * mu + r*r

因此，根据i的定义，我们有||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R	//R = 大气层边缘半径
从上面要导出||pi||

根据pow((a + b),2) = a*a + 2 * a * b + b*b
pow(d + r*mu,2) = d*d + 2*d*r*mu + r*r*mu*mu

d*d + 2*r*d*mu + r*r = R*R ->
pow(d + r*mu,2) - r*r*mu*mu + r*r = R*R
pow(d + r*mu,2) = R*R + r*r*mu*mu - r*r
d + r*mu = sqrt(R*R + r*r*mu*mu - r*r)
d = sqrt(R*R + r*r*mu*mu - r*r) - r*mu
d = sqrt(R*R + r*r * (mu*mu - 1)) - r*mu
注意上面的sqrt(R*R + r*r * (mu*mu - 1))可能为正为负
当和地面相交时为负
*/
Length DistanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.top_radius * atmopshere.top_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
现在需要考虑另一种情况，当[p,i]和地面相交时，可以用相似的方式计算
*/
Length DistanceToBottomAtmosphereBoundary(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius;
	//注意这里的SafeSqrt(discriminant)为负
	return ClampDistance(-r * mu - SafeSqrt(discriminant));
}

/*
判断[P,i]是否与地面相交
由上面的公式得出，当和地面相交时有等式
d*d + 2*r*d*mu + r*r = Rb*Rb
d = sqrt( Rb*Rb + r*r * (mu*mu - 1)) - r*mu
这需要Rb*Rb + r*r * (mu*mu - 1) > 0
由此我们推导出
*/

bool RayIntersectsGround(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//只有当mu <0才可能和地面相交
	return mu < 0.0 && r*r * (mu*mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius >= 0.0 * m2;
}


/*
Beer-Lambert law
T = exp(-r)
r : optical depth（光学深度）
我们现在可以计算p和i之间的透射率。根据Beer-Lambert law，会涉及沿着线段[p,i]上的空气分子的密度的数值积分，以及气溶胶密度的数值积分和
吸收光（比如臭氧）的空气分子的密度的数值积分。当线段[p,i]不和地面相交时，这个3个积分有一样的形式（exp(-r)），它们可以再接下来的一些辅助
函数（使用trapezoidal rule）的帮助下进行数值计算。
*/
Number GetLayerDensity(_IN(DensityProfileLayer) layer, Length altitude)
{
	Number density = layer.exp_term * exp(layer.exp_scale * altitude) + layer.linear_term * altitude + layer.constant_term;
	return clamp(density, Number(0.0), Number(1.0));
}

Number GetProfileDensity(_IN(DensityProfile) profile, Length altitude)
{
	return altitude < profile.layers[0].width ?
		GetLayerDensity(profile.layers[0], altitude) :
		GetLayerDensity(profile.layers[1], altitude);
}

Length ComputeOpticalLengthToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, _IN(DensityProfile) profile, Length r, Number mu)
{
	//数值积分的间隔数量
	const int SAMPLE_COUNT = OpticalLengthSampleCount;
	//积分步长，每个积分间隔的长度
	Length dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / Number(SAMPLE_COUNT);
	//积分循环
	Length result = 0.0 * m;
	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = dx * i;
		//当前采样点到地心的距离
		//根据||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R ->R = sqrt(d*d + 2*r*d*mu + r*r)
		Length r_i = sqrt(d_i*d_i + 2 * d_i*mu*r + r*r);
		//当前采样点的密度（用大气层底部的密度来除得到一个无量纲的数(指没有单位的物理量)）
		Number y_i = GetProfileDensity(profile, r_i - atmosphere.bottom_radius);
		//采样权重
		Number weight_i = i == 0 || i == SAMPLE_COUNT ? 0.5 : 1.0;
		result += y_i * weight_i * dx;
	}

	return result;
}

/*
p和i之间的透射率现在很容易计算了（我们继续假设线段没有和地面相交）
*/
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, Length r, Number mu)
{
	Length opticalLength_rayleigh = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.rayleigh_density, r, mu);
	Length opticalLength_mie = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.mie_density, r, mu);
	Length opticalLength_absorption = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.absorption_density, r, mu);
	
	/*opticalLength_rayleigh *= atmosphere.rayleigh_scattering;
	opticalLength_mie *= atmosphere.mie_extinction;
	opticalLength_absorption *= atmosphere.absorption_extinction;*/
	/*ScatteringSpectrum rayleigh = atmosphere.rayleigh_scattering * opticalLength_rayleigh;
	ScatteringSpectrum mie = atmosphere.mie_extinction * opticalLength_mie;
	ScatteringSpectrum absorption = atmosphere.absorption_extinction * opticalLength_absorption;*/

	//Length opticalLength = opticalLength_rayleigh + opticalLength_mie + opticalLength_absorption;
	//ScatteringSpectrum opticalLength = rayleigh + mie + absorption;
	//return exp(-opticalLength);
	return exp(-(atmosphere.rayleigh_scattering * opticalLength_rayleigh +
		atmosphere.mie_extinction * opticalLength_mie +
		atmosphere.absorption_extinction * opticalLength_absorption));
}

/*
预计算
extrapolated：推断
{
在数学中，推断是在原始观察范围之外，根据与另一个变量的关系估计变量的值的过程。 
它类似于插值，它在已知观测值之间产生估计值，但是外推会受到更大的不确定性和产生无意义结果的更高风险。
}

上述函数的评估成本很高，计算单次和多次散射需要很多的评估。
幸运的是函数只依赖两个参数并且十分平滑（应该是值函数没有高频形状）
我们可以在一个2d纹理里预计算来优化评估

这样我们需要在函数参数(r,mu)和纹理坐标(u,v)之间做一个映射，反之也然，
因为这些参数不具有一致的单位和值范围。
即使是这种情况，在大小为n的纹理中以间隔[0,1]储存函数f将在0.5/n，1.5/n，......(n - 0.5)/nd点上采样函数
因为纹理采样时在texels的中心（所以从0.5开始，以1为间隔）。
因此，在纹理的边界（0和1）只能给出我们推断的函数值（extrapolated function values）。
【上面的意思我理解是：因为前面的采样时从0.5开始，(n-0.5)/n结束，所以在纹理的边界
0和1是没有采样的，所以只能通过推断（插值）来获得值】

为了避免这种情况，
我们需要在texels(纹理元素)0的中心储存f(0)（而不是f(0.5)）,
在纹理元素n-1的中心储存f(1)（而不是f((n-0.5)/n)）。
可以通过以下的映射来完成:
从值x[0,1]到纹理坐标u[0.5/n,(n - 0.5)/n] 和它的inverse
*/

Number GetTextureCoordFromUnitRange(Number x, int texture_size)
{
	//(0.5 - x) / Number(texture_size) + x
	return 0.5 / Number(texture_size) + x * (1.0 - 1.0 / Number(texture_size));
}

Number GetUnitRangeFromTextureCoord(Number u, int texture_size)
{
	return (u - 0.5 / Number(texture_size)) / (1.0 - 1.0 / Number(texture_size));
}

/*
利用这些函数，我们现在可以定义一个(r,mu)和纹理坐标的映射和它的inverse，并避免在纹理查询时的任何extrapolation。
在最初的实现中，这种映射使用了为地球大气状况选择的一些特殊常数。
在这里我们使用一般映射，适用于任何的大气层，但是任然在地平线附近提供更高的采样率。
我们的改进是基于在我们的论文中4D纹理的参数化描述：
对r我们使用一样的映射
对mu的映射有稍微的改进（只考虑view ray不和地面相交的情况）
更进一步，参考到大气层顶部边界的距离d，相对它的最大值和最小值 

----------
        -- t---------
		    |              ---------
dmin	|			            ------------
			|					             --------------
			|								       ---------------
			P									           -------------- 
            |													----------				
 	-------------------------								--------
            |                 ----------------e---						 ------
		    | 			                     ------------							 i-------
		   O		      			                      ----------地面						------ 大气层
								
										

上面所示：
d_min = ||Ot||
d_max = ||pi|| = ||pe|| + ||ei||
p代表相机
O是地心
t是[O,p]延长线和大气层顶部边缘的交点
e是view ray和地面相切的点
i是veiw ray和大气层相交的点
r是p到O的距离：||Op||

view ray和大气层相交的最短距离是 ||pt||
view ray和大气层相交的最长距离是||pe|| + ||ei||

||pe|| = K
||ei|| = H

dmin = rtop - r =  ||Ot|| - r
dmax = K + H
我们映射mu到一个范围为[0,1]的值Xmu

有了这些定义，我们映射(r,mu)到纹理坐标(u,v)可以如下执行:
【
一些思考：
一下情况是p在大气层里的情况：
从上面可知，因为d_max是view ray在于地面相切的射线与大气层边缘相交的距离
所以当p刚好在地面，也就是r = 地球半径时，与地面相交的view ray是水平的
也就是mu = 0时，d最大
但是当r逐渐增大，view ray与地面相切时,mu会 < 0
当p点在大气层顶部边缘时，Ope,Oei组成两个相同等腰三角形
这时d_max是最大的
 】
*/

float2 GetTransmittanceTextureUvFromRMu(_IN(AtmosphereParameters) atmosphere, Length r, Number mu)
{
	//O为地心
	//为了求view ray和大气层距离d的最大值，需要求H和K
	//求||ei||的距离H，因为e是和地球相交切线的交点，所以||Oe||垂直于view ray[p,i]
	//这样(O,e,i)组成一个直角三角形，斜边为[O,i]
	//H = ||ei|| = sqrt(||Oi||*||Oi|| - ||Oe||*||Oe||)
	//而||Oi|| = atmosphere.top_radius
	//||Oe|| = atmosphere.bottom_radius
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	
	//同理(O,p,e)组成一个直角三角形，斜边为[O,p]
	//k = ||pe|| = sqrt(||Op||*||Op|| - ||Oe||*||Oe||)
	//而||Op|| = r
	//||Oe|| = atmosphere.bottom_radius
	Length K = sqrt(r * r - atmosphere.bottom_radius * atmosphere.bottom_radius);

	Length d = DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
	Length d_min = atmosphere.top_radius - r;
	Length d_max = H + K;
	Number x_mu = (d - d_min) / (d_max - d_min);
	/*
	这里的x_r不是直接用r来表示，而是用K/H
	当r = 地球半径时,K = 0 所以 x_r = 0
	当r = 大气层半径时,从上面总结知道
	Ope,Oei组成两个相同等腰三角形，也就是||pe|| = ||ei||
	而||pe|| = K , ||ei|| = H
	所以x_r = 1
	*/
	Number x_r = K / H;

	Number u = GetTextureCoordFromUnitRange(x_mu, TRANSMITTANCE_TEXTURE_WIDTH);
	Number v = GetTextureCoordFromUnitRange(x_r, TRANSMITTANCE_TEXTURE_HEIGHT);
	return float2(u, v);
}

//上面函数的逆过程
void GetRMuFromTransmittanceTextureUv(_IN(AtmosphereParameters) atmosphere, _IN(float2) uv, _OUT(Length) r, _OUT(Number) mu)
{
	Number x_mu = GetUnitRangeFromTextureCoord(uv.x, TRANSMITTANCE_TEXTURE_WIDTH);
	Number x_r = GetUnitRangeFromTextureCoord(uv.y, TRANSMITTANCE_TEXTURE_HEIGHT);

	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	Length K = H * x_r;
	r = sqrt(K * K + atmosphere.bottom_radius * atmosphere.bottom_radius);

	Length d_min = atmosphere.top_radius - r;
	Length d_max = H + K;
	Length d = x_mu * (d_max - d_min) + d_min;
	//从上面知道，d_min是mu = 1时出现，当d == 0时，mu = 1
	mu = d == 0.0 * m ? Number(1.0) : (H * H - K * K - d * d) / (2.0 * r * d);
	mu = ClampCosine(mu);
}

//现在可以用一个函数预计算透射率纹理
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(_IN(AtmosphereParameters) atmosphere, _IN(float2) uv)
{
	const float2 TRANSMITTANCE_TEXTURE_SIZE = float2(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);
	Length r;
	Number mu;
	GetRMuFromTransmittanceTextureUv(atmosphere, uv / TRANSMITTANCE_TEXTURE_SIZE, r, mu);
	return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
查找
在上面预计算纹理的帮助下，我们现在可以用单次纹理查询得到点和顶部大气层边界的透射率（假设view ray没有和地面相交）
*/
DimensionlessSpectrum GetTransmittanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture, Length r, Number mu)
{
	float2 uv = GetTransmittanceTextureUvFromRMu(atmosphere, r, mu);
	//return DimensionlessSpectrum(transmittance_texture.Sample(TransmittanceTexture_Sampler, uv));
#ifdef _HLSL
	return transmittance_texture.Sample(TransmittanceSampler, uv).rgb;
#else
	return DimensionlessSpectrum(texture(transmittance_texture, uv));
#endif
}

/*
				----------
		-----         |        ------
-----					|					--i----
----                  | cos(a)   q            ------atmosphere
		------      p
			| ------ |-------
	---	|			|      -----
-----		r			|           -----
--			|			|                 -------earth
	-----------   O
由上面推导的，在p高度为r，沿着view ray距离为d的点q到地心的距离：
rd = ||Oq|| = sqrt(d*d + 2*d*mu*r + r*r)
mud = oq*pi /||oq||*||pi|| = (r*mu + d) / rd

现在我们需要知道在q点的r和mu: rd , mud
mud是向量[O,q]和[p,i]的夹角
我们假设pi是归一化向量，那么
||oq||*||pi|| = ||Oq||
oq*pi = oq在向量[p,i]上的投影
我们假设oq在[p,i]上的投影点为k
那么Okq组成一个直角三角形，斜边为[O,q],那么得到
oq*pi = r*mu + d,最后得到
mud = oq*pi /||oq||*||pi|| = (r*mu + d) / rd


得到在大气层里任意两点p和q之间的透射率只需要两次纹理查询
重温:
T(p,i) = T(p,q) * T(q,i)
T(p,q) = T(p,i) / T(q,i)
我们继续假设线段[p,q]不和地面相交
*/

DimensionlessSpectrum GetTransmittance(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu, Length d, bool ray_r_mu_intersects_ground)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * r*mu*d + r*r));
	Number mu_d = ClampCosine((r * mu + d) / r_d);

	if (ray_r_mu_intersects_ground)
	{
		//DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
		//DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
		//DimensionlessSpectrum Tqp = Tqi / Tpi;
		//注意，这里因为对mu,mu_d取反了，为什么取反？因为这里假设不和地面相交
		//这里我的理解是这时r_d变成了r:r_d->r
		//r变成了r_d:r->r_d
		//也就是射线是从r_d发出的，所以Tpi和Tqi对换了,所以重写为
		DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
		DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
		DimensionlessSpectrum Tpq = Tpi / Tqi;
#ifdef _HLSL
		return min(Tpq, DimensionlessSpectrum(1.0,1.0,1.0));
#else
		return min(Tpq, DimensionlessSpectrum(1.0));
#endif
	}
	else
	{
		DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu);
		DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, mu_d);
		DimensionlessSpectrum Tpq = Tpi / Tqi;
#ifdef _HLSL
		return min(Tpq, DimensionlessSpectrum(1.0, 1.0, 1.0));
#else
		return min(Tpq, DimensionlessSpectrum(1.0));
#endif
	}
}

/*
计算到太阳的透射率
r和mu定义的射线和地面相交时ray_r_mu_intersects_ground为true。
我们在这里不会用RayIntersectsGround来计算它，由于有限的精度和浮点数操作的rounding error，当射线十分接近地平线时结果可能错误。
并且调用者一般来说有更健壮的方法知道射线是否和地面相交（见下面）

最终，我们需要在大气层里一点和太阳之间的透射率。
太阳不是一个点光源，这是整个太阳圆盘上透射率的积分。
在这里我们认为整个圆盘上的透射率是不变的，除了在地平线以下，这时透射率为0。
作为结果，到太阳的透射率可以用GetTransmittanceToTopAtmosphereBoundary计算，并乘以在地平线上太阳的部分。
= ~表示“约等于”
Fraction的变化从当太阳天顶角度Rs大于地平线天顶角度Rh加上太阳角度半径As的0，到Rs小于Rh - As的1。
等价的，它的变化从当mu_s = cos(Rs) < cos(Rh + As) = ~ cos(Rh) - As*sin(Rh)时的0，
到mu_s > cos(Rh - As) =~ cos(Rh) + As*sin(Rh)时的1
在这之间，太阳圆盘可见部分的近似于smoothstep(这可以通过绘制圆形段的面积作为其sagitta的函数来验证)。
因此，因为sin(Rh) = rbotttom / r,我们可以用下面的函数近似的表示到太阳的透射率：
*/
DimensionlessSpectrum GetTransmittanceToSun(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu_s)
{
	//sin_theta_h = sin(Rh)
	//在地面上有一点p，有||op|| = r,
	//从P点向地面做切线，切点为e,||oe|| = rbottom = 地球半径
	//oep组成直角三角形，直角为oep,斜边为[o,p]
	//Rh为[pe]和[p,o的夹角]
	// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
	Number sin_theta_h = atmosphere.bottom_radius / r;
	Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));
	

	/*
	Rs:太阳天顶角
	Rh:地平线天顶角
	As:太阳的角度半径，就也是太阳半径对应的角度
	由前面知：
	当太阳完全在地平线之下时，有
	Rs > Rh + As  -> 
	cos(Rs) < cos(Rh +Ａs)　＝～cos(Rh) - As*sin(Rh) ->
	cos(Rs) < cos(Rh) - As*sin(Rh) ->
	cos(Rs) - cos(Rh) < -As*sin(Rh)
	也就是当cos(Rs) - cos(Rh) 小于 -As*sin(Rh)时，太阳完全在地平线之下（Fraction = 0）
	同理：
	当太阳完全在地平线之上时：
	Rs < Rh - As ->
	cos(Rs) > cos(Rh - As) ->
	cos(Rs) > cos(Rh) + As*sin(Rh) ->
	cos(Rs) - cos(Rh) > As*sin(Rh)
	也就是当cos(Rs) - cos(Rh) 大于 As*sin(Rh)时，太阳完全在地平线之上（Fraction = 1）
	Fraction = 0 :cos(Rs) - cos(Rh) < -As*sin(Rh)
	Fraction = 1: cos(Rs) - cos(Rh) > As*sin(Rh)
	上面是Fraction在<0 和 >1的范围，
	Fraction在[0,1]时，符号取反,得：
	 -As*sin(Rh) <= cos(Rs) - cos(Rh) <= As*sin(Rh)
	 smoothstep(-As*sin(Rh),As*sin(Rh),cos(Rs) - cos(Rh))
	*/
	Number sin_theat_h_sun_angular = sin_theta_h * atmosphere.sun_angular_radius / rad;
	Number Fraction = smoothstep(-sin_theat_h_sun_angular, sin_theat_h_sun_angular, mu_s - cos_theta_h);
	DimensionlessSpectrum TransmittanceToSun = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu_s);

	return TransmittanceToSun * Fraction;
}

/*
单次散射

单次散射辐射是在大气中恰好一次散射事件之后从太阳到达某些点的光（这可能是由于空气分子或气溶胶粒子;我们排除了来自地面的反射，它单独计算）。
以下部分描述了我们如何计算它，如何将它存储在预先计算的纹理中，以及我们如何读取它。

计算

考虑在达到点p之前，太阳光在点q被空气分子散射的情况（对气溶胶用"Mie"替换“Rayleigh”）

				----------
	-----          |        ------
 -----			 |					--i----
----				 | cos(a)   q            ------atmosphere
		------    p
		 | ------ |-------
 ---	 |			|      -----
-----	r			|           -----
--		|			|                 -------earth
-----------   O

达到p点的radiance的生成：
在大气层顶部的太阳irradiance
太阳和q之间的透射率（一分部顶部大气层太阳光达到q（内散射？））
在q点的Rayleigh散射系数（从任何方向，被散射到q点的光（外散射？））
Rayleigh相位函数（在q点被散射到[q,p]方向上的光）
q和p之间的透射率（在q点沿着[q,p]到达p点的光）

Ws表示太阳的方向，是单位向量 ||Ws|| = 1
r = ||op||
d = ||pq||
mu = (op*pq) / r*d
mu_s = (op*Ws) / (||op||*||Ws||) = (op*Ws) / r 
v = (pq * Ws) / (||pq||*||Ws||) = (pq * Ws) / d

在q点的r和mu_s
rq = ||oq|| = sqrt(d*d + 2*d*r*mu + r*r);(上面推导过)
mu_sd = (oq * Ws) / (||oq|| * ||Ws||) ,因为oq = op + pq ->
mu_sd = ((op + pq) * Ws) / rd = (op*Ws + pq*Ws) / rd = (r*mu_s + d*v) / rd

RayLeigh和Mie单次散射可以像下面这样计算（我们忽略太阳irradiance和相位函数，还有在大气层底部的散射系数，为了效率我们之后加入它们）
*/
//下面的nu是上面的v
void ComputeSingleScatteringIntegrand(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu, Length d,bool ray_r_mu_intersects_ground,
	_OUT(DimensionlessSpectrum) rayleigh,_OUT(DimensionlessSpectrum) mie)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * d*r*mu + r*r));
	Number mu_s_d = ClampCosine((r*mu_s + nu*d) / r_d);

	//q到p的透射率
	DimensionlessSpectrum T_q_p = GetTransmittance(atmosphere, transmittance_texture, r, mu, d, ray_r_mu_intersects_ground);
	//q和太阳之间的透射率
	DimensionlessSpectrum T_sun_q = GetTransmittanceToSun(atmosphere, transmittance_texture, r_d, mu_s_d);

	//太阳到q,然后q到p的透射率
	DimensionlessSpectrum T_sun_q_p = T_sun_q * T_q_p;

	rayleigh = T_sun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
	mie = T_sun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
}

/*
在完成一次散射后，太阳光从方向W达到p。（也就是视线方向view ray为W,dot(W,op) = mu,这里op归一化了）
散射发生在p和half-line[p,W)和最近的大气层边界交点i之间的任意点q。（线段[p,i]之间的任意点q）
因此，在p点方向W上的单次散射radiance是在线段p和i上所有点q到p的单次散射radiance的积分。
为了计算它，我们首先需要||pi||长度
*/

Length DistanceToNearestAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, Length r, Number mu, bool ray_r_mu_intersects_ground)
{
	if (ray_r_mu_intersects_ground)
		return DistanceToBottomAtmosphereBoundary(atmosphere, r, mu);
	else
		return DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
单次散射的积分
*/

void ComputeSingleScattering(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu,bool ray_r_mu_intersects_ground,
	_OUT(IrradianceSpectrum) rayleigh,_OUT(IrradianceSpectrum) mie)
{
	//数值积分的间隔数（采样数）
	const int SAMPLE_COUNT = 50;
	//采样点的间隔长度
	Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);
#ifdef _HLSL
	DimensionlessSpectrum rayleigh_sum = DimensionlessSpectrum(0.0,0.0,0.0);
	DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0,0.0,0.0);
#else
	DimensionlessSpectrum rayleigh_sum = DimensionlessSpectrum(0.0);
	DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);
#endif

	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = Number(i) * dx;

		DimensionlessSpectrum rayleigh_i;
		DimensionlessSpectrum mie_i;

		ComputeSingleScatteringIntegrand(atmosphere, transmittance_texture, r, mu, mu_s, nu, d_i, ray_r_mu_intersects_ground, rayleigh_i, mie_i);

		//采样权重(trapezoidla rule)
		Number weight_i = (i == 0) || (i == SAMPLE_COUNT) ? 0.5 : 1.0;
		rayleigh_sum += rayleigh_i * weight_i;
		mie_sum += mie_i * weight_i;
	}

	rayleigh = rayleigh_sum * dx * atmosphere.solar_irradiance * atmosphere.rayleigh_scattering;
	mie = mie_sum * dx * atmosphere.solar_irradiance * atmosphere.mie_scattering;
}

/*
现在我们把在ComputeSingleScatteringIntegrand中忽略的太阳irradiance和散射参数加上，
但是没有相位函数，为了更好的角度精度我们在render time时再加。
*/

InverseSolidAngle RayleighPhaseFunction(Number nu)
{
	InverseSolidAngle k = 3.0 / (16.0 * PI * sr);
	return k * (1.0 + nu*nu);
}

InverseSolidAngle MiePhaseFunction(Number g, Number nu)
{
	InverseSolidAngle k = 3.0 / (8.0 * PI * sr) * (1.0 - g*g) / (2.0 + g*g);
	return k * (1.0 + nu*nu) / pow(1.0 + g*g - 2.0*g*nu, 1.5);
}

/*
预计算

ComputeSingleScattering函数的评估成本十分高，而计算多次散射需要很多次的评估。
因此我们希望预计算到纹理，这个需要映射函数的4个参数到纹理坐标。
假设现在我们有个4D纹理，我们需要定义一个从(r,mu,mu_s,v)到纹理坐标(u,v,w,z)的映射。
下面的执行函数早我们的纹理里定义，有一些小的改进。

.mu的映射考虑到最近大气层边界的最小距离，映射mu到完整的[0,1]间隔（最初的映射没有覆盖全[0,1]间隔）
.mu_s的映射比论文里的更通用（原始纹理里的映射使用了一个为大气层情况选择的特殊常数）。
 它基于到顶部大气层边界的距离（对太阳ray来说），就像mu的映射，只使用一个特殊参数（可配置的）。
  就像原始定义的，它会在靠近地平线时增加采样。
*/

float4 GetScatteringTextureUvwzFromRMuMuSNu(_IN(AtmosphereParameters) atmosphere, Length r, Number mu,
	Number mu_s, Number nu, bool ray_r_mu_intersects_ground)
{
	//在大气层中一点p对地球做切线，切点e，切线和大气层交点为i,地心为O，Ope为直角三角形，斜边为Op
	//H = ||ei||= sqrt(||Oi||*||Oi|| - ||Oe||*||Oe||) = sqrt(top_radius * top_radius - bottom_radius * bottom_radius)
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);

	//K = ||pe||= sqrt(||Op||*||Op|| - ||Oe||*||Oe||) = sqrt(r * r - bottom_radius * bottom_radius)
	Length K = sqrt(r*r - atmosphere.bottom_radius * atmosphere.bottom_radius);
	
	Number u_r = GetTextureCoordFromUnitRange(K / H, SCATTERING_TEXTURE_R_SIZE);

	//对ray(r,mu)和地面的二次方程的判别（see RayIntersectsGround）
	Area discriminant = atmosphere.bottom_radius * atmosphere.bottom_radius + r*r*(mu*mu - 1.0);
	Number u_mu;
	if (ray_r_mu_intersects_ground)
	{
		//ray(r,mu)到地面的距离，对所有mu的最小和最大值，通过ray(r,-1)和ray(r,m_horizon)获得
		//当前ray(r,mu)和地面相交的d
		//这里有点不明白，d = sqrt(discriminant) - r*mu，但是这里为什么要是-sqrt(discriminant) - r*mu
		//【这里为符号可能和mu有关，因为只有ray和Op角度为大于90度，才能和地面相交。也就是mu<0】
		Length d = -sqrt(discriminant) - r * mu;
		Length d_min = r - atmosphere.bottom_radius;
		Length d_max = K;

		//这里不是映射到[0,1]而是[0,0.5]，因为另一半要留给不和地面相交的情况
		u_mu = 0.5 - 0.5 * GetTextureCoordFromUnitRange(d_max == d_min ? 0.0 : (d - d_min) / (d_max - d_min), SCATTERING_TEXTURE_MU_SIZE / 2);
	}
	else
	{
		//ray(r,mu)和顶部大气层的距离，低所有mu的最小最大值，通过ray(r,1)和ray(r,mu_horizon)获得
		//这里的sqrt(discriminant)符号为正，因为mu>0?,但是不和地面相交也可能mu < 0
		//由于是求和顶部大气层交点距离，所有dis = top_radius*top_radius + r*r*(mu*mu - 1.0)
		//H*H = top_radius*top_radius - bottom_radius*bottom_radius
		//discriminant + H * H = bottom_radius*bottom_radius + r*r(mu*mu - 1) + H*H = top_radius*top_radius + r*r*(mu*mu - 1.0)
		Length d = sqrt(discriminant + H * H) - r*mu;
		Length d_min = atmosphere.top_radius - r;
		Length d_max = K + H;
		u_mu = 0.5 + 0.5 * GetTextureCoordFromUnitRange((d - d_min) / (d_max - d_min), SCATTERING_TEXTURE_MU_SIZE / 2);
	}

	//计算u_mu_s
	//当前mu_s，在地面一点到大气层的距离
	Length d_s = DistanceToTopAtmosphereBoundary(atmosphere, atmosphere.bottom_radius, mu_s);
	//ray(bottom_radius,mu_s)最小最大是 min:ray(bottom,cos(0)),max:ray(bottom,cos(90))
	Length d_min_s = atmosphere.top_radius - atmosphere.bottom_radius;
	Length d_max_s = H;
	Number a = (d_s - d_min_s) / (d_max_s - d_min_s);
	Number A = -2.0 * atmosphere.mu_s_min * atmosphere.bottom_radius / (d_max_s - d_min_s);
	Number u_mu_s = GetTextureCoordFromUnitRange(max(1.0 - a / A, 0.0) / (1.0 + a), SCATTERING_TEXTURE_MU_S_SIZE);
	Number u_nu = (nu + 1.0) / 2.0;

	return float4(u_nu, u_mu_s, u_mu, u_r);
}

//逆过程
void GetRMuMuSNuFromScatteringTextureUvwz(_IN(AtmosphereParameters) atmosphere, _IN(float4) uvwz,
	_OUT(Length) r, _OUT(Number) mu, _OUT(Number) mu_s, _OUT(Number) nu,
	_OUT(bool) ray_r_mu_intersects_ground)
{
	assert(uvwz.x >= 0.0 && uvwz.x <= 1.0);
	assert(uvwz.y >= 0.0 && uvwz.y <= 1.0);
	assert(uvwz.z >= 0.0 && uvwz.z <= 1.0);
	assert(uvwz.w >= 0.0 && uvwz.w <= 1.0);

	//地面（海拔为0）水平射线和顶部大气层的距离
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	Length K = H * GetUnitRangeFromTextureCoord(uvwz.w,SCATTERING_TEXTURE_R_SIZE);
	r = sqrt(K*K + atmosphere.bottom_radius * atmosphere.bottom_radius);
	if (uvwz.z < 0.5)
	{
		//ray(r,mu)到地面的距离，对所有mu的最小和最大值，通过ray(r,-1)和ray(r,m_horizon)获得
		Length d_min = r - atmosphere.bottom_radius;
		Length d_max = K;
		Length d = d_min + (d_max - d_min) * GetUnitRangeFromTextureCoord(1.0 - 2.0 * uvwz.z, SCATTERING_TEXTURE_MU_SIZE / 2);
		mu = d == 0.0*m ? Number(-1.0) : ClampCosine(-(K * K + d * d) / (2.0 * r * d));
		ray_r_mu_intersects_ground = true;
	}
	else
	{
		//ray(r,mu)和顶部大气层的距离，低所有mu的最小最大值，通过ray(r,1)和ray(r,mu_horizon)获得
		Length d_min = atmosphere.top_radius - r;
		Length d_max = K + H;
		Length d = d_min + (d_max - d_min) * GetUnitRangeFromTextureCoord(
			2.0 * uvwz.z - 1.0, SCATTERING_TEXTURE_MU_SIZE / 2);
		mu = d == 0.0*m ? Number(1.0) : ClampCosine((H * H - K * K - d * d) / (2.0 * r * d));
		ray_r_mu_intersects_ground = false;
	}

	Number x_mu_s = GetUnitRangeFromTextureCoord(uvwz.y, SCATTERING_TEXTURE_MU_S_SIZE);
	Length d_min_s = atmosphere.top_radius - atmosphere.bottom_radius;
	Length d_max_s = H;
	Number A = -2.0 * atmosphere.mu_s_min * atmosphere.bottom_radius / (d_max_s - d_min_s);
	Number a = (A - x_mu_s * A) / (1.0 + x_mu_s * A);
	Length d = d_min_s + min(a, A) * (d_max_s - d_min_s);
	mu_s = d == 0.0*m ? Number(1.0) : ClampCosine((H * H - d * d) / (2.0 * atmosphere.bottom_radius * d));

	nu = ClampCosine(uvwz.x * 2.0 - 1.0);
}

/*
我们上面假设的4D纹理现实中并不存在。因此我们需要进一步在3D和4D纹理之间映射。
下面的函数展开一个3D坐标到一个4D纹理坐标（r,mu,mu_s,v）。
它是通过从x坐标"unpacking"两个坐标来实现。注意我们是怎么样在最后clamp v的。
这是因为v不是完全的独立，它的值范围有赖于mu和mu_s（这可以从zenith,view,sun direction的笛卡尔坐标来计算mu,mu_s,v看出来）
之前的函数隐含地假设这一点（如果不遵守此约束，它们的断言可能会被打破）
*/
void GetRMuMuSNuFromScatteringTextureFragCoord(_IN(AtmosphereParameters) atmosphere, _IN(float3) fragCoord,
	_OUT(Length) r, _OUT(Number) mu, _OUT(Number) mu_s, _OUT(Number) nu, _OUT(bool) ray_r_mu_intersects_ground)
{
	const float4 SCATTERING_TEXTURE_SIZE = float4(SCATTERING_TEXTURE_NU_SIZE - 1,
																					SCATTERING_TEXTURE_MU_S_SIZE,
																					SCATTERING_TEXTURE_MU_SIZE,
																					SCATTERING_TEXTURE_R_SIZE);
	/*
	fragcoord范围是[0,width]不是[0,1]
	在这里需要把nu和mu_s两维映射成一维
	比如一个2D纹理x,y [with,height]映射成1D纹理 x[width * height]
	2d->1d  frag_coord_x = y * width + x 
	1d->2d y = frag_coord_x / width ; x = frag_coord_x % width
	在这里，width = SCATTERING_TEXTURE_MU_S_SIZE
	*/
	Number frag_coord_nu = floor(fragCoord.x / Number(SCATTERING_TEXTURE_MU_S_SIZE));
	Number frag_coord_mu_s = mod(fragCoord.x, Number(SCATTERING_TEXTURE_MU_S_SIZE));
	float4 uvwz = float4(frag_coord_nu, frag_coord_mu_s, fragCoord.y, fragCoord.z) / SCATTERING_TEXTURE_SIZE;

	GetRMuMuSNuFromScatteringTextureUvwz(atmosphere,uvwz,r,mu,mu_s,nu,ray_r_mu_intersects_ground);

	//在给定mu，mu_s的情况下，Clamp nu到它的有效值范围
	Number v0 = sqrt((1.0 - mu*mu) * (1.0 - mu_s*mu_s));
	nu = clamp(nu, mu*mu_s - v0, mu*mu_s + v0);
}

//4D坐标映射带3D坐标
//这个函数原始代码没有，我自己添加
//void GetScatteringTextureFragCoordFromRMuMuSNu(_IN(AtmosphereParameters) atmosphere, _OUT(float3) fragCoord,
//	_IN(Length) r, _IN(Number) mu, _IN(Number) mu_s, _IN(Number) nu, _IN(bool) ray_r_mu_intersects_ground)
//{
//
//}

//有了这个映射，我们可以写一个预计算单次散射的函数
void ComputeSingleScatteringTexture(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture, _IN(float3) fragCoord,
	_OUT(IrradianceSpectrum) rayleigh, _OUT(IrradianceSpectrum) mie)
{
	Length r;
	Number mu;
	Number mu_s;
	Number nu;
	bool ray_r_mu_intersects_ground;

	GetRMuMuSNuFromScatteringTextureFragCoord(atmosphere,fragCoord,r,mu,mu_s,nu,ray_r_mu_intersects_ground);

	ComputeSingleScattering(atmosphere, transmittance_texture, r, mu, mu_s, nu, ray_r_mu_intersects_ground, rayleigh, mie);
}

/*
查找
在上面预计算纹理的帮助下，我们现在可以用两次纹理查询得到一个点和最近的大气层边界之间的散射
（我们需要两个3D纹理的双线性插值来模拟一个4D纹理查询；我们使用在GetRMuMuSNuFromScatteringTextureFragCoord中
定义的映射的逆过程来计算3D纹理坐标）
*/
TEMPLATE(AbstractSpectrum)
AbstractSpectrum GetScattering(_IN(AtmosphereParameters) atmosphere, 
	_IN(AbstractScatteringTexture TEMPLATE_ARGUMENT(AbstractSpectrum))scattering_texture,
	Length r, Number mu, Number mu_s, Number nu, bool ray_r_mu_intersects_ground)
{
	float4 uvwz = GetScatteringTextureUvwzFromRMuMuSNu(atmosphere, r, mu, mu_s, nu, ray_r_mu_intersects_ground);

	Number tex_coord_x = uvwz.x * Number(SCATTERING_TEXTURE_NU_SIZE - 1);
	Number tex_x = floor(tex_coord_x);
	Number lerp = tex_coord_x - tex_x;
	float3 uvw0 = float3((tex_x + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE),uvwz.z, uvwz.w);
	float3 uvw1 = float3((tex_x + 1.0 + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE),uvwz.z, uvwz.w);
#ifdef _HLSL
	return scattering_texture.Sample(ScatteringTextureSampler, uvw0).rgb * (1.0 - lerp) + 
		scattering_texture.Sample(ScatteringTextureSampler, uvw1).rgb * lerp;
#else
	return AbstractSpectrum(texture(scattering_texture, uvw0) * (1.0 - lerp) + texture(scattering_texture, uvw1) * lerp);
#endif
}

/*
最后，我们在这里提供了一个方便查找功能，这将在下一节中有用。 
此函数返回包含相位函数的单个散射，或者第n order散射(n>1)。 
假设，如果scat_order严格大于1，则multiple_scattering_texture对应于此散射order，包括Rayleigh和Mie，以及所有相位函数项。
*/
RadianceSpectrum GetScattering(
	_IN(AtmosphereParameters) atmosphere,
	_IN(ReducedScatteringTexture) single_rayleigh_scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	_IN(ScatteringTexture) multiple_scattering_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground,
	int scattering_order) 
{
	if (scattering_order == 1)
	{
		IrradianceSpectrum rayleigh = GetScattering(atmosphere, single_rayleigh_scattering_texture, r, mu, mu_s, nu,ray_r_mu_intersects_ground);
		IrradianceSpectrum mie = GetScattering(atmosphere, single_mie_scattering_texture, r, mu, mu_s, nu,ray_r_mu_intersects_ground);
		return rayleigh * RayleighPhaseFunction(nu) + mie * MiePhaseFunction(atmosphere.mie_phase_function_g, nu);
	}
	else 
	{
		return GetScattering(atmosphere, multiple_scattering_texture, r, mu, mu_s, nu,ray_r_mu_intersects_ground);
	}
}

//多次散射
/*
多次散射辐射度(radiance)是指在大气层中从太阳经过两次或多次反射后到达某点的太阳光（其中反弹是散射事件或来自地面的反射）。 
以下部分描述了我们如何计算它，如何将它存储在预先计算的纹理中，以及我们如何读取它。

请注意，对于单次散射，我们在此处排除了最后一次反弹是地面反射的光路。 
这些路径的贡献在渲染时单独计算，以便考虑实际的地面反照率（对于地面上的中间反射，这是预先计算的，我们使用平均的均匀反射率）。
*/

//计算
/*
多次散射可以分解为两次散射，三次散射等的总和，其中每个项对应于在正好2,3次等反弹之后在大气中的某个点来自太阳的光。 
而且，每个项可以在前一个项的基础上计算。 
实际上，在n次反弹之后从方向ω到达某点p的光是在上一次反弹的所有可能点q上的积分，其涉及在n-1反弹之后从任何方向到达q的光。

该描述表明，每个散射orfer需要从前一个计算三重积分（在从ω方向上的p到最近大气边界线段上的所有点q上的一个积分，以及在q点上每个方向上的所有方向上的嵌套双积分）。 
因此，如果我们想要“从头开始”计算每个order，我们对两次散射需要三重积分，三次散射需要六重积分等。
由于所有冗余计算（对n order的计算基本就是重复所有之前order的计算，导致对order数量的二次复杂度(O(n*n))），这显然是低效的。 
相反，下面的计算要有效很多:
1) 
预处理单次散射并储存在一个纹理里
2) 
对n>=2
	预计算纹理中的第n个散射，其被积函数的三重积分由在第（n-1）个散射纹理中查找得到

该策略避免了许多冗余计算，但并未消除所有这些计算。 
考虑例如下图中的点p和p'，以及计算在n次反弹之后从方向ω到达这两个点所需的光所需的计算。 
这些计算尤其涉及辐射(radiance)L的评估，该辐射L来自n-1反弹之后的所有方向，在q点被散射到方向-ω：	

p -------p'---------q---->ω

因此，如果我们用如上所述的三重积分计算第n个散射，我们将冗余地计算L（事实上，对所有在p和q之间的点和在-ω方向上大气层边界最近点）。 
为了避免这种情况，从而提高多次散射计算的效率，我们将上述算法细化如下：
1)
预处理单次散射并储存在一个纹理里
2)
对n>=2
	对于每个点q和方向ω，预先计算在q处朝向-ω方向散射的光，这些光来自n-1反弹之后的任何方向（这仅涉及双积分，其被积函数使用（n-1）次散射纹理来查找）
	对于每个点p和方向ω，预先计算n次反弹后来自方向ω的光（这只涉及单个积分，其被积函数使用前一行计算的纹理中的查找）

为了获得完整的算法，我们现在必须指定如何在上面的循环中实现这两个步骤。 这就是我们在本节其余部分所做的工作。

第一步

第一步计算在大气中某个点q处向某个方向-ω散射的辐射(radiance)。 此外，我们假设此散射事件是第n次反弹。
该辐射是所有可能的入射方向ω_i的积分
	入射辐射度(radiance)L_i在第（n - 1）次反射后从方向ω_i到达q的和是:
		(n - 1) order的预计算散射纹理给出一个项
		如果射线ray(q,ω_i)和地面相交于r，则来自n - 1次反弹光路的贡献，其最后的一次反弹位于r，即在地面上
		(根据定义，在我们的逾计算纹理里，这些路径被排除在外，但是在这里我们必须考虑它们，因为地面的反弹之后是q处的反弹。)
		反过来，这些贡献由以下产生:
			q和r之间的透射率
			(平均)地面反射率
			Lambertian BRDF 1/PI
			在n - 2次反弹后的地面辐射(irradiance)接受。我们在下一段解释我们怎么样预计算到纹理。
			现在，我们假设我们能用下面的函数从预计算纹理接受辐射(irradiance)：
*/
IrradianceSpectrum GetIrradiance(_IN(AtmosphereParameters) atmosphere, _IN(IrradianceTexture) irradiance_texture, Length r, Number mu_s);

/*
	在q点的散射系数
	ω和ω_i方向的散射相位函数

这导致了以下实现（其中multiple_scattering_texture应该包含散射的第（n-1）阶
如果n> 2，则illuminiance_texture是在n-2之后在地面上接收的辐照度 ，scattering_order等于n）：
*/
RadianceDensitySpectrum ComputeScatteringDensity(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ReducedScatteringTexture) single_rayleigh_scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	_IN(ScatteringTexture) multiple_scattering_texture,
	_IN(IrradianceTexture) irradiance_texture,
	Length r, Number mu, Number mu_s, Number nu, int scattering_order) 
{
	assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	assert(mu >= -1.0 && mu <= 1.0);
	assert(mu_s >= -1.0 && mu_s <= 1.0);
	assert(nu >= -1.0 && nu <= 1.0);
	assert(scattering_order >= 2);

	// Compute unit direction vectors for the zenith, the view direction omega and
	// and the sun direction omega_s, such that the cosine of the view-zenith
	// angle is mu, the cosine of the sun-zenith angle is mu_s, and the cosine of
	// the view-sun angle is nu. The goal is to simplify computations below.
	float3 zenith_direction = float3(0.0, 0.0, 1.0);
	float3 omega = float3(sqrt(1.0 - mu * mu), 0.0, mu);
	Number sun_dir_x = omega.x == 0.0 ? 0.0 : (nu - mu * mu_s) / omega.x;
	Number sun_dir_y = sqrt(max(1.0 - sun_dir_x * sun_dir_x - mu_s * mu_s, 0.0));
	float3 omega_s = float3(sun_dir_x, sun_dir_y, mu_s);

	const int SAMPLE_COUNT = 16;
	const Angle dphi = pi / Number(SAMPLE_COUNT);
	const Angle dtheta = pi / Number(SAMPLE_COUNT);
#ifdef _HLSL
	RadianceDensitySpectrum rayleigh_mie = RadianceDensitySpectrum(0.0 * watt_per_cubic_meter_per_sr_per_nm,
		0.0 * watt_per_cubic_meter_per_sr_per_nm, 0.0 * watt_per_cubic_meter_per_sr_per_nm);
#else
	RadianceDensitySpectrum rayleigh_mie = RadianceDensitySpectrum(0.0 * watt_per_cubic_meter_per_sr_per_nm);
#endif

	// Nested loops for the integral over all the incident directions omega_i.
	for (int l = 0; l < SAMPLE_COUNT; ++l) 
	{
		Angle theta = (Number(l) + 0.5) * dtheta;
		Number cos_theta = cos(theta);
		Number sin_theta = sin(theta);
		bool ray_r_theta_intersects_ground = RayIntersectsGround(atmosphere, r, cos_theta);

		// The distance and transmittance to the ground only depend on theta, so we
		// can compute them in the outer loop for efficiency.
		Length distance_to_ground = 0.0 * m;
#ifdef _HLSL
		DimensionlessSpectrum transmittance_to_ground = DimensionlessSpectrum(0.0,0.0,0.0);
		DimensionlessSpectrum ground_albedo = DimensionlessSpectrum(0.0,0.0,0.0);
#else
		DimensionlessSpectrum transmittance_to_ground = DimensionlessSpectrum(0.0);
		DimensionlessSpectrum ground_albedo = DimensionlessSpectrum(0.0);
#endif
		if (ray_r_theta_intersects_ground) 
		{
			distance_to_ground = DistanceToBottomAtmosphereBoundary(atmosphere, r, cos_theta);
			transmittance_to_ground = GetTransmittance(atmosphere, transmittance_texture, r, cos_theta,distance_to_ground, true /* ray_intersects_ground */);
			ground_albedo = atmosphere.ground_albedo;
		}

		for (int m = 0; m < 2 * SAMPLE_COUNT; ++m) 
		{
			Angle phi = (Number(m) + 0.5) * dphi;
			float3 omega_i = float3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
			SolidAngle domega_i = (dtheta / rad) * (dphi / rad) * sin(theta) * sr;

			// The radiance L_i arriving from direction omega_i after n-1 bounces is
			// the sum of a term given by the precomputed scattering texture for the
			// (n-1)-th order:
			Number nu1 = dot(omega_s, omega_i);
			RadianceSpectrum incident_radiance = GetScattering(atmosphere,
				single_rayleigh_scattering_texture, single_mie_scattering_texture,
				multiple_scattering_texture, r, omega_i.z, mu_s, nu1,
				ray_r_theta_intersects_ground, scattering_order - 1);

			// and of the contribution from the light paths with n-1 bounces and whose
			// last bounce is on the ground. This contribution is the product of the
			// transmittance to the ground, the ground albedo, the ground BRDF, and
			// the irradiance received on the ground after n-2 bounces.
			float3 ground_normal = normalize(zenith_direction * r + omega_i * distance_to_ground);
			IrradianceSpectrum ground_irradiance = GetIrradiance(atmosphere, irradiance_texture, atmosphere.bottom_radius,dot(ground_normal, omega_s));
			incident_radiance += transmittance_to_ground * ground_albedo * (1.0 / (PI * sr)) * ground_irradiance;

			// The radiance finally scattered from direction omega_i towards direction
			// -omega is the product of the incident radiance, the scattering
			// coefficient, and the phase function for directions omega and omega_i
			// (all this summed over all particle types, i.e. Rayleigh and Mie).
			Number nu2 = dot(omega, omega_i);
			Number rayleigh_density = GetProfileDensity(atmosphere.rayleigh_density, r - atmosphere.bottom_radius);
			Number mie_density = GetProfileDensity(atmosphere.mie_density, r - atmosphere.bottom_radius);
			rayleigh_mie += incident_radiance * (atmosphere.rayleigh_scattering * rayleigh_density * RayleighPhaseFunction(nu2) + 
				atmosphere.mie_scattering * mie_density *
				MiePhaseFunction(atmosphere.mie_phase_function_g, nu2)) *
				domega_i;
		}
	}
	return rayleigh_mie;
}

/*
第二步

计算第n个散射order的第二步是利用前面函数预计算的纹理，计算每个点p和方向ω的radiance
其radiance来自n次反弹后的方向ω

这个radiance是q和p点间所有点和在ω方向上和大气层最近交点的积分:
	1）之前函数预计算的纹理给出一个项，来自n - 1次反弹后的任何方向，在q点被散射到p
	2）p和q直接的透射率

注意这里排除了上一次反弹在地面上的第n次反弹（也就是n-1次的反弹后击中地面，造成第n次反弹）
确实，我们选择在我们的预计算纹理排除这些路径，这样那我们可以在渲染时计算它们，使用实际的地面反射率。

第二部的执行很简单：
*/
RadianceSpectrum ComputeMultipleScattering(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ScatteringDensityTexture) scattering_density_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground) 
{
	assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	assert(mu >= -1.0 && mu <= 1.0);
	assert(mu_s >= -1.0 && mu_s <= 1.0);
	assert(nu >= -1.0 && nu <= 1.0);

	// Number of intervals for the numerical integration.
	const int SAMPLE_COUNT = 50;
	// The integration step, i.e. the length of each integration interval.
	Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);
	// Integration loop.
#ifdef _HLSL
	RadianceSpectrum rayleigh_mie_sum = RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm,
		0.0 * watt_per_square_meter_per_sr_per_nm, 0.0 * watt_per_square_meter_per_sr_per_nm);
#else
	RadianceSpectrum rayleigh_mie_sum = RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm);
#endif
	for (int i = 0; i <= SAMPLE_COUNT; ++i) 
	{
		Length d_i = Number(i) * dx;

		// The r, mu and mu_s parameters at the current integration point (see the
		// single scattering section for a detailed explanation).
		Length r_i = ClampRadius(atmosphere, sqrt(d_i * d_i + 2.0 * r * mu * d_i + r * r));
		Number mu_i = ClampCosine((r * mu + d_i) / r_i);
		Number mu_s_i = ClampCosine((r * mu_s + d_i * nu) / r_i);

		// The Rayleigh and Mie multiple scattering at the current sample point.
		RadianceSpectrum rayleigh_mie_i =
			GetScattering(
				atmosphere, scattering_density_texture, r_i, mu_i, mu_s_i, nu,
				ray_r_mu_intersects_ground) *
			GetTransmittance(
				atmosphere, transmittance_texture, r, mu, d_i,
				ray_r_mu_intersects_ground) *
			dx;
		// Sample weight (from the trapezoidal rule).
		Number weight_i = (i == 0 || i == SAMPLE_COUNT) ? 0.5 : 1.0;
		rayleigh_mie_sum += rayleigh_mie_i * weight_i;
	}
	return rayleigh_mie_sum;
}

/*
预计算

如在计算多次散射的整体算法中所解释的，我们需要预先计算纹理中的每个散射order以在计算下一个order时节省计算。 
并且，为了在纹理中存储函数，我们需要从函数参数到纹理坐标的映射。 
幸运的是，所有散射order都取决于相同的（r，mu，mu_s，nu）参数作为单个散射，因此我们可以简单地重用为单个散射定义的映射。
这立即导致以下简单函数：预先计算每次迭代的第一步和第二步的纹理纹理元素，而不是反弹次数：
*/
RadianceDensitySpectrum ComputeScatteringDensityTexture(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ReducedScatteringTexture) single_rayleigh_scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	_IN(ScatteringTexture) multiple_scattering_texture,
	_IN(IrradianceTexture) irradiance_texture,
	_IN(float3) gl_frag_coord, int scattering_order) 
{
	Length r;
	Number mu;
	Number mu_s;
	Number nu;
	bool ray_r_mu_intersects_ground;
	GetRMuMuSNuFromScatteringTextureFragCoord(atmosphere, gl_frag_coord,
		r, mu, mu_s, nu, ray_r_mu_intersects_ground);
	return ComputeScatteringDensity(atmosphere, transmittance_texture,
		single_rayleigh_scattering_texture, single_mie_scattering_texture,
		multiple_scattering_texture, irradiance_texture, r, mu, mu_s, nu,
		scattering_order);
}

RadianceSpectrum ComputeMultipleScatteringTexture(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ScatteringDensityTexture) scattering_density_texture,
	_IN(float3) gl_frag_coord, _OUT(Number) nu) 
{
	Length r;
	Number mu;
	Number mu_s;
	bool ray_r_mu_intersects_ground;
	GetRMuMuSNuFromScatteringTextureFragCoord(atmosphere, gl_frag_coord,
		r, mu, mu_s, nu, ray_r_mu_intersects_ground);
	return ComputeMultipleScattering(atmosphere, transmittance_texture,
		scattering_density_texture, r, mu, mu_s, nu,
		ray_r_mu_intersects_ground);
}

/*
查找

同样，我们可以简单地重用为单次散射实现的查找函数GetScattering，以从预先计算的纹理中读取值以进行多次散射。
实际上，这就是我们在ComputeScatteringDensity和ComputeMultipleScattering函数中所做的。

地面辐照度

地面辐照度是在n≥0反弹之后接收在地面上的太阳光（其中反弹是散射事件或地面反射）。我们需要这个有两个目的：

	1) 当n >= 2 时，为了计算地面上(n - 1)次反弹光路的贡献(需要n - 2次反弹后的地面辐射度)，同时预计算第n-order的散射
	2)在渲染时，计算地面上最后一次反弹光路的贡献（根据定义，这些路径被排除在我们预先计算的散射纹理之外）

在第一种情况下，我们只需要大气底部水平面的地面辐照度（在预计算期间，我们假设一个完美的球形地面和均匀的反照率）。
然而，在第二种情况下，我们需要任何高度和任何表面法线的地面辐照度，我们希望预先计算它以提高效率。
事实上，正如我们的论文所述，我们只对水平表面进行预计算，在任何高度（一般情况下只需要2D纹理，而不是4D纹理），我们对非水平表面使用近似。

以下部分描述了我们如何计算地面辐照度，如何将其存储在预先计算的纹理中，以及我们如何将其读回。

计算

对于直接辐照度，即直接从太阳接收的光，没有任何中间反弹，以及间接辐照度（至少一次反弹），地面辐照度计算是不同的。 
我们从直接辐照度开始。

辐照度是入射辐射半球的积分，乘以余弦因子。 
对于直接地面辐照度，入射辐射是大气顶部的太阳辐射，乘以通过大气的透射率。
并且，由于太阳立体角很小，我们可以用常数近似透射率，即我们可以将它移到辐照度积分之外，这可以在太阳圆盘（而不是半球）的（可见部分）上进行。 
然后积分变得等于由于球体引起的环境遮挡，也称为视角因子，它在辐射视图因子中给出（第10页）。 对于小立体角，这些复杂的方程可以简化如下：
*/
IrradianceSpectrum ComputeDirectIrradiance(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu_s) 
{
	assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	assert(mu_s >= -1.0 && mu_s <= 1.0);

	Number alpha_s = atmosphere.sun_angular_radius / rad;
	// Approximate average of the cosine factor mu_s over the visible fraction of
	// the Sun disc.
	Number average_cosine_factor =
		mu_s < -alpha_s ? 0.0 : (mu_s > alpha_s ? mu_s :
		(mu_s + alpha_s) * (mu_s + alpha_s) / (4.0 * alpha_s));

	return atmosphere.solar_irradiance *
		GetTransmittanceToTopAtmosphereBoundary(
			atmosphere, transmittance_texture, r, mu_s) * average_cosine_factor;
}

/*
对于间接地面辐照度，必须以数字方式计算半球上的积分。 更确切地说，我们需要计算半球的所有方向ω上的积分：

	1)在n次反弹后从方向w达到的辐射
	2)余弦因子 wz

这导致以下实现（其中multiple_scattering_texture应该包含第n级散射，如果n> 1，并且scattering_order等于n）：
*/
IrradianceSpectrum ComputeIndirectIrradiance(
	_IN(AtmosphereParameters) atmosphere,
	_IN(ReducedScatteringTexture) single_rayleigh_scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	_IN(ScatteringTexture) multiple_scattering_texture,
	Length r, Number mu_s, int scattering_order) 
{
	assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	assert(mu_s >= -1.0 && mu_s <= 1.0);
	assert(scattering_order >= 1);

	const int SAMPLE_COUNT = 32;
	const Angle dphi = pi / Number(SAMPLE_COUNT);
	const Angle dtheta = pi / Number(SAMPLE_COUNT);
#ifdef _HLSL
	IrradianceSpectrum result = IrradianceSpectrum(0.0,0.0,0.0);
#else
	IrradianceSpectrum result = IrradianceSpectrum(0.0 * watt_per_square_meter_per_nm);
#endif
	float3 omega_s = float3(sqrt(1.0 - mu_s * mu_s), 0.0, mu_s);
	for (int j = 0; j < SAMPLE_COUNT / 2; ++j) 
	{
		Angle theta = (Number(j) + 0.5) * dtheta;
		for (int i = 0; i < 2 * SAMPLE_COUNT; ++i) 
		{
			Angle phi = (Number(i) + 0.5) * dphi;
			float3 omega = float3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			SolidAngle domega = (dtheta / rad) * (dphi / rad) * sin(theta) * sr;

			Number nu = dot(omega, omega_s);
			result += GetScattering(atmosphere, single_rayleigh_scattering_texture,
				single_mie_scattering_texture, multiple_scattering_texture,
				r, omega.z, mu_s, nu, false /* ray_r_theta_intersects_ground */,
				scattering_order) *
				omega.z * domega;
		}
	}
	return result;
}

/*
预计算

为了预先计算纹理中的地面辐照度，我们需要从地面辐照度参数到纹理坐标的映射。
由于我们仅对水平表面预先计算地面辐照度，因此该辐照度仅取决于r和mu_s，因此我们需要从（r，mu_s）到（u，v）纹理坐标的映射。 
这里最简单的仿射映射就足够了，因为地面辐照度函数非常平滑：
*/
float2 GetIrradianceTextureUvFromRMuS(_IN(AtmosphereParameters) atmosphere,
	Length r, Number mu_s) 
{
	//这里没考虑浮点数误差，r可能为6359.999999999,先屏蔽
	assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	assert(mu_s >= -1.0 && mu_s <= 1.0);
	Number x_r = (r - atmosphere.bottom_radius) / (atmosphere.top_radius - atmosphere.bottom_radius);
	Number x_mu_s = mu_s * 0.5 + 0.5;
	return float2(GetTextureCoordFromUnitRange(x_mu_s, IRRADIANCE_TEXTURE_WIDTH),
		GetTextureCoordFromUnitRange(x_r, IRRADIANCE_TEXTURE_HEIGHT));
}

//逆映射
void GetRMuSFromIrradianceTextureUv(_IN(AtmosphereParameters) atmosphere,
	_IN(float2) uv, _OUT(Length) r, _OUT(Number) mu_s) 
{
	assert(uv.x >= 0.0 && uv.x <= 1.0);
	assert(uv.y >= 0.0 && uv.y <= 1.0);
	Number x_mu_s = GetUnitRangeFromTextureCoord(uv.x, IRRADIANCE_TEXTURE_WIDTH);
	Number x_r = GetUnitRangeFromTextureCoord(uv.y, IRRADIANCE_TEXTURE_HEIGHT);
	r = atmosphere.bottom_radius + x_r * (atmosphere.top_radius - atmosphere.bottom_radius);
	mu_s = ClampCosine(2.0 * x_mu_s - 1.0);
}

/*
现在可以很容易地定义片段着色器函数来预先计算地面辐照度纹理的纹素，用于直接辐照度：
*/
static const float2 IRRADIANCE_TEXTURE_SIZE = float2(IRRADIANCE_TEXTURE_WIDTH, IRRADIANCE_TEXTURE_HEIGHT);

IrradianceSpectrum ComputeDirectIrradianceTexture(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(float2) gl_frag_coord)
{
	Length r;
	Number mu_s;
	GetRMuSFromIrradianceTextureUv(atmosphere, gl_frag_coord / IRRADIANCE_TEXTURE_SIZE, r, mu_s);
	return ComputeDirectIrradiance(atmosphere, transmittance_texture, r, mu_s);
}

//间接的
IrradianceSpectrum ComputeIndirectIrradianceTexture(
	_IN(AtmosphereParameters) atmosphere,
	_IN(ReducedScatteringTexture) single_rayleigh_scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	_IN(ScatteringTexture) multiple_scattering_texture,
	_IN(float2) gl_frag_coord, int scattering_order)
{
	Length r;
	Number mu_s;
	GetRMuSFromIrradianceTextureUv(atmosphere, gl_frag_coord / IRRADIANCE_TEXTURE_SIZE, r, mu_s);
	return ComputeIndirectIrradiance(atmosphere,
		single_rayleigh_scattering_texture, single_mie_scattering_texture,
		multiple_scattering_texture, r, mu_s, scattering_order);
}

/*
查找

由于这些预先计算的纹理，我们现在可以通过单个纹理查找获得地面辐照度：
*/
IrradianceSpectrum GetIrradiance(
	_IN(AtmosphereParameters) atmosphere,
	_IN(IrradianceTexture) irradiance_texture,
	Length r, Number mu_s) 
{
	float2 uv = GetIrradianceTextureUvFromRMuS(atmosphere, r, mu_s);
#ifdef _HLSL
	return irradiance_texture.Sample(IrradianceTextureSampler, uv).rgb;
#else
	return IrradianceSpectrum(texture(irradiance_texture, uv));
#endif
}

/*
渲染

在这里，我们假设透射率，散射和辐照度纹理已经预先计算，我们提供使用它们来计算天空颜色，空中透视和地面辐射的函数。

更准确地说，我们假设没有相位函数项的单次瑞利散射加上多个散射项（除以维度均匀性的瑞利相位函数）存储在scattering_texture中。
我们还假设存储了单次Mie散射，没有相位函数项：

	1）单独或在single_mie_scattering_texture中（此选项在我们的原始实现未提供），
	2）或者，如果定义了COMBINED_SCATTERING_TEXTURES预处理器宏，则在scattering_texture中。
		 在这种仅适用于GLSL编译器的情况下，瑞利和多次散射存储在RGB通道中，并且单次Mie散射的红色分量存储在alpha通道中。

在第二种情况下，单次Mie散射的绿色和蓝色成分按照我们的论文中的描述进行外推，具有以下功能：
*/
#ifdef COMBINED_SCATTERING_TEXTURES
float3 GetExtrapolatedSingleMieScattering(
	IN(AtmosphereParameters) atmosphere, IN(vec4) scattering) 
{
	if (scattering.r == 0.0) {
		return float3(0.0);
	}
	return scattering.rgb * scattering.a / scattering.r *
		(atmosphere.rayleigh_scattering.r / atmosphere.mie_scattering.r) *
		(atmosphere.mie_scattering / atmosphere.rayleigh_scattering);
}
#endif

/*
然后我们可以使用以下函数检索所有散射分量（Rayleigh +一侧多次散射，另一侧是单Mie散射），
基于GetScattering（我们在这里复制一些代码，而不是使用两次GetScattering调用）
确保在scattering_texture和single_mie_scattering_texture中的查找之间共享纹理坐标计算：
*/
IrradianceSpectrum GetCombinedScattering(
	_IN(AtmosphereParameters) atmosphere,
	_IN(ReducedScatteringTexture) scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	Length r, Number mu, Number mu_s, Number nu,
	bool ray_r_mu_intersects_ground,
	_OUT(IrradianceSpectrum) single_mie_scattering) 
{
	float4 uvwz = GetScatteringTextureUvwzFromRMuMuSNu(
		atmosphere, r, mu, mu_s, nu, ray_r_mu_intersects_ground);
	Number tex_coord_x = uvwz.x * Number(SCATTERING_TEXTURE_NU_SIZE - 1);
	Number tex_x = floor(tex_coord_x);
	Number lerp = tex_coord_x - tex_x;
	float3 uvw0 = float3((tex_x + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE),uvwz.z, uvwz.w);
	float3 uvw1 = float3((tex_x + 1.0 + uvwz.y) / Number(SCATTERING_TEXTURE_NU_SIZE),uvwz.z, uvwz.w);
#ifdef COMBINED_SCATTERING_TEXTURES

#ifdef _HLSL
	float4 combined_scattering = scattering_texture.Sample(ScatteringTextureSampler, uvw0) * (1.0 - lerp) +
		scattering_texture.Sample(ScatteringTextureSampler, uvw1) * lerp;
#else
	float4 combined_scattering = texture(scattering_texture, uvw0) * (1.0 - lerp) + texture(scattering_texture, uvw1) * lerp;
#endif
	IrradianceSpectrum scattering = IrradianceSpectrum(combined_scattering);
	single_mie_scattering = GetExtrapolatedSingleMieScattering(atmosphere, combined_scattering);
#else

#ifdef _HLSL
	//irradiance_texture.Sample(IrradianceTextureSampler, uv).rgb;
	IrradianceSpectrum scattering = scattering_texture.Sample(ScatteringTextureSampler, uvw0).rgb * (1.0 - lerp) +
		scattering_texture.Sample(ScatteringTextureSampler, uvw1).rgb * lerp;

	single_mie_scattering = single_mie_scattering_texture.Sample(SingleMieScatteringTextureSampler, uvw0).rgb * (1.0 - lerp) +
		single_mie_scattering_texture.Sample(SingleMieScatteringTextureSampler, uvw1).rgb * lerp;
#else
	IrradianceSpectrum scattering = IrradianceSpectrum(
		texture(scattering_texture, uvw0) * (1.0 - lerp) + texture(scattering_texture, uvw1) * lerp);
	single_mie_scattering = IrradianceSpectrum(
		texture(single_mie_scattering_texture, uvw0) * (1.0 - lerp) + texture(single_mie_scattering_texture, uvw1) * lerp);
#endif

#endif
	return scattering;
}

/*
天空

为了渲染天空，我们只需要显示天空辐射，我们可以通过在预先计算的散射纹理中查找，乘以在预计算期间省略的相位函数项来获得。 
我们还可以返回大气的透射率（我们可以通过预先计算的透射率纹理中的单个查找获得），这是正确渲染空间中的对象（例如太阳和月亮）所需的。 
这导致了以下功能，其中大多数计算用于正确处理大气层外的观察者的情况，以及light shafts的情况：
*/
RadianceSpectrum GetSkyRadiance(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ReducedScatteringTexture) scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	Position camera, _IN(Direction) view_ray, Length shadow_length,
	_IN(Direction) sun_direction, _OUT(DimensionlessSpectrum) transmittance)
{
	// Compute the distance to the top atmosphere boundary along the view ray,
	// assuming the viewer is in space (or NaN if the view ray does not intersect
	// the atmosphere).
	Length r = length(camera);
	Length rmu = dot(camera, view_ray);
	Length distance_to_top_atmosphere_boundary = -rmu -
		sqrt(rmu * rmu - r * r + atmosphere.top_radius * atmosphere.top_radius);
	// If the viewer is in space and the view ray intersects the atmosphere, move
	// the viewer to the top atmosphere boundary (along the view ray):
	if (distance_to_top_atmosphere_boundary > 0.0 * m) {
		camera = camera + view_ray * distance_to_top_atmosphere_boundary;
		r = atmosphere.top_radius;
		rmu += distance_to_top_atmosphere_boundary;
	}
	else if (r > atmosphere.top_radius) 
	{
		// If the view ray does not intersect the atmosphere, simply return 0.
#ifdef _HLSL
		transmittance = DimensionlessSpectrum(1.0,1.0,1.0);
		return RadianceSpectrum(0.0,0.0,0.0);
#else
		transmittance = DimensionlessSpectrum(1.0);
		return RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm);
#endif		
	}
	// Compute the r, mu, mu_s and nu parameters needed for the texture lookups.
	Number mu = rmu / r;
	Number mu_s = dot(camera, sun_direction) / r;
	Number nu = dot(view_ray, sun_direction);
	bool ray_r_mu_intersects_ground = RayIntersectsGround(atmosphere, r, mu);
#ifdef _HLSL
	transmittance = ray_r_mu_intersects_ground ? DimensionlessSpectrum(0.0,0.0,0.0) :
		GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu);
#else
	transmittance = ray_r_mu_intersects_ground ? DimensionlessSpectrum(0.0) :
		GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu);
#endif
	IrradianceSpectrum single_mie_scattering;
	IrradianceSpectrum scattering;
	if (shadow_length == 0.0 * m) 
	{
		scattering = GetCombinedScattering(
			atmosphere, scattering_texture, single_mie_scattering_texture,
			r, mu, mu_s, nu, ray_r_mu_intersects_ground,
			single_mie_scattering);
	}
	else 
	{
		// Case of light shafts (shadow_length is the total length noted l in our
		// paper): we omit the scattering between the camera and the point at
		// distance l, by implementing Eq. (18) of the paper (shadow_transmittance
		// is the T(x,x_s) term, scattering is the S|x_s=x+lv term).
		Length d = shadow_length;
		Length r_p =
			ClampRadius(atmosphere, sqrt(d * d + 2.0 * r * mu * d + r * r));
		Number mu_p = (r * mu + d) / r_p;
		Number mu_s_p = (r * mu_s + d * nu) / r_p;

		scattering = GetCombinedScattering(
			atmosphere, scattering_texture, single_mie_scattering_texture,
			r_p, mu_p, mu_s_p, nu, ray_r_mu_intersects_ground,
			single_mie_scattering);
		DimensionlessSpectrum shadow_transmittance =
			GetTransmittance(atmosphere, transmittance_texture,
				r, mu, shadow_length, ray_r_mu_intersects_ground);
		scattering = scattering * shadow_transmittance;
		single_mie_scattering = single_mie_scattering * shadow_transmittance;
	}
	return scattering * RayleighPhaseFunction(nu) + single_mie_scattering *
		MiePhaseFunction(atmosphere.mie_phase_function_g, nu);
}

/*
空中透视

为了渲染空中透视，我们需要两点之间的透射率和散射（即在观察者和地面上的点之间，其可以在一个高度）。 
我们已经有了计算两点之间透射率的函数（在纹理中使用2次查找仅包含大气顶部的透射率），但我们没有一个用于2点之间的散射。 
希望2点之间的散射可以从包含散射到最近大气边界的纹理中的两个查找来计算，对于透射率（除了这里必须减去两个查找结果，而不是分割）。 
这是我们在以下函数中实现的（初始计算用于正确处理大气层外的查看器的情况）：
*/
RadianceSpectrum GetSkyRadianceToPoint(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(ReducedScatteringTexture) scattering_texture,
	_IN(ReducedScatteringTexture) single_mie_scattering_texture,
	Position camera, _IN(Position) _point, Length shadow_length,
	_IN(Direction) sun_direction, _OUT(DimensionlessSpectrum) transmittance) 
{
	// Compute the distance to the top atmosphere boundary along the view ray,
	// assuming the viewer is in space (or NaN if the view ray does not intersect
	// the atmosphere).
	Direction view_ray = normalize(_point - camera);
	Length r = length(camera);
	Length rmu = dot(camera, view_ray);
	Length distance_to_top_atmosphere_boundary = -rmu -
		sqrt(rmu * rmu - r * r + atmosphere.top_radius * atmosphere.top_radius);
	// If the viewer is in space and the view ray intersects the atmosphere, move
	// the viewer to the top atmosphere boundary (along the view ray):
	if (distance_to_top_atmosphere_boundary > 0.0 * m) 
	{
		camera = camera + view_ray * distance_to_top_atmosphere_boundary;
		r = atmosphere.top_radius;
		rmu += distance_to_top_atmosphere_boundary;
	}

	// Compute the r, mu, mu_s and nu parameters for the first texture lookup.
	Number mu = rmu / r;
	Number mu_s = dot(camera, sun_direction) / r;
	Number nu = dot(view_ray, sun_direction);
	Length d = length(_point - camera);
	bool ray_r_mu_intersects_ground = RayIntersectsGround(atmosphere, r, mu);

	transmittance = GetTransmittance(atmosphere, transmittance_texture,
		r, mu, d, ray_r_mu_intersects_ground);

	IrradianceSpectrum single_mie_scattering;
	IrradianceSpectrum scattering = GetCombinedScattering(
		atmosphere, scattering_texture, single_mie_scattering_texture,
		r, mu, mu_s, nu, ray_r_mu_intersects_ground,
		single_mie_scattering);

	// Compute the r, mu, mu_s and nu parameters for the second texture lookup.
	// If shadow_length is not 0 (case of light shafts), we want to ignore the
	// scattering along the last shadow_length meters of the view ray, which we
	// do by subtracting shadow_length from d (this way scattering_p is equal to
	// the S|x_s=x_0-lv term in Eq. (17) of our paper).
	d = max(d - shadow_length, 0.0 * m);
	Length r_p = ClampRadius(atmosphere, sqrt(d * d + 2.0 * r * mu * d + r * r));
	Number mu_p = (r * mu + d) / r_p;
	Number mu_s_p = (r * mu_s + d * nu) / r_p;

	IrradianceSpectrum single_mie_scattering_p;
	IrradianceSpectrum scattering_p = GetCombinedScattering(
		atmosphere, scattering_texture, single_mie_scattering_texture,
		r_p, mu_p, mu_s_p, nu, ray_r_mu_intersects_ground,
		single_mie_scattering_p);

	// Combine the lookup results to get the scattering between camera and point.
	DimensionlessSpectrum shadow_transmittance = transmittance;
	if (shadow_length > 0.0 * m) 
	{
		// This is the T(x,x_s) term in Eq. (17) of our paper, for light shafts.
		shadow_transmittance = GetTransmittance(atmosphere, transmittance_texture,
			r, mu, d, ray_r_mu_intersects_ground);
	}
	scattering = scattering - shadow_transmittance * scattering_p;
	single_mie_scattering =
		single_mie_scattering - shadow_transmittance * single_mie_scattering_p;
#ifdef COMBINED_SCATTERING_TEXTURES
	single_mie_scattering = GetExtrapolatedSingleMieScattering(
		atmosphere, vec4(scattering, single_mie_scattering.r));
#endif

	// Hack to avoid rendering artifacts when the sun is below the horizon.
	single_mie_scattering = single_mie_scattering *
		smoothstep(Number(0.0), Number(0.01), mu_s);

	return scattering * RayleighPhaseFunction(nu) + single_mie_scattering *
		MiePhaseFunction(atmosphere.mie_phase_function_g, nu);
}

/*
地面

为了渲染地面，我们需要在大气层或地面上0次或更多次弹跳后在地面上接收的辐照度。 
可以通过GetTransmittanceToSun在透射率纹理中查找来计算直接辐照度，
而通过预计算辐照度纹理中的查找给出间接辐照度（该纹理仅包含水平表面的辐照度;我们使用本文中定义的近似值） 对于其他情况）。 
以下功能分别返回直接辐射和间接辐照：
*/
IrradianceSpectrum GetSunAndSkyIrradiance(
	_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture,
	_IN(IrradianceTexture) irradiance_texture,
	_IN(Position) _point, _IN(Direction) normal, _IN(Direction) sun_direction,
	_OUT(IrradianceSpectrum) sky_irradiance)
{
	Length r = length(_point);
	Number mu_s = dot(_point, sun_direction) / r;

	// Indirect irradiance (approximated if the surface is not horizontal).
	sky_irradiance = GetIrradiance(atmosphere, irradiance_texture, r, mu_s) *
		(1.0 + dot(normal, _point) / r) * 0.5;
	//(1.0 + dot(normal, _point) / r) * 0.5 这个是模拟球的环境遮挡（AO）

	// Direct irradiance.
	return atmosphere.solar_irradiance *
		GetTransmittanceToSun(
			atmosphere, transmittance_texture, r, mu_s) *
		max(dot(normal, sun_direction), 0.0);
}