

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
*/
Length DistanceToTopAtmosphereBoundary(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.top_radius * atmopshere.top_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
������Ҫ������һ���������[p,i]�͵����ཻʱ�����������Ƶķ�ʽ����
*/
Length DistanceToBottomAtmosphereBoundary(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//R*R + r*r * (mu*mu - 1)
	Area discriminant = r * r * (mu * mu - 1.0) + atmopshere.bottom_radius * atmopshere.bottom_radius;
	return ClampDistance(SafeSqrt(discriminant) - r * mu);
}

/*
�ж�[P,i]�Ƿ�������ཻ
������Ĺ�ʽ�ó������͵����ཻʱ�е�ʽ
d*d + 2*r*d*mu + r*r = Rb*Rb
d = sqrt( Rb*Rb + r*r * (mu*mu - 1)) - r*mu
����ҪRb*Rb + r*r * (mu*mu - 1) > 0
�ɴ������Ƶ���
*/

bool RayIntersectsGround(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//ֻ�е�mu <0�ſ��ܺ͵����ཻ
	return mu < 0.0 && atmopshere.bottom_radius * atmopshere.bottom_radius + r*r * (mu*mu - 1.0) >= 0.0;
}


/*
Beer-Lambert law
T = exp(-r)
r : optical depth����ѧ��ȣ�
�������ڿ��Լ���p��i֮���͸���ʡ�����Beer-Lambert law�����漰�����߶�[p,i]�ϵĿ������ӵ��ܶȵ���ֵ���֣��Լ����ܽ��ܶȵ���ֵ���ֺ�
���չ⣨����������Ŀ������ӵ��ܶȵ���ֵ���֡����߶�[p,i]���͵����ཻʱ�����3��������һ������ʽ��exp(-r)�������ǿ����ٽ�������һЩ����
������ʹ��trapezoidal rule���İ����½�����ֵ���㡣
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
	//��ֵ���ֵļ������
	const int SAMPLE_COUNT = 500;
	//���ֲ�����ÿ�����ּ���ĳ���
	Length dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / Number(SAMPLE_COUNT);
	//����ѭ��
	Length result = 0.0;
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
		result += y_i * weight * dx;
	}

	return result;
}

/*
p��i֮���͸�������ں����׼����ˣ����Ǽ��������߶�û�к͵����ཻ��
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

float2 GetTransmittanceTextureUvFromRMu(in AtmosphereParameters atmosphere, Length r, Number mu)
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
	Number v = GetTextureCoordFromUnitRange(x_r, TRANSMITTANCE_TEXTURE_WIDTH);
	return float2(u, v);
}

//���溯���������
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
	//������֪����d_min��mu = 1ʱ���֣���d == 0ʱ��mu = 1
	mu = d == 0.0 ? Number(1.0) : (H * H - K * K - d * d) / (2.0 * r * d);
	mu = ClampCosine(mu);
}

//���ڿ�����һ������Ԥ����͸��������
DimensionlessSpectrum ComputeTransmittanceToTopAtmosphereBoundaryTexture(in AtmosphereParameters atmosphere, in float2 uv)
{
	const float2 TRANSMITTANCE_TEXTURE_SIZE = float2(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);
	Length r;
	Number mu;
	GetRMuFromTransmittanceTextureUv(atmosphere, gl_frag_coord / TRANSMITTANCE_TEXTURE_SIZE, r, mu);
	return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
����
������Ԥ��������İ����£��������ڿ����õ��������ѯ�õ���Ͷ���������߽��͸���ʣ�����view rayû�к͵����ཻ��
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
DimensionlessSpectrum GetTransmittanceToSun(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu_s)
{
	//sin_theta_h = sin(Rh)
	//�ڵ�������һ��p����||op|| = r,
	//��P������������ߣ��е�Ϊe,||oe|| = rbottom = ����뾶
	//oep���ֱ�������Σ�ֱ��Ϊoep,б��Ϊ[o,p]
	//RhΪ[pe]��[p,o�ļн�]
	// sin(Rh) = ||oe|| / ||po|| = rbottom / r	
	Number sin_theta_h = atmosphere.bottom_radius / r;
	Number cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h), 0.0);
	

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
	Number sin_theat_h_sun_angular = sin_theta_h * atmosphere.sun_angular_radius;
	Number Fraction = smoothstep(-sin_theat_h_sun_angular, sin_theat_h_sun_angular, mu_s - cons_theta_h);
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
void ComputeSingleScattingIntegrand(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu, Length d,bool ray_r_mu_intersects_ground,
	out DimensionlessSpectrum rayleigh,out DimensionlessSpectrum mie)
{
	Length r_d = ClampRadius(atmosphere, sqrt(d*d + 2 * d*r*mu + r*r));
	Number mu_s_d = ClampCosine((r*mu_s + nu*d) / r_d);

	//p,q֮���͸����
	DimensionlessSpectrum Tpq = GetTransmittance(atmosphere, transmittance_texture, r, mu, d, ray_r_mu_intersects_ground);
	//q��̫��֮���͸����
	DimensionlessSpectrum Tsun_q = GetTransmittanceToSun(atmosphere, transmittance_texture, r_d, mu_s_d);

	//̫����q,Ȼ��q��p��͸����
	DimensionlessSpectrum Tsun_q_p = Tsun_q * Tpq;

	rayleigh = Tsun_q_p * GetProfileDensity(atmosphere.rayleigh_density, r_d - atmosphere.bottom_radius);
	mie = Tsun_q_p * GetProfileDensity(atmosphere.mie_density, r_d - atmosphere.bottom_radius);
}

/*
�����һ��ɢ���̫����ӷ���W�ﵽp����Ҳ�������߷���view rayΪW,dot(W,op) = mu,����op��һ���ˣ�
ɢ�䷢����p��half-line[p,W)������Ĵ�����߽罻��i֮��������q�����߶�[p,i]֮��������q��
��ˣ���p�㷽��W�ϵĵ���ɢ��radiance�����߶�p��i�����е�q��p�ĵ���ɢ��radiance�Ļ��֡�
Ϊ�˼�����������������Ҫ||pi||����
*/

Length DistanceToNearestAtmosphereBoundary(in AtmosphereParameters atmosphere, Length r, Number mu, bool ray_r_mu_intersects_ground)
{
	if (ray_r_mu_intersects_ground)
		return DistanceToBottomAtmosphereBoundary(atmosphere, r, mu);
	else
		return DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
}

/*
����ɢ��Ļ���
*/

void ComputeSingleScatteringin(in AtmosphereParameters atmosphere, in TransmittanceTexture transmittance_texture,
	Length r, Number mu, Number mu_s, Number nu,bool ray_r_mu_intersects_ground,
	out IrradianceSpectrum rayleigh,out IrradianceSpectrum mie)
{
	//��ֵ���ֵļ��������������
	const int SAMPLE_COUNT = 50;
	//������ļ������
	Length dx = DistanceToNearestAtmosphereBoundary(atmosphere, r, mu, ray_r_mu_intersects_ground) / Number(SAMPLE_COUNT);

	DimensionlessSpectrum raylegh_sum = DimensionlessSpectrum(0.0);
	DimensionlessSpectrum mie_sum = DimensionlessSpectrum(0.0);

	for (int i = 0; i <= SAMPLE_COUNT; ++i)
	{
		Length d_i = Number(i) * dx;

		DimensionlessSpectrum rayleigh_i;
		DimensionlessSpectrum mie_i;

		ComputeSingleScatteringIntegrand(atmosphere, transmittance_texture, r, mu, mu_s, nu, d_i, ray_r_mu_interescts_ground, rayleigh_i, mie_i);

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
	return k * (1.0 = nu*nu) / pow(1.0 + g*g - 2.0*g*nu, 1.5);
}