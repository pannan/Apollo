

Number ClampCosine(Number mu)
{
	return clamp(mu, Number(-1.0), Number(1.0));
}

Length ClampDistance(Length d) 
{
	return max(d, 0.0 * m);
}

Length ClampRadius(int AtmosphereParameters atmosphere, Length r) 
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
*/
Length DistanceToTopAtmosphereBoundary(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.top_radius * atmopshere.top_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
现在需要考虑另一种情况，当[p,i]和地面相交时，可以用相似的方式计算
*/
Length DistanceToBottomAtmosphereBoundary(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
判断[P,i]是否与地面相交
由上面的公式得出，当和地面相交时有等式
d*d + 2*r*d*mu + r*r = Rb*Rb
d = sqrt( Rb*Rb + r*r * (mu*mu - 1)) - r*mu
这需要Rb*Rb + r*r * (mu*mu - 1) > 0
由此我们推导出
*/

bool RayIntersectsGround(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//只有当mu <0才可能和地面相交
	return mu < 0.0 && atmopshere.bottom_radius * atmopshere.bottom_radius + r*r * (mu*mu - 1.0) >= 0.0;
}


/*
Beer-Lambert law
T = exp(-r)
r : optical depth（光学深度）
我们现在可以计算p和i之间的透射率。根据Beer-Lambert law，会涉及沿着线段[p,i]上的空气分子的密度的数值积分，以及气溶胶密度的数值积分和
吸收光（比如臭氧）的空气分子的密度的数值积分。当线段[p,i]不和地面相交时，这个3个积分有一样的形式（exp(-r)），它们可以再接下来的一些辅助
函数（使用trapezoidal rule）的帮助下进行数值计算。
*/
Number GetLayerDensity(in DesityProfileLayer layer, Length altitude)
{
	Number density = layer.exp_term * exp(layer.exp_scale * altitude) + layer.linear_term * altitude + layer.constant_term;
	return clamp(density, Number(0.0), Number(1.0));
}

Number GetProfileDensity(in DensityProfile profile, Length altitude)
{
	return altitude < profile.layers[0].width ?
		GetLayerDensity(profile.layers[0], altitude) :
		GetLayerDensity(profile.layers[1], altitude);
}

Length ComputeOpticalLengthToTopAtmosphereBoundary(in AtmosphereParameters atmosphere, in DensityProfile profile, Length r, Number mu)
{
	//数值积分的间隔数量
	const int SAMPLE_COUNT = 500;
	//积分步长，每个积分间隔的长度
	Length dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / Number(SAMPLE_COUNT);
	//积分循环
	Length result = 0.0;
	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = dx * i;
		//当前采样点到地心的距离
		//根据||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R ->R = sqrt(d*d + 2*r*d*mu + r*r)
		Length r_i = sqrt(d_i*d_i + 2 * d_i*mu*r + r*r);
		//当前采样点的密度（用大气层底部的密度来除得到一个无量纲的数(指没有单位的物流量)）
		Number y_i = GetProfileDensity(profile, r_i - atmosphere.bottom_radius);
		//采样权重
		Number weight_i = i == 0 || i == SAMPLE_COUNT ? 0.5 : 1.0;
		result += y_i * weight * dx;
	}

	return result;
}

/*
p和i之间的透射率现在很容易计算了（我们继续假设线段没有和地面相交）
*/
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundary(in AtmosphereParameters atmosphere, Length r, Number mu)
{
	Number opticalLength_rayleigh = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.rayleigh_density, r, mu);
	Number opticalLength_mie = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.mie_density, r, mu);
	Number opticalLength_absorption = ComputeOpticalLengthToTopAtmosphereBoundary(atmosphere, atmosphere.absorption_density, r, mu);
	
	opticalLength_rayleigh *= atmosphere.rayleigh_scattering;
	opticalLength_mie *= atmosphere.mie_extinction;
	opticalLength_absorption *= atmosphere.absorption_extinction;

	Number opticalLength = opticalLength_rayleigh + opticalLength_mie + opticalLength_absorption;

	return exp(-opticalLength);
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

float2 GetTransmittanceTextureUvFromRMu(in AtmosphereParameters atmosphere, Length r, Number mu)
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
	Number v = GetTextureCoordFromUnitRange(x_r, TRANSMITTANCE_TEXTURE_WIDTH);
	return float2(u, v);
}

//上面函数的逆过程
void GetRMuFromTransmittanceTextureUv(in AtmosphereParameters atmosphere, in float2 uv, out Length r, out Number mu)
{
	Number x_mu = GetUintRangeFromTextureCoord(uv.x, TRANSMITTANCE_TEXTURE_WIDTH);
	Number x_r = GetUintRangeFromTextureCoord(uv.y, TRANSMITTANCE_TEXTURE_WIDTH);

	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	Length K = H * x_r;
	r = sqrt(K * K + atmosphere.bottom_radius * atmosphere.bottom_radius);

	Length d_min = atmosphere.top_radius - r;
	Length d_max = H + K;
	Length d = x_mu * (d_max - d_min) + d_min;
	//从上面知道，d_min是mu = 1时出现，当d == 0时，mu = 1
	mu = d == 0.0 ? Number(1.0) : (H * H - K * K - d * d) / (2.0 * r * d);
	mu = ClampCosine(mu);
}

//现在可以用一个函数预计算透射率纹理
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(in AtmosphereParameters atmosphere, in float2 uv)
{
	const float2 TRANSMITTANCE_TEXTURE_SIZE = float2(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);
	Length r;
	Number mu;
	GetRMuFromTransmittanceTextureUv(atmosphere, gl_frag_coord / TRANSMITTANCE_TEXTURE_SIZE, r, mu);
	return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
查找
在上面预计算纹理的帮助下，我们现在可以用单次纹理查询得到点和顶部大气层边界的透射率（假设view ray没有和地面相交）
*/
DimensionlessSpectrum GetTransmittanceToTopAtmosphereBoundary(in AtmosphereParameters atmosphere,
	in TransmittanceTexture transmittance_texture, Length r, Number mu)
{
	float2 uv = GetTransmittanceTextureUvFromRMu(atmosphere, r, mu);
	return DimensionlessSpectrum(transmittance_texture.Sample(TransmittanceTexture_Sampler, uv));
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

DimensionlessSpectrum GetTransmittance(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu, Length d, bool ray_r_mu_intersects_ground)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * r*mu*d + r*r));
	Number mu_d = ClampCosine((r * mu + d) / r_d);

	if (ray_r_mu_intersects_ground)
	{
		Number Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
		Number Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
		Number Tpq = Tpi / Tqi;
		return min(Tpq, DimensionlessSpectrum(1.0));
	}
	else
	{
		Number Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu);
		Number Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, mu_d);
		Number Tpq = Tpi / Tqi;
		return min(Tpq, DimensionlessSpectrum(1.0));
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
DimensionlessSpectrum GetTransmittanceToSun(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu_s)
{
	//sin_theta_h = sin(Rh)
	//在地面上有一点p，有||op|| = r,
	//从P点向地面做切线，切点为e,||oe|| = rbottom = 地球半径
	//oep组成直角三角形，直角为oep,斜边为[o,p]
	//Rh为[pe]和[p,o的夹角]
	// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
	Number sin_theta_h = atmosphere.bottom_radius / r;
	Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h), 0.0);
	

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
	Number sin_theat_h_sun_angular = sin_theta_h * atmosphere.sun_angular_radius;
	Number Fraction = smoothstep(-sin_theat_h_sun_angular, sin_theat_h_sun_angular, mu_s - cons_theta_h);
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
void ComputeSingleScattingIntegrand(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu, Length d,bool ray_r_mu_intersects_ground,
	out DimensionlessSpectrum rayleigh,out DimensionlessSpectrum mie)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * d*r*mu + r*r));
	Number mu_s_d = ClampCosine((r*mu_s + nu*d) / r_d);

	//p,q之间的透射率
	DimensionlessSpectrum Tpq = GetTransmittance(atmosphere, transmittance_texture, r, mu, d, ray_r_mu_intersects_ground);
	//q和太阳之间的透射率
	DimensionlessSpectrum Tsun_q = GetTransmittanceToSun(atmosphere, transmittance_texture, r_d, mu_s_d);

	//太阳到q,然后q到p的透射率
	DimensionlessSpectrum Tsun_q_p = Tsun_q * Tpq;

	rayleigh = Tsun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
	mie = Tsun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
}

/*
在完成一次散射后，太阳光从方向W达到p。（也就是视线方向view ray为W,dot(W,op) = mu,这里op归一化了）
散射发生在p和half-line[p,W)和最近的大气层边界交点i之间的任意点q。（线段[p,i]之间的任意点q）
因此，在p点方向W上的单次散射radiance是在线段p和i上所有点q到p的单次散射radiance的积分。
为了计算它，我们首先需要||pi||长度
*/

Length DistanceToNearestAtmosphereBoundary(in AtmosphereParameters atmosphere, Length r, Number mu, bool ray_r_mu_intersects_ground)
{
	if (ray_r_mu_intersects_ground)
		return DistanceToBottomAtmosphereBoundary(atmosphere, r, mu);
	else
		return DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
单次散射的积分
*/

void ComputeSingleScatteringin(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu,bool ray_r_mu_intersects_ground,
	out IrradianceSpectrum rayleigh,out IrradianceSpectrum mie)
{
	//数值积分的间隔数（采样数）
	const int SAMPLE_COUNT = 50;
	//采样点的间隔长度
	Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);

	DimensionlessSpectrum raylegh_sum = DimensionlessSpectrum(0.0);
	DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);

	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = Number(i) * dx;

		DimensionlessSpectrum rayleigh_i;
		DimensionlessSpectrum mie_i;

		ComputeSingleScatteringIntegrand(atmosphere, transmittance_texture, r, mu, mu_s, nu, d_i, ray_r_mu_interescts_ground, rayleigh_i, mie_i);

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
	return k * (1.0 = nu*nu) / pow(1.0 + g*g - 2.0*g*nu, 1.5);
}