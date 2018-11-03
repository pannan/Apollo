/*
此GLSL文件用于在GPU和CPU上的model_test.cc中渲染测试场景，以评估GPU气大气型中的近似值。
因此，对于GPU模型着色器，它的编写方式可以使用GLSL编译器或C ++编译器进行编译。

如上所示，测试场景是纯球形行星P上的球体S.它由“光线跟踪”渲染，即顶点着色器输出视图光线方向，片段着色器计算此光线与球体的交点 S和P产生最终像素。 
片段着色器还计算光线与球体S的交点，以计算阴影，以及视线与阴影体积S的交点，以计算光轴。

阴影和光shatfs

在我们可以在主着色器函数中使用它们之前，必须先定义计算阴影和光shatfs的函数，因此我们首先定义它们。 
测试点是否在球体S的阴影中等效于测试相应的光线是否与球体相交，这非常简单。 但是，这仅适用于punctual光源，而不是太阳的情况。
在下面的函数中，我们通过考虑太阳的角度大小来计算近似（和偏置）的软阴影：
*/
Number GetSunVisibility(Position point, Direction sun_direction) 
{
	Position p = point - kSphereCenter;
	Length p_dot_v = dot(p, sun_direction);
	Area p_dot_p = dot(p, p);
	Area ray_sphere_center_squared_distance = p_dot_p - p_dot_v * p_dot_v;
	Length distance_to_intersection = -p_dot_v - sqrt(
		kSphereRadius * kSphereRadius - ray_sphere_center_squared_distance);
	if (distance_to_intersection > 0.0 * m) 
	{		
		//计算视线和球体之间的距离，以及相应的（切线）对角。 最后，使用它来计算近似的太阳能见度。
		Length ray_sphere_distance = kSphereRadius - sqrt(ray_sphere_center_squared_distance);
		Number ray_sphere_angular_distance = -ray_sphere_distance / p_dot_v;
		return smoothstep(Number(1.0), Number(0.0), ray_sphere_angular_distance / sun_size_.x);
	}
	return 1.0;
}

/*
球体也部分地遮挡了天空光，我们用环境遮挡因子来近似这种效应。 
由球体引起的环境遮挡因子在辐射视图因子中给出（Isidoro Martinez，1995）。 
在球体完全可见的简单情况下，它由以下函数给出：
*/
Number GetSkyVisibility(Position point) 
{
	Position p = point - kSphereCenter;
	Area p_dot_p = dot(p, p);
	return 1.0 + p.z / sqrt(p_dot_p) * kSphereRadius * kSphereRadius / p_dot_p;
}

/*
为了计算光轴，我们需要视线与球体S的阴影体积的交点。
由于太阳不是一个准时光源，这个阴影体不是圆柱体而是圆锥体（对于本影，加上另一个圆锥体用于 半影，但我们在这里忽略它）：

注意，如上图所示，p是摄像机位置，v和s是单位view ray和太阳方向矢量，R是球体半径（假设以原点为中心），
距离摄像机距离d的点是q =p + dv。 该点距球面中心沿着本影锥轴的距离δ=-q⋅s
距离该轴的距离r由r⋅r =q⋅q-δ⋅δ给出。 最后，在沿轴线的距离δ处，本影锥具有半径ρ= R-δ⋅tanα，其中α是太阳的角半径。
只有当r⋅r =ρ⋅ρ时，距离摄像机距离d的点才在阴影锥上，即仅
(p+dv)⋅(p+dv)−pow(((p+dv)⋅s),2)=pow((R+((p+dv)⋅s)tanα),2)

开发它给出了d的二次方程式：
ad*ad+2*bd+c=0

a=1−l*pow((v⋅s),2)
b=p⋅v−l(p⋅s)(v⋅s)−tan(α)R(v⋅s)
c=p⋅p−l*pow((p⋅s),2)−2tan(α)R(p⋅s)−R*R
l=1+pow(tanα,2)

由此我们推导出d的两种可能的解决方案，
必须将其clamp到数学锥体的实际阴影部分（即球体中心和锥形顶点之间的平板，或换句话说，δ在0 和R /tanα之间）。
以下函数实现了这些方程式：
*/
void GetSphereShadowInOut(Direction view_direction, Direction sun_direction,_OUT(Length) d_in, _OUT(Length) d_out) 
{
	Position pos = camera_ - kSphereCenter;
	Length pos_dot_sun = dot(pos, sun_direction_);
	Number view_dot_sun = dot(view_direction, sun_direction_);
	Number k = sun_size_.x;
	Number l = 1.0 + k * k;
	Number a = 1.0 - l * view_dot_sun * view_dot_sun;
	Length b = dot(pos, view_direction) - l * pos_dot_sun * view_dot_sun - k * kSphereRadius * view_dot_sun;
	Area c = dot(pos, pos) - l * pos_dot_sun * pos_dot_sun - 2.0 * k * kSphereRadius * pos_dot_sun - kSphereRadius * kSphereRadius;
	Area discriminant = b * b - a * c;
	if (discriminant > 0.0 * m2)
	{
		d_in = max(0.0 * m, (-b - sqrt(discriminant)) / a);
		d_out = (-b + sqrt(discriminant)) / a;
		// The values of d for which delta is equal to 0 and kSphereRadius / k.
		Length d_base = -pos_dot_sun / view_dot_sun;
		Length d_apex = -(pos_dot_sun + kSphereRadius / k) / view_dot_sun;
		if (view_dot_sun > 0.0) {
			d_in = max(d_in, d_apex);
			d_out = a > 0.0 ? min(d_out, d_base) : d_base;
		}
		else 
		{
			d_in = a > 0.0 ? max(d_in, d_base) : d_base;
			d_out = min(d_out, d_apex);
		}
	}
	else 
	{
		d_in = 0.0 * m;
		d_out = 0.0 * m;
	}
}

/*
主要着色功能

使用这些函数，我们现在可以实现主着色器函数，该函数计算给定视图光线的场景辐射。
此功能首先测试视线是否与球体S相交。如果是这样，它会计算球体在交叉点处接收的太阳和天空光，将其与球体BRDF和相机与球体之间的空中视角相结合。
然后它与地面相同，即与行星球P相同，然后计算天空辐射和透射率。 
最后，将所有这些项合成在一起（还使用近似视锥 - 球面交叉因子计算每个对象的不透明度）以获得最终的辐亮度。

我们首先计算视线与球体阴影体积的交点，因为它们需要获得球体和行星的空中透视：
*/
RadianceSpectrum GetViewRayRadiance(Direction view_ray,Direction view_ray_diff) 
{
	// Normalized view direction vector.
	Direction view_direction = normalize(view_ray);
	// Tangent of the angle subtended by this fragment.
	Number fragment_angular_size = length(view_ray_diff) / length(view_ray);

	Length shadow_in;
	Length shadow_out;
	GetSphereShadowInOut(view_direction, sun_direction_, shadow_in, shadow_out);

	//然后，我们测试视线是否与球体S相交。 如果是，我们使用与GetSunVisibility中相同的近似值计算近似（和偏差）不透明度值：

	//计算view ray和球心的距离，以及相机和view ray与球交点的距离（如果没有相交为NaN）
	Position p = camera_ - kSphereCenter;
	Length p_dot_v = dot(p, view_direction);
	Area p_dot_p = dot(p, p);
	Area ray_sphere_center_squared_distance = p_dot_p - p_dot_v * p_dot_v;
	Length distance_to_intersection = -p_dot_v - sqrt(kSphereRadius * kSphereRadius - ray_sphere_center_squared_distance);

	// 如果相交，计算被球反射的辐射
	Number sphere_alpha = 0.0;
	RadianceSpectrum sphere_radiance = RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm);

	if (distance_to_intersection > 0.0 * m) 
	{
		// Compute the distance between the view ray and the sphere, and the
		// corresponding (tangent of the) subtended angle. Finally, use this to
		// compute the approximate analytic antialiasing factor sphere_alpha.
		Length ray_sphere_distance = kSphereRadius - sqrt(ray_sphere_center_squared_distance);
		Number ray_sphere_angular_distance = -ray_sphere_distance / p_dot_v;
		sphere_alpha = min(ray_sphere_angular_distance / fragment_angular_size, 1.0);

		//然后我们可以计算交点及其法线，并使用它们来获得此时收到的太阳和天空辐照度。 通过将辐照度与球体BRDF相乘，得到反射辐射：
		Position point = camera_ + view_direction * distance_to_intersection;
		Direction normal = normalize(point - kSphereCenter);

		// Compute the radiance reflected by the sphere.
		IrradianceSpectrum sky_irradiance;
		IrradianceSpectrum sun_irradiance = GetSunAndSkyIrradiance(point - earth_center_, normal, sun_direction_, sky_irradiance);
		sphere_radiance = sphere_albedo_ * (1.0 / (PI * sr)) * (sun_irradiance + sky_irradiance);

		//最后，我们考虑了相机和球体之间的空中视角，这取决于阴影中这段的长度：
		Length shadow_length =
			max(0.0 * m, min(shadow_out, distance_to_intersection) - shadow_in);
		DimensionlessSpectrum transmittance;
		RadianceSpectrum in_scatter = GetSkyRadianceToPoint(camera_ - earth_center_,
			point - earth_center_, shadow_length, sun_direction_, transmittance);
		sphere_radiance = sphere_radiance * transmittance + in_scatter;
	}

	/*
	在下面我们重复上面相同的步骤，但是对于行星球体P而不是球体S（这里不需要平滑的不透明度，所以我们不计算它。
	还要注意我们如何调制太阳和天空的辐照度 太阳和天空能见度因素接收在地面上）：
	*/

	/*
	计算视线和地球中心之间的距离，
	以及相机和view ray和地面交点的距离（如果没有交叉点则为NaN）
	*/

	p = camera_ - earth_center_;
	p_dot_v = dot(p, view_direction);
	p_dot_p = dot(p, p);
	Area ray_earth_center_squared_distance = p_dot_p - p_dot_v * p_dot_v;
	distance_to_intersection = -p_dot_v - sqrt(earth_center_.z * earth_center_.z - ray_earth_center_squared_distance);

	// Compute the radiance reflected by the ground, if the ray intersects it.
	Number ground_alpha = 0.0;
	RadianceSpectrum ground_radiance = RadianceSpectrum(0.0 * watt_per_square_meter_per_sr_per_nm);
	if (distance_to_intersection > 0.0 * m) 
	{
		Position point = camera_ + view_direction * distance_to_intersection;
		//现在有个问题，view ray和地面交点p，可能计算出来p到地心的距离为6359.999999，这个误差需要处理
		//判断point到地心距离
		Length pointDisToEarthCenter = length(point - earth_center_);
		if (pointDisToEarthCenter < atmosphere_parameters_.bottom_radius)
		{
			Length dx = atmosphere_parameters_.bottom_radius - pointDisToEarthCenter;
			Direction vec = (point - earth_center_) / pointDisToEarthCenter;
			point = point + vec * dx * 1.5;

			//Length pointDisToEarthCenter2 = length(point - earth_center_);
			//int ii = 0;
		}
		Direction normal = normalize(point - earth_center_);

		// Compute the radiance reflected by the ground.
		IrradianceSpectrum sky_irradiance;
		IrradianceSpectrum sun_irradiance = GetSunAndSkyIrradiance(
			point - earth_center_, normal, sun_direction_, sky_irradiance);
		ground_radiance = ground_albedo_ * (1.0 / (PI * sr)) * (
			sun_irradiance * GetSunVisibility(point, sun_direction_) +
			sky_irradiance * GetSkyVisibility(point));

		Length shadow_length =
			max(0.0 * m, min(shadow_out, distance_to_intersection) - shadow_in);
		DimensionlessSpectrum transmittance;
		RadianceSpectrum in_scatter = GetSkyRadianceToPoint(camera_ - earth_center_,
			point - earth_center_, shadow_length, sun_direction_, transmittance);
		ground_radiance = ground_radiance * transmittance + in_scatter;
		ground_alpha = 1.0;
	}
	//最后，我们计算天空的辐射度和透射率，并从后到前合成场景中所有物体的辐射和不透明度：
	// Compute the radiance of the sky.
	Length shadow_length = max(0.0 * m, shadow_out - shadow_in);
	DimensionlessSpectrum transmittance;
	RadianceSpectrum radiance = GetSkyRadiance(
		camera_ - earth_center_, view_direction, shadow_length, sun_direction_,
		transmittance);

	// If the view ray intersects the Sun, add the Sun radiance.
	if (dot(view_direction, sun_direction_) > sun_size_.y)
	{
		radiance = radiance + transmittance * GetSolarRadiance();
	}
	radiance = radiance * (1.0 - ground_alpha) + ground_radiance * ground_alpha;
	radiance = radiance * (1.0 - sphere_alpha) + sphere_radiance * sphere_alpha;
	return radiance;
}