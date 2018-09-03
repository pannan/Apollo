

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

