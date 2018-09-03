

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

因此，为了计算任意两点的透视率，需要满足知道在大气层里点X和在大气层边缘上点i之间的透视率。
（
这里的意思是，要知道在大气层里任意两点p，q之间的透视率T(p,q)，需要知道沿着p，q发射射线得到和大气层边缘交点i，这样计算T(p,i)T(q,i)
就可以计算T(p,q) = T(p,i) / T(q,i)
）

透视率值依赖两个参数，点到地球中心的半径r=||op||和视线和向量op之间的夹角（view zenith angle）的cos值。
mu = cos(a) = op * pi / （||op|| * ||pi||) = dot(op,pi)（dot需要归一化向量）

为了计算这个，首先要计算长度||pi||，并且需要知道线段[p,i]是否和地面相交
*/


/*
点到顶部大气层边缘的距离
从点p沿着[p,i)距离d的点的坐标为[d*sqrt(1 - mu * mu),r + d*mu)
这里mu=cos(view zenith angle),所以sqrt(1 - mu * mu) = sin(view zenith angle)
d * sin(view zenith angle)=线段[p,i]在xz屏幕的长度
r + d *mu= 线段[O,i]投影到[O,p]的长度

由于i是在一个圆上，所以可以计算[O,i]的长度为：
pow(d*sqrt(1 - mu * mu),2) + pow(r + d*mu,2) = 
d*d * (1 - mu*mu) + r*r + d*d * mu*mu + 2 * r * d * mu = 
d*d - d*d * mu*mu + r*r + d*d + d*d * mu*mu + 2 * r * d * mu =
d*d + 2 *r *d * mu + r*r

因此，根据i的定义，我们有||pi|| * ||pi|| + 2 * r * mu * ||pi|| + r*r = R * R	//R = 大气层边缘半径
从上面要导出||pi||

根据pow((a + b),2) = a*a + 2 * a * b + b*b
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

bool RayIntersectGround(in AtmosphereParameters atmopshere, Length r, Number mu)
{
	//只有当mu <0才可能和地面相交
	return mu < 0.0 && atmopshere.bottom_radius * atmopshere.bottom_radius + r*r * (mu*mu - 1.0) >= 0.0;
}

