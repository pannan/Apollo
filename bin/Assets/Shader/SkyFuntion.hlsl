

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

����i����һ��Բ�ϣ����Կ��Լ���[O,i]�ĳ���Ϊ��
pow(d*sqrt(1 - mu * mu),2) + pow(r + d*mu,2) = 
d*d * (1 - mu*mu) + r*r + d*d * mu*mu + 2 * r * d * mu = 
d*d - d*d * mu*mu + r*r + d*d + d*d * mu*mu + 2 * r * d * mu =
d*d + 2 *r *d * mu + r*r

��ˣ�����i�Ķ��壬������||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R	//R = �������Ե�뾶
������Ҫ����||pi||

����pow((a + b),2) = a*a + 2 * a * b + b*b
pow(d + r*mu,2) = d*d + 2*d*r*mu + r*r*mu*mu

d*d + 2*r*d*mu + r*r = 
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

bool RayIntersectGround(in AtmosphereParameters atmopshere, Length r, Number mu)
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
����ѧ�У���������ԭʼ�۲췶Χ֮�⣬��������һ�������Ĺ�ϵ���Ʊ�����ֵ�Ĺ��̡� 
�������ڲ�ֵ��������֪�۲�ֵ֮���������ֵ���������ƻ��ܵ�����Ĳ�ȷ���ԺͲ������������ĸ��߷��ա�
}

���������������ɱ��ܸߣ����㵥�κͶ��ɢ����Ҫ�ܶ��������
���˵��Ǻ���ֻ����������������ʲôƽ����Ӧ����ֵ����û�и�Ƶ��״��
���ǿ�����һ��2d������Ԥ�������Ż�����

����������Ҫ�ں�������(r,mu)����������(u,v)֮����һ��ӳ�䣬��֮ҲȻ��
��Ϊ��Щ����������һ�µĵ�λ��ֵ��Χ��
��ʹ������������ڴ�СΪn���������Լ��[0,1]���溯��f����0.5/n��1.5/n��......(n - 0.5)/n��������
��Ϊ�������ʱ��texels�����ģ����Դ�0.5��ʼ����1Ϊ�������
��ˣ�������ı߽磨0��1��ֻ�ܸ��������ƶϵĺ���ֵ��extrapolated function values����
���������˼������ǣ���Ϊǰ��Ĳ���ʱ��0.5��ʼ��(n-0.5)/n����������������ı߽�
0��1��û�в����ģ�����ֻ��ͨ���ƶϣ���ֵ�������ֵ��

Ϊ�˱������������������Ҫ������Ԫ��0�����Ĵ���f(0)��������f(0.5)��
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
                              ----------------e---						 ------
		       			                     ------------							 i-------
					      			                      ----------						------ ������
								
										

������ʾ��
��p�����ཻ
view ray�ʹ������ཻ����̾����� ||pt||
view ray�ʹ������ཻ���������||pe|| + ||ei||
||pe|| = p
||ei|| = H
��e��view ray�͵������еĵ�
i��veiw ray�ʹ������ཻ�ĵ�

dmin = rtop - r
dmax = k + H
����ӳ��mu��һ����ΧΪ[0,1]��ֵXmu

������Щ���壬����ӳ��(r,mu)����������(u,v)��������ִ��:
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
	//k = ||Oe|| = sqrt(||Op||*||Op|| - ||Oe||*||Oe||)
	//��||Op|| = r
	//||Oe|| = atmosphere.bottom_radius
	Length K = sqrt(r * r - atmosphere.bottom_radius * atmosphere.bottom_radius);

	Length d = DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
	Length d_min = atmosphere.top_radius - r;
	Length d_max = H + K;
	Number x_mu = (d - d_min) / (d_max - d_min);
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