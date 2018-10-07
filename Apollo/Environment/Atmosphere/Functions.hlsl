
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

��������һ�����ϵ�������p��q��i����p��i֮���͸������p��q֮���͸���ʺ�q��i֮��͸���ʵĳ˻���
Ҳ����T(p,i) = T(p,q) * T(q,i)
����������Ƶ���T(p,q) = T(p,i) / T(q,i)
���⣬p��q��q��p֮���͸������ͬ  T(p,q) = T(q,p)

��ˣ�Ϊ�˼������������͸���ʣ���Ҫ����֪���ڴ��������X���ڴ������Ե�ϵ�i֮���͸���ʡ�
��
�������˼�ǣ�Ҫ֪���ڴ���������������p��q֮���͸����T(p,q)����Ҫ֪������p��q�������ߵõ��ʹ������Ե����i����������T(p,i)T(q,i)
�Ϳ��Լ���T(p,q) = T(p,i) / T(q,i)
��

͸����ֵ���������������㵽�������ĵİ뾶r=||op||�����ߺ�����op֮��ļнǣ�view zenith angle����cosֵ��
mu = cos(a) = op * pi / ��||op|| * ||pi||) = dot(op,pi)��dot��Ҫ��һ��������

Ϊ�˼������������Ҫ���㳤��||pi||��������Ҫ֪���߶�[p,i]�Ƿ�͵����ཻ
*/


/*
�㵽�����������Ե�ľ���
�ӵ�p����[p,i)����d�ĵ������Ϊ[d*sqrt(1 - mu * mu),r + d*mu)
����mu=cos(view zenith angle),����sqrt(1 - mu * mu) = sin(view zenith angle)
d * sin(view zenith angle)=�߶�[p,i]��xz��Ļ�ĳ���
r + d *mu= �߶�[O,i]ͶӰ��[O,p]�ĳ���

����i����һ��Բ�ϣ����Կ��Լ���[O,i]�ĳ���ƽ��Ϊ��
pow(d*sqrt(1 - mu * mu),2) + pow(r + d*mu,2) = 
d*d * (1 - mu*mu) + r*r + d*d * mu*mu + 2 * r * d * mu = 
d*d - d*d * mu*mu + r*r + d*d + d*d * mu*mu + 2 * r * d * mu =
d*d + 2 *r *d * mu + r*r

��ˣ�����i�Ķ��壬������||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R	//R = �������Ե�뾶
������Ҫ����||pi||

����pow((a + b),2) = a*a + 2 * a * b + b*b
pow(d + r*mu,2) = d*d + 2*d*r*mu + r*r*mu*mu

d*d + 2*r*d*mu + r*r = R*R ->
pow(d + r*mu,2) - r*r*mu*mu + r*r = R*R
pow(d + r*mu,2) = R*R + r*r*mu*mu - r*r
d + r*mu = sqrt(R*R + r*r*mu*mu - r*r)
d = sqrt(R*R + r*r*mu*mu - r*r) - r*mu
d = sqrt(R*R + r*r * (mu*mu - 1)) - r*mu
ע�������sqrt(R*R + r*r * (mu*mu - 1))����Ϊ��Ϊ��
���͵����ཻʱΪ��
*/
Length DistanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.top_radius * atmopshere.top_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
������Ҫ������һ���������[p,i]�͵����ཻʱ�����������Ƶķ�ʽ����
*/
Length DistanceToBottomAtmosphereBoundary(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius;
	//ע�������SafeSqrt(discriminant)Ϊ��
	return ClampDistance(-r * mu - SafeSqrt(discriminant));
}

/*
�ж�[P,i]�Ƿ�������ཻ
������Ĺ�ʽ�ó������͵����ཻʱ�е�ʽ
d*d + 2*r*d*mu + r*r = Rb*Rb
d = sqrt( Rb*Rb + r*r * (mu*mu - 1)) - r*mu
����ҪRb*Rb + r*r * (mu*mu - 1) > 0
�ɴ������Ƶ���
*/

bool RayIntersectsGround(_IN(AtmosphereParameters) atmopshere, Length r, Number mu)
{
	//ֻ�е�mu <0�ſ��ܺ͵����ཻ
	return mu < 0.0 && r*r * (mu*mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius >= 0.0 * m2;
}


/*
Beer-Lambert law
T = exp(-r)
r : optical depth����ѧ��ȣ�
�������ڿ��Լ���p��i֮���͸���ʡ�����Beer-Lambert law�����漰�����߶�[p,i]�ϵĿ������ӵ��ܶȵ���ֵ���֣��Լ����ܽ��ܶȵ���ֵ���ֺ�
���չ⣨����������Ŀ������ӵ��ܶȵ���ֵ���֡����߶�[p,i]���͵����ཻʱ�����3��������һ������ʽ��exp(-r)�������ǿ����ٽ�������һЩ����
������ʹ��trapezoidal rule���İ����½�����ֵ���㡣
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
	//��ֵ���ֵļ������
	const int SAMPLE_COUNT = 500;
	//���ֲ�����ÿ�����ּ���ĳ���
	Length dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / Number(SAMPLE_COUNT);
	//����ѭ��
	Length result = 0.0 * m;
	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = dx * i;
		//��ǰ�����㵽���ĵľ���
		//����||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R ->R = sqrt(d*d + 2*r*d*mu + r*r)
		Length r_i = sqrt(d_i*d_i + 2 * d_i*mu*r + r*r);
		//��ǰ��������ܶȣ��ô�����ײ����ܶ������õ�һ�������ٵ���(ָû�е�λ��������)��
		Number y_i = GetProfileDensity(profile, r_i - atmosphere.bottom_radius);
		//����Ȩ��
		Number weight_i = i == 0 || i == SAMPLE_COUNT ? 0.5 : 1.0;
		result += y_i * weight_i * dx;
	}

	return result;
}

/*
p��i֮���͸�������ں����׼����ˣ����Ǽ��������߶�û�к͵����ཻ��
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
Ԥ����
extrapolated���ƶ�
{
����ѧ�У��ƶ�����ԭʼ�۲췶Χ֮�⣬��������һ�������Ĺ�ϵ���Ʊ�����ֵ�Ĺ��̡� 
�������ڲ�ֵ��������֪�۲�ֵ֮���������ֵ���������ƻ��ܵ�����Ĳ�ȷ���ԺͲ������������ĸ��߷��ա�
}

���������������ɱ��ܸߣ����㵥�κͶ��ɢ����Ҫ�ܶ��������
���˵��Ǻ���ֻ����������������ʮ��ƽ����Ӧ����ֵ����û�и�Ƶ��״��
���ǿ�����һ��2d������Ԥ�������Ż�����

����������Ҫ�ں�������(r,mu)����������(u,v)֮����һ��ӳ�䣬��֮ҲȻ��
��Ϊ��Щ����������һ�µĵ�λ��ֵ��Χ��
��ʹ������������ڴ�СΪn���������Լ��[0,1]���溯��f����0.5/n��1.5/n��......(n - 0.5)/nd���ϲ�������
��Ϊ�������ʱ��texels�����ģ����Դ�0.5��ʼ����1Ϊ�������
��ˣ�������ı߽磨0��1��ֻ�ܸ��������ƶϵĺ���ֵ��extrapolated function values����
���������˼������ǣ���Ϊǰ��Ĳ���ʱ��0.5��ʼ��(n-0.5)/n����������������ı߽�
0��1��û�в����ģ�����ֻ��ͨ���ƶϣ���ֵ�������ֵ��

Ϊ�˱������������
������Ҫ��texels(����Ԫ��)0�����Ĵ���f(0)��������f(0.5)��,
������Ԫ��n-1�����Ĵ���f(1)��������f((n-0.5)/n)����
����ͨ�����µ�ӳ�������:
��ֵx[0,1]����������u[0.5/n,(n - 0.5)/n] ������inverse
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
������Щ�������������ڿ��Զ���һ��(r,mu)�����������ӳ�������inverse���������������ѯʱ���κ�extrapolation��
�������ʵ���У�����ӳ��ʹ����Ϊ�������״��ѡ���һЩ���ⳣ����
����������ʹ��һ��ӳ�䣬�������κεĴ����㣬������Ȼ�ڵ�ƽ�߸����ṩ���ߵĲ����ʡ�
���ǵĸĽ��ǻ��������ǵ�������4D����Ĳ�����������
��r����ʹ��һ����ӳ��
��mu��ӳ������΢�ĸĽ���ֻ����view ray���͵����ཻ�������
����һ�����ο��������㶥���߽�ľ���d������������ֵ����Сֵ 

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
		   O		      			                      ----------����						------ ������
								
										

������ʾ��
d_min = ||Ot||
d_max = ||pi|| = ||pe|| + ||ei||
p�������
O�ǵ���
t��[O,p]�ӳ��ߺʹ����㶥����Ե�Ľ���
e��view ray�͵������еĵ�
i��veiw ray�ʹ������ཻ�ĵ�
r��p��O�ľ��룺||Op||

view ray�ʹ������ཻ����̾����� ||pt||
view ray�ʹ������ཻ���������||pe|| + ||ei||

||pe|| = K
||ei|| = H

dmin = rtop - r =  ||Ot|| - r
dmax = K + H
����ӳ��mu��һ����ΧΪ[0,1]��ֵXmu

������Щ���壬����ӳ��(r,mu)����������(u,v)��������ִ��:
��
һЩ˼����
һ�������p�ڴ�������������
�������֪����Ϊd_max��view ray���ڵ������е�������������Ե�ཻ�ľ���
���Ե�p�պ��ڵ��棬Ҳ����r = ����뾶ʱ��������ཻ��view ray��ˮƽ��
Ҳ����mu = 0ʱ��d���
���ǵ�r������view ray���������ʱ,mu�� < 0
��p���ڴ����㶥����Եʱ��Ope,Oei���������ͬ����������
��ʱd_max������
 ��
*/

float2 GetTransmittanceTextureUvFromRMu(_IN(AtmosphereParameters) atmosphere, Length r, Number mu)
{
	//OΪ����
	//Ϊ����view ray�ʹ��������d�����ֵ����Ҫ��H��K
	//��||ei||�ľ���H����Ϊe�Ǻ͵����ཻ���ߵĽ��㣬����||Oe||��ֱ��view ray[p,i]
	//����(O,e,i)���һ��ֱ�������Σ�б��Ϊ[O,i]
	//H = ||ei|| = sqrt(||Oi||*||Oi|| - ||Oe||*||Oe||)
	//��||Oi|| = atmosphere.top_radius
	//||Oe|| = atmosphere.bottom_radius
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	
	//ͬ��(O,p,e)���һ��ֱ�������Σ�б��Ϊ[O,p]
	//k = ||pe|| = sqrt(||Op||*||Op|| - ||Oe||*||Oe||)
	//��||Op|| = r
	//||Oe|| = atmosphere.bottom_radius
	Length K = sqrt(r * r - atmosphere.bottom_radius * atmosphere.bottom_radius);

	Length d = DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
	Length d_min = atmosphere.top_radius - r;
	Length d_max = H + K;
	Number x_mu = (d - d_min) / (d_max - d_min);
	/*
	�����x_r����ֱ����r����ʾ��������K/H
	��r = ����뾶ʱ,K = 0 ���� x_r = 0
	��r = ������뾶ʱ,�������ܽ�֪��
	Ope,Oei���������ͬ���������Σ�Ҳ����||pe|| = ||ei||
	��||pe|| = K , ||ei|| = H
	����x_r = 1
	*/
	Number x_r = K / H;

	Number u = GetTextureCoordFromUnitRange(x_mu, TRANSMITTANCE_TEXTURE_WIDTH);
	Number v = GetTextureCoordFromUnitRange(x_r, TRANSMITTANCE_TEXTURE_HEIGHT);
	return float2(u, v);
}

//���溯���������
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
	//������֪����d_min��mu = 1ʱ���֣���d == 0ʱ��mu = 1
	mu = d == 0.0 * m ? Number(1.0) : (H * H - K * K - d * d) / (2.0 * r * d);
	mu = ClampCosine(mu);
}

//���ڿ�����һ������Ԥ����͸��������
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(_IN(AtmosphereParameters) atmosphere, _IN(float2) uv)
{
	const float2 TRANSMITTANCE_TEXTURE_SIZE = float2(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);
	Length r;
	Number mu;
	GetRMuFromTransmittanceTextureUv(atmosphere, uv / TRANSMITTANCE_TEXTURE_SIZE, r, mu);
	return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
����
������Ԥ��������İ����£��������ڿ����õ��������ѯ�õ���Ͷ���������߽��͸���ʣ�����view rayû�к͵����ཻ��
*/
DimensionlessSpectrum GetTransmittanceToTopAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere,
	_IN(TransmittanceTexture) transmittance_texture, Length r, Number mu)
{
	float2 uv = GetTransmittanceTextureUvFromRMu(atmosphere, r, mu);
	//return DimensionlessSpectrum(transmittance_texture.Sample(TransmittanceTexture_Sampler, uv));
	return DimensionlessSpectrum(texture(transmittance_texture, uv));
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
�������Ƶ��ģ���p�߶�Ϊr������view ray����Ϊd�ĵ�q�����ĵľ��룺
rd = ||Oq|| = sqrt(d*d + 2*d*mu*r + r*r)
mud = oq*pi /||oq||*||pi|| = (r*mu + d) / rd

����������Ҫ֪����q���r��mu: rd , mud
mud������[O,q]��[p,i]�ļн�
���Ǽ���pi�ǹ�һ����������ô
||oq||*||pi|| = ||Oq||
oq*pi = oq������[p,i]�ϵ�ͶӰ
���Ǽ���oq��[p,i]�ϵ�ͶӰ��Ϊk
��ôOkq���һ��ֱ�������Σ�б��Ϊ[O,q],��ô�õ�
oq*pi = r*mu + d,���õ�
mud = oq*pi /||oq||*||pi|| = (r*mu + d) / rd


�õ��ڴ���������������p��q֮���͸����ֻ��Ҫ���������ѯ
����:
T(p,i) = T(p,q) * T(q,i)
T(p,q) = T(p,i) / T(q,i)
���Ǽ��������߶�[p,q]���͵����ཻ
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
		//ע�⣬������Ϊ��mu,mu_dȡ���ˣ�Ϊʲôȡ������Ϊ������費�͵����ཻ
		//�����ҵ��������ʱr_d�����r:r_d->r
		//r�����r_d:r->r_d
		//Ҳ���������Ǵ�r_d�����ģ�����Tpi��Tqi�Ի���,������дΪ
		DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, -mu);
		DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, -mu_d);
		DimensionlessSpectrum Tpq = Tpi / Tqi;
		return min(Tpq, DimensionlessSpectrum(1.0));
	}
	else
	{
		DimensionlessSpectrum Tpi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu);
		DimensionlessSpectrum Tqi = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r_d, mu_d);
		DimensionlessSpectrum Tpq = Tpi / Tqi;
		return min(Tpq, DimensionlessSpectrum(1.0));
	}
}

/*
���㵽̫����͸����
r��mu��������ߺ͵����ཻʱray_r_mu_intersects_groundΪtrue��
���������ﲻ����RayIntersectsGround�����������������޵ľ��Ⱥ͸�����������rounding error��������ʮ�ֽӽ���ƽ��ʱ������ܴ���
���ҵ�����һ����˵�и���׳�ķ���֪�������Ƿ�͵����ཻ�������棩

���գ�������Ҫ�ڴ�������һ���̫��֮���͸���ʡ�
̫������һ�����Դ����������̫��Բ����͸���ʵĻ��֡�
������������Ϊ����Բ���ϵ�͸�����ǲ���ģ������ڵ�ƽ�����£���ʱ͸����Ϊ0��
��Ϊ�������̫����͸���ʿ�����GetTransmittanceToTopAtmosphereBoundary���㣬�������ڵ�ƽ����̫���Ĳ��֡�
= ~��ʾ��Լ���ڡ�
Fraction�ı仯�ӵ�̫���춥�Ƕ�Rs���ڵ�ƽ���춥�Ƕ�Rh����̫���ǶȰ뾶As��0����RsС��Rh - As��1��
�ȼ۵ģ����ı仯�ӵ�mu_s = cos(Rs) < cos(Rh + As) = ~ cos(Rh) - As*sin(Rh)ʱ��0��
��mu_s > cos(Rh - As) =~ cos(Rh) + As*sin(Rh)ʱ��1
����֮�䣬̫��Բ�̿ɼ����ֵĽ�����smoothstep(�����ͨ������Բ�ζε������Ϊ��sagitta�ĺ�������֤)��
��ˣ���Ϊsin(Rh) = rbotttom / r,���ǿ���������ĺ������Ƶı�ʾ��̫����͸���ʣ�
*/
DimensionlessSpectrum GetTransmittanceToSun(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu_s)
{
	//sin_theta_h = sin(Rh)
	//�ڵ�������һ��p����||op|| = r,
	//��P������������ߣ��е�Ϊe,||oe|| = rbottom = ����뾶
	//oep���ֱ�������Σ�ֱ��Ϊoep,б��Ϊ[o,p]
	//RhΪ[pe]��[p,o�ļн�]
	// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
	Number sin_theta_h = atmosphere.bottom_radius / r;
	Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));
	

	/*
	Rs:̫���춥��
	Rh:��ƽ���춥��
	As:̫���ĽǶȰ뾶����Ҳ��̫���뾶��Ӧ�ĽǶ�
	��ǰ��֪��
	��̫����ȫ�ڵ�ƽ��֮��ʱ����
	Rs > Rh + As  -> 
	cos(Rs) < cos(Rh +��s)������cos(Rh) - As*sin(Rh) ->
	cos(Rs) < cos(Rh) - As*sin(Rh) ->
	cos(Rs) - cos(Rh) < -As*sin(Rh)
	Ҳ���ǵ�cos(Rs) - cos(Rh) С�� -As*sin(Rh)ʱ��̫����ȫ�ڵ�ƽ��֮�£�Fraction = 0��
	ͬ��
	��̫����ȫ�ڵ�ƽ��֮��ʱ��
	Rs < Rh - As ->
	cos(Rs) > cos(Rh - As) ->
	cos(Rs) > cos(Rh) + As*sin(Rh) ->
	cos(Rs) - cos(Rh) > As*sin(Rh)
	Ҳ���ǵ�cos(Rs) - cos(Rh) ���� As*sin(Rh)ʱ��̫����ȫ�ڵ�ƽ��֮�ϣ�Fraction = 1��
	Fraction = 0 :cos(Rs) - cos(Rh) < -As*sin(Rh)
	Fraction = 1: cos(Rs) - cos(Rh) > As*sin(Rh)
	������Fraction��<0 �� >1�ķ�Χ��
	Fraction��[0,1]ʱ������ȡ��,�ã�
	 -As*sin(Rh) <= cos(Rs) - cos(Rh) <= As*sin(Rh)
	 smoothstep(-As*sin(Rh),As*sin(Rh),cos(Rs) - cos(Rh))
	*/
	Number sin_theat_h_sun_angular = sin_theta_h * atmosphere.sun_angular_radius / rad;
	Number Fraction = smoothstep(-sin_theat_h_sun_angular, sin_theat_h_sun_angular, mu_s - cos_theta_h);
	DimensionlessSpectrum TransmittanceToSun = GetTransmittanceToTopAtmosphereBoundary(atmosphere, transmittance_texture, r, mu_s);

	return TransmittanceToSun * Fraction;
}

/*
����ɢ��

����ɢ��������ڴ�����ǡ��һ��ɢ���¼�֮���̫������ĳЩ��Ĺ⣨����������ڿ������ӻ����ܽ�����;�����ų������Ե���ķ��䣬���������㣩��
���²���������������μ���������ν����洢��Ԥ�ȼ���������У��Լ�������ζ�ȡ����

����

�����ڴﵽ��p֮ǰ��̫�����ڵ�q����������ɢ�������������ܽ���"Mie"�滻��Rayleigh����

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

�ﵽp���radiance�����ɣ�
�ڴ����㶥����̫��irradiance
̫����q֮���͸���ʣ�һ�ֲ�����������̫����ﵽq����ɢ�䣿����
��q���Rayleighɢ��ϵ�������κη��򣬱�ɢ�䵽q��Ĺ⣨��ɢ�䣿����
Rayleigh��λ��������q�㱻ɢ�䵽[q,p]�����ϵĹ⣩
q��p֮���͸���ʣ���q������[q,p]����p��Ĺ⣩

Ws��ʾ̫���ķ����ǵ�λ���� ||Ws|| = 1
r = ||op||
d = ||pq||
mu = (op*pq) / r*d
mu_s = (op*Ws) / (||op||*||Ws||) = (op*Ws) / r 
v = (pq * Ws) / (||pq||*||Ws||) = (pq * Ws) / d

��q���r��mu_s
rq = ||oq|| = sqrt(d*d + 2*d*r*mu + r*r);(�����Ƶ���)
mu_sd = (oq * Ws) / (||oq|| * ||Ws||) ,��Ϊoq = op + pq ->
mu_sd = ((op + pq) * Ws) / rd = (op*Ws + pq*Ws) / rd = (r*mu_s + d*v) / rd

RayLeigh��Mie����ɢ������������������㣨���Ǻ���̫��irradiance����λ�����������ڴ�����ײ���ɢ��ϵ����Ϊ��Ч������֮��������ǣ�
*/
//�����nu�������v
void ComputeSingleScatteringIntegrand(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu, Length d,bool ray_r_mu_intersects_ground,
	_OUT(DimensionlessSpectrum) rayleigh,_OUT(DimensionlessSpectrum) mie)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * d*r*mu + r*r));
	Number mu_s_d = ClampCosine((r*mu_s + nu*d) / r_d);

	//q��p��͸����
	DimensionlessSpectrum T_q_p = GetTransmittance(atmosphere, transmittance_texture, r, mu, d, ray_r_mu_intersects_ground);
	//q��̫��֮���͸����
	DimensionlessSpectrum T_sun_q = GetTransmittanceToSun(atmosphere, transmittance_texture, r_d, mu_s_d);

	//̫����q,Ȼ��q��p��͸����
	DimensionlessSpectrum T_sun_q_p = T_sun_q * T_q_p;

	rayleigh = T_sun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
	mie = T_sun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
}

/*
�����һ��ɢ���̫����ӷ���W�ﵽp����Ҳ�������߷���view rayΪW,dot(W,op) = mu,����op��һ���ˣ�
ɢ�䷢����p��half-line[p,W)������Ĵ�����߽罻��i֮��������q�����߶�[p,i]֮��������q��
��ˣ���p�㷽��W�ϵĵ���ɢ��radiance�����߶�p��i�����е�q��p�ĵ���ɢ��radiance�Ļ��֡�
Ϊ�˼�����������������Ҫ||pi||����
*/

Length DistanceToNearestAtmosphereBoundary(_IN(AtmosphereParameters) atmosphere, Length r, Number mu, bool ray_r_mu_intersects_ground)
{
	if (ray_r_mu_intersects_ground)
		return DistanceToBottomAtmosphereBoundary(atmosphere, r, mu);
	else
		return DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
����ɢ��Ļ���
*/

void ComputeSingleScattering(_IN(AtmosphereParameters) atmosphere, _IN(TransmittanceTexture) transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu,bool ray_r_mu_intersects_ground,
	_OUT(IrradianceSpectrum) rayleigh,_OUT(IrradianceSpectrum) mie)
{
	//��ֵ���ֵļ��������������
	const int SAMPLE_COUNT = 50;
	//������ļ������
	Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);

	DimensionlessSpectrum rayleigh_sum = DimensionlessSpectrum(0.0);
	DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);

	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = Number(i) * dx;

		DimensionlessSpectrum rayleigh_i;
		DimensionlessSpectrum mie_i;

		ComputeSingleScatteringIntegrand(atmosphere, transmittance_texture, r, mu, mu_s, nu, d_i, ray_r_mu_intersects_ground, rayleigh_i, mie_i);

		//����Ȩ��(trapezoidla rule)
		Number weight_i = (i == 0) || (i == SAMPLE_COUNT) ? 0.5 : 1.0;
		rayleigh_sum += rayleigh_i * weight_i;
		mie_sum += mie_i * weight_i;
	}

	rayleigh = rayleigh_sum * dx * atmosphere.solar_irradiance * atmosphere.rayleigh_scattering;
	mie = mie_sum * dx * atmosphere.solar_irradiance * atmosphere.mie_scattering;
}

/*
�������ǰ���ComputeSingleScatteringIntegrand�к��Ե�̫��irradiance��ɢ��������ϣ�
����û����λ������Ϊ�˸��õĽǶȾ���������render timeʱ�ټӡ�
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
Ԥ����

ComputeSingleScattering�����������ɱ�ʮ�ָߣ���������ɢ����Ҫ�ܶ�ε�������
�������ϣ��Ԥ���㵽���������Ҫӳ�亯����4���������������ꡣ
�������������и�4D����������Ҫ����һ����(r,mu,mu_s,v)����������(u,v,w,z)��ӳ�䡣
�����ִ�к��������ǵ������ﶨ�壬��һЩС�ĸĽ���

.mu��ӳ�俼�ǵ����������߽����С���룬ӳ��mu��������[0,1]����������ӳ��û�и���ȫ[0,1]�����
.mu_s��ӳ���������ĸ�ͨ�ã�ԭʼ�������ӳ��ʹ����һ��Ϊ���������ѡ������ⳣ������
 �����ڵ�����������߽�ľ��루��̫��ray��˵��������mu��ӳ�䣬ֻʹ��һ����������������õģ���
  ����ԭʼ����ģ������ڿ�����ƽ��ʱ���Ӳ�����
*/

float4 GetScatteringTextureUvwzFromRMuMuSNu(_IN(AtmosphereParameters) atmosphere, Length r, Number mu,
	Number mu_s, Number nu, bool ray_r_mu_intersects_ground)
{
	//�ڴ�������һ��p�Ե��������ߣ��е�e�����ߺʹ����㽻��Ϊi,����ΪO��OpeΪֱ�������Σ�б��ΪOp
	//H = ||ei||= sqrt(||Oi||*||Oi|| - ||Oe||*||Oe||) = sqrt(top_radius * top_radius - bottom_radius * bottom_radius)
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);

	//K = ||pe||= sqrt(||Op||*||Op|| - ||Oe||*||Oe||) = sqrt(r * r - bottom_radius * bottom_radius)
	Length K = sqrt(r*r - atmosphere.bottom_radius * atmosphere.bottom_radius);
	
	Number u_r = GetTextureCoordFromUnitRange(K / H, SCATTERING_TEXTURE_R_SIZE);

	//��ray(r,mu)�͵���Ķ��η��̵��б�see RayIntersectsGround��
	Area discriminant = atmosphere.bottom_radius * atmosphere.bottom_radius + r*r*(mu*mu - 1.0);
	Number u_mu;
	if (ray_r_mu_intersects_ground)
	{
		//ray(r,mu)������ľ��룬������mu����С�����ֵ��ͨ��ray(r,-1)��ray(r,m_horizon)���
		//��ǰray(r,mu)�͵����ཻ��d
		//�����е㲻���ף�d = sqrt(discriminant) - r*mu����������ΪʲôҪ��-sqrt(discriminant) - r*mu
		//������Ϊ���ſ��ܺ�mu�йأ���Ϊֻ��ray��Op�Ƕ�Ϊ����90�ȣ����ܺ͵����ཻ��Ҳ����mu<0��
		Length d = -sqrt(discriminant) - r * mu;
		Length d_min = r - atmosphere.bottom_radius;
		Length d_max = K;

		//���ﲻ��ӳ�䵽[0,1]����[0,0.5]����Ϊ��һ��Ҫ�������͵����ཻ�����
		u_mu = 0.5 - 0.5 * GetTextureCoordFromUnitRange(d_max == d_min ? 0.0 : (d - d_min) / (d_max - d_min), SCATTERING_TEXTURE_MU_SIZE / 2);
	}
	else
	{
		//ray(r,mu)�Ͷ���������ľ��룬������mu����С���ֵ��ͨ��ray(r,1)��ray(r,mu_horizon)���
		//�����sqrt(discriminant)����Ϊ������Ϊmu>0?,���ǲ��͵����ཻҲ����mu < 0
		//��������Ͷ��������㽻����룬����dis = top_radius*top_radius + r*r*(mu*mu - 1.0)
		//H*H = top_radius*top_radius - bottom_radius*bottom_radius
		//discriminant + H * H = bottom_radius*bottom_radius + r*r(mu*mu - 1) + H*H = top_radius*top_radius + r*r*(mu*mu - 1.0)
		Length d = sqrt(discriminant + H * H) - r*mu;
		Length d_min = atmosphere.top_radius - r;
		Length d_max = K + H;
		u_mu = 0.5 + 0.5 * GetTextureCoordFromUnitRange((d - d_min) / (d_max - d_min), SCATTERING_TEXTURE_MU_SIZE / 2);
	}

	//����u_mu_s
	//��ǰmu_s���ڵ���һ�㵽������ľ���
	Length d_s = DistanceToTopAtmosphereBoundary(atmosphere, atmosphere.bottom_radius, mu_s);
	//ray(bottom_radius,mu_s)��С����� min:ray(bottom,cos(0)),max:ray(bottom,cos(90))
	Length d_min_s = atmosphere.top_radius - atmosphere.bottom_radius;
	Length d_max_s = H;
	Number a = (d_s - d_min_s) / (d_max_s - d_min_s);
	Number A = -2.0 * atmosphere.mu_s_min * atmosphere.bottom_radius / (d_max_s - d_min_s);
	Number u_mu_s = GetTextureCoordFromUnitRange(max(1.0 - a / A, 0.0) / (1.0 + a), SCATTERING_TEXTURE_MU_S_SIZE);
	Number u_nu = (nu + 1.0) / 2.0;

	return float4(u_nu, u_mu_s, u_mu, u_r);
}

//�����
void GetRMuMuSNuFromScatteringTextureUvwz(_IN(AtmosphereParameters) atmosphere, _IN(float4) uvwz,
	_OUT(Length) r, _OUT(Number) mu, _OUT(Number) mu_s, _OUT(Number) nu,
	_OUT(bool) ray_r_mu_intersects_ground)
{
	Length H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);
	Length K = H * GetUnitRangeFromTextureCoord(uvwz.w,SCATTERING_TEXTURE_R_SIZE);
	r = sqrt(K*K + atmosphere.bottom_radius * atmosphere.bottom_radius);
	if (uvwz.z < 0.5)
	{
		//ray(r,mu)������ľ��룬������mu����С�����ֵ��ͨ��ray(r,-1)��ray(r,m_horizon)���
		Length d_min = r - atmosphere.bottom_radius;
		Length d_max = K;
		Length d = d_min + (d_max - d_min) * GetUnitRangeFromTextureCoord(1.0 - 2.0 * uvwz.z, SCATTERING_TEXTURE_MU_SIZE / 2);
		mu = d == 0.0*m ? Number(-1.0) : ClampCosine(-(K * K + d * d) / (2.0 * r * d));
		ray_r_mu_intersects_ground = true;
	}
	else
	{
		//ray(r,mu)�Ͷ���������ľ��룬������mu����С���ֵ��ͨ��ray(r,1)��ray(r,mu_horizon)���
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
������������4D������ʵ�в������ڡ����������Ҫ��һ����3D��4D����֮��ӳ�䡣
����ĺ���չ��һ��3D���굽һ��4D�������꣨r,mu,mu_s,v����
����ͨ����x����"unpacking"����������ʵ�֡�ע����������ô�������clamp v�ġ�
������Ϊv������ȫ�Ķ���������ֵ��Χ������mu��mu_s������Դ�zenith,view,sun direction�ĵѿ�������������mu,mu_s,v��������
֮ǰ�ĺ��������ؼ�����һ�㣨��������ش�Լ�������ǵĶ��Կ��ܻᱻ���ƣ�
*/
void GetRMuMuSNuFromScatteringTextureFragCoord(_IN(AtmosphereParameters) atmosphere, _IN(float3) fragCoord,
	_OUT(Length) r, _OUT(Number) mu, _OUT(Number) mu_s, _OUT(Number) nu, _OUT(bool) ray_r_mu_intersects_ground)
{
	const float4 SCATTERING_TEXTURE_SIZE = float4(SCATTERING_TEXTURE_NU_SIZE - 1,
																					SCATTERING_TEXTURE_MU_S_SIZE,
																					SCATTERING_TEXTURE_MU_SIZE,
																					SCATTERING_TEXTURE_R_SIZE);
	/*
	fragcoord��Χ��[0,width]����[0,1]
	��������Ҫ��nu��mu_s��άӳ���һά
	����һ��2D����x,y [with,height]ӳ���1D���� x[width * height]
	2d->1d  frag_coord_x = y * width + x 
	1d->2d y = frag_coord_x / width ; x = frag_coord_x % width
	�����width = SCATTERING_TEXTURE_MU_S_SIZE
	*/
	Number frag_coord_nu = floor(fragCoord.x / Number(SCATTERING_TEXTURE_MU_S_SIZE));
	Number frag_coord_mu_s = mod(fragCoord.x, Number(SCATTERING_TEXTURE_MU_S_SIZE));
	float4 uvwz = float4(frag_coord_nu, frag_coord_mu_s, fragCoord.y, fragCoord.z) / SCATTERING_TEXTURE_SIZE;

	GetRMuMuSNuFromScatteringTextureUvwz(atmosphere,uvwz,r,mu,mu_s,nu,ray_r_mu_intersects_ground);

	//�ڸ���mu��mu_s������£�Clamp nu��������Чֵ��Χ
	Number v0 = sqrt((1.0 - mu*mu) * (1.0 - mu_s*mu_s));
	nu = clamp(nu, mu*mu_s - v0, mu*mu_s + v0);
}

//�������ӳ�䣬���ǿ���дһ��Ԥ���㵥��ɢ��ĺ���
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
����
������Ԥ��������İ����£��������ڿ��������������ѯ�õ�һ���������Ĵ�����߽�֮���ɢ��
��������Ҫ����3D�����˫���Բ�ֵ��ģ��һ��4D�����ѯ������ʹ����GetRMuMuSNuFromScatteringTextureFragCoord��
�����ӳ��������������3D�������꣩
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
	return AbstractSpectrum(texture(scattering_texture, uvw0) * (1.0 - lerp) + texture(scattering_texture, uvw1) * lerp);
}

/*
��������������ṩ��һ��������ҹ��ܣ��⽫����һ�������á� 
�˺������ذ�����λ�����ĵ���ɢ�䣬���ߵ�n orderɢ��(n>1)�� 
���裬���scat_order�ϸ����1����multiple_scattering_texture��Ӧ�ڴ�ɢ��order������Rayleigh��Mie���Լ�������λ�����
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

//���ɢ��
/*
���ɢ������(radiance)��ָ�ڴ������д�̫���������λ��η���󵽴�ĳ���̫���⣨���з�����ɢ���¼������Ե���ķ��䣩�� 
���²���������������μ���������ν����洢��Ԥ�ȼ���������У��Լ�������ζ�ȡ����

��ע�⣬���ڵ���ɢ�䣬�����ڴ˴��ų������һ�η����ǵ��淴��Ĺ�·�� 
��Щ·���Ĺ�������Ⱦʱ�������㣬�Ա㿼��ʵ�ʵĵ��淴���ʣ����ڵ����ϵ��м䷴�䣬����Ԥ�ȼ���ģ�����ʹ��ƽ���ľ��ȷ����ʣ���
*/

//����
/*
���ɢ����Էֽ�Ϊ����ɢ�䣬����ɢ��ȵ��ܺͣ�����ÿ�����Ӧ��������2,3�εȷ���֮���ڴ����е�ĳ��������̫���Ĺ⡣ 
���ң�ÿ���������ǰһ����Ļ����ϼ��㡣 
ʵ���ϣ���n�η���֮��ӷ���ص���ĳ��p�Ĺ�������һ�η��������п��ܵ�q�ϵĻ��֣����漰��n-1����֮����κη��򵽴�q�Ĺ⡣

������������ÿ��ɢ��orfer��Ҫ��ǰһ���������ػ��֣��ڴӦط����ϵ�p����������߽��߶��ϵ����е�q�ϵ�һ�����֣��Լ���q����ÿ�������ϵ����з����ϵ�Ƕ��˫���֣��� 
��ˣ����������Ҫ����ͷ��ʼ������ÿ��order�����Ƕ�����ɢ����Ҫ���ػ��֣�����ɢ����Ҫ���ػ��ֵȡ�
��������������㣨��n order�ļ�����������ظ�����֮ǰorder�ļ��㣬���¶�order�����Ķ��θ��Ӷ�(O(n*n))��������Ȼ�ǵ�Ч�ġ� 
�෴������ļ���Ҫ��Ч�ܶ�:
1) 
Ԥ������ɢ�䲢������һ��������
2) 
��n>=2
	Ԥ���������еĵ�n��ɢ�䣬�䱻�����������ػ������ڵڣ�n-1����ɢ�������в��ҵõ�

�ò��Ա��������������㣬����δ����������Щ���㡣 
����������ͼ�еĵ�p��p'���Լ�������n�η���֮��ӷ���ص���������������Ĺ�����ļ��㡣 
��Щ���������漰����(radiance)L���������÷���L����n-1����֮������з�����q�㱻ɢ�䵽����-�أ�	

p -------p'---------q---->��

��ˣ�����������������������ػ��ּ����n��ɢ�䣬���ǽ�����ؼ���L����ʵ�ϣ���������p��q֮��ĵ����-�ط����ϴ�����߽�����㣩�� 
Ϊ�˱�������������Ӷ���߶��ɢ������Ч�ʣ����ǽ������㷨ϸ�����£�
1)
Ԥ������ɢ�䲢������һ��������
2)
��n>=2
	����ÿ����q�ͷ���أ�Ԥ�ȼ�����q������-�ط���ɢ��Ĺ⣬��Щ������n-1����֮����κη�������漰˫���֣��䱻������ʹ�ã�n-1����ɢ�����������ң�
	����ÿ����p�ͷ���أ�Ԥ�ȼ���n�η��������Է���صĹ⣨��ֻ�漰�������֣��䱻������ʹ��ǰһ�м���������еĲ��ң�

Ϊ�˻���������㷨���������ڱ���ָ������������ѭ����ʵ�����������衣 ����������ڱ������ಿ�������Ĺ�����

��һ��

��һ�������ڴ�����ĳ����q����ĳ������-��ɢ��ķ���(radiance)�� ���⣬���Ǽ����ɢ���¼��ǵ�n�η�����
�÷��������п��ܵ����䷽���_i�Ļ���
	��������(radiance)L_i�ڵڣ�n - 1���η����ӷ����_i����q�ĺ���:
		(n - 1) order��Ԥ����ɢ���������һ����
		�������ray(q,��_i)�͵����ཻ��r��������n - 1�η�����·�Ĺ��ף�������һ�η���λ��r�����ڵ�����
		(���ݶ��壬�����ǵ�������������Щ·�����ų����⣬�������������Ǳ��뿼�����ǣ���Ϊ����ķ���֮����q���ķ�����)
		����������Щ���������²���:
			q��r֮���͸����
			(ƽ��)���淴����
			Lambertian BRDF 1/PI
			��n - 2�η�����ĵ������(irradiance)���ܡ���������һ�ν���������ô��Ԥ���㵽����
			���ڣ����Ǽ���������������ĺ�����Ԥ����������ܷ���(irradiance)��
*/
IrradianceSpectrum GetIrradiance(_IN(AtmosphereParameters) atmosphere,_IN(IrradianceTexture) irradiance_texture,Length r, Number mu_s);

/*
	��q���ɢ��ϵ��
	�غͦ�_i�����ɢ����λ����

�⵼��������ʵ�֣�����multiple_scattering_textureӦ�ð���ɢ��ĵڣ�n-1����
���n> 2����illuminiance_texture����n-2֮���ڵ����Ͻ��յķ��ն� ��scattering_order����n����
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
	RadianceDensitySpectrum rayleigh_mie = RadianceDensitySpectrum(0.0 * watt_per_cubic_meter_per_sr_per_nm);

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
		DimensionlessSpectrum transmittance_to_ground = DimensionlessSpectrum(0.0);
		DimensionlessSpectrum ground_albedo = DimensionlessSpectrum(0.0);
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
�ڶ���

�����n��ɢ��order�ĵڶ���������ǰ�溯��Ԥ�������������ÿ����p�ͷ���ص�radiance
��radiance����n�η�����ķ����

���radiance��q��p������е���ڦط����Ϻʹ������������Ļ���:
	1��֮ǰ����Ԥ������������һ�������n - 1�η�������κη�����q�㱻ɢ�䵽p
	2��p��qֱ�ӵ�͸����

ע�������ų�����һ�η����ڵ����ϵĵ�n�η�����Ҳ����n-1�εķ�������е��棬��ɵ�n�η�����
ȷʵ������ѡ�������ǵ�Ԥ���������ų���Щ·�������������ǿ�������Ⱦʱ�������ǣ�ʹ��ʵ�ʵĵ��淴���ʡ�

�ڶ�����ִ�кܼ򵥣�
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
	RadianceSpectrum rayleigh_mie_sum = RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm);
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