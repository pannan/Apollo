// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Custom/RealSky" {
	Properties
	{
		_Color("Color", Color) = (1,0,0,1)
		//inscatterSampler("Texture", 3D) = "" {}
		//transmittanceSampler("Texture", 2D) = "" {}
		[HideInInspector]Rg("Rg", Float) = 6360
		[HideInInspector]Rt("Rt", Float) = 6420
		[HideInInspector]RL("RL", Float) = 6421
		[HideInInspector]M_PI("PI",Float) = 3.1415936
		//ISun("Sun Power",Range(0,50)) = 30

		[HideInInspector]RES_R("RES_R",Int) = 32
		[HideInInspector]RES_MU("RES_MU",Int) = 128
		[HideInInspector]RES_MU_S("RES_MU_S",Int) = 32
		[HideInInspector]RES_NU("RES_NU",Int) = 8

		betaR("betaR",Vector) = (0.0058,0.0135,0.0331)// float3(5.8e-3, 1.35e-2, 3.31e-2);
		betaMSca("betaMSca",Vector) = (0.03,0.03,0.03)//
		betaMEx("betaMEx",Vector) = (0.0333,0.0333,0.0333)// earthEyePos

		[HideInInspector]earthEyePos("earthEyePos",Vector) = (0,6361,0)// 

		[HideInInspector]HR("HR",Float) = 8.0
		[HideInInspector]HM("HM",Float) = 1.0
		//[HideInInspector]mieG("mieG",Float) = 0.99

		//sunPhi("sunPhi",Range(0,90)) = 89
		//sunTheta("sunTheta",Range(0,360)) = 270

		[HideInInspector]TRANSMITTANCE_INTEGRAL_SAMPLES("TRANSMITTANCE_INTEGRAL_SAMPLES",Int) = 8
		[HideInInspector]INSCATTER_INTEGRAL_SAMPLES("INSCATTER_INTEGRAL_SAMPLES",Int) = 8
		[HideInInspector]IRRADIANCE_INTEGRAL_SAMPLES("IRRADIANCE_INTEGRAL_SAMPLES",Int) = 2
		[HideInInspector]INSCATTER_SPHERICAL_INTEGRAL_SAMPLES("INSCATTER_SPHERICAL_INTEGRAL_SAMPLES",Int) = 2

		/////cloud//////
		_StartHeight("CloudStartHeight",Float) = 1500
		cloudEndHeight("cloudEndHeight",Float) = 4000
		_AtmosphereThickness("AtmosphereThickness",Float) = 2500
		_LODDistance("LODDistance",Float) = 0.3
		_DetailScale("DetailScale",Float) = 8.0
		_CloudDistortion("CloudDistortion",Float) = 0.35
		_CloudBottomFade("CloudBottomFade",Float) = 0.3

		_DarkOutlineScalar("DarkOutlineScalar",Float) = 1
		_AmbientScalar("AmbientScalar",Float) = 1.23
		_RayMinimumY("RayMinimumY",Float) = 0.0
		_HorizonFadeScalar("HorizonFadeScalar",Float) = 0.18
		_HorizonFadeStartAlpha("HorizonFadeStartAlpha",Float) = 0.76
		_OneMinusHorizonFadeStartAlpha("OneMinusHorizonFadeStartAlpha",Float) = 0.24
		_SampleScalar("SampleScalar",Float) = 1
	}

	SubShader
	{
		//Tags{ "RenderType" = "Background" }
		LOD 200
		Cull Off ZWrite Off ZTest Always

	Pass
	{
		//Stencil
		//{
		//Ref 1        //参考值为2，stencilBuffer值默认为0  
		//Comp NotEqual            //stencil比较方式是永远通过  
		//Pass Keep           //pass的处理是替换，就是拿2替换buffer 的值  
		//					   //ZFail decrWrap<span style = "white-space:pre">      < / span>//ZFail的处理是溢出型减1  
		//}

		CGPROGRAM
#pragma vertex vert
#pragma fragment frag

#include "UnityCG.cginc"
#include "SkyBase.cginc"  
#include "CloudBase.cginc"  
#include "HDR.cginc"  

	float3 earthEyePos;
	float3 sunDirection;

	float sunPhi;
	float sunTheta;

	float M_PI;// = 3.141592657;

	////////////////////////////////////////////////////////////////////////////////
	float4 texture4D(sampler3D table, float r, float mu, float muS, float nu)
	{
		float H = sqrt(Rt * Rt - Rg * Rg);
		float rho = sqrt(r * r - Rg * Rg);
#ifdef INSCATTER_NON_LINEAR
		float rmu = r * mu;
		float delta = rmu * rmu - r * r + Rg * Rg;
		float4 cst = rmu < 0.0 && delta > 0.0 ? float4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : float4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));
		float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
		float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(RES_MU));
		// paper formula
		//float uMuS = 0.5 / float(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / float(RES_MU_S));
		// better formula
		float uMuS = 0.5 / float(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));
#else
		float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
		float uMu = 0.5 / float(RES_MU) + (mu + 1.0) / 2.0 * (1.0 - 1.0 / float(RES_MU));
		float uMuS = 0.5 / float(RES_MU_S) + max(muS + 0.2, 0.0) / 1.2 * (1.0 - 1.0 / float(RES_MU_S));
#endif
		float lerp = (nu + 1.0) / 2.0 * (float(RES_NU) - 1.0);
		float uNu = floor(lerp);
		lerp = lerp - uNu;
		return	tex3D(table, float3((uNu + uMuS) / (RES_NU), uMu, uR)) * (1.0 - lerp) +
		tex3D(table, float3((uNu + uMuS + 1.0) / (RES_NU), uMu, uR)) * lerp;
	}

	float phaseFunctionR(float mu)
	{
		return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
	}

	float phaseFunctionM(float mu)
	{
		return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG*mieG) * pow(1.0 + (mieG*mieG) - 2.0*mieG*mu, -3.0 / 2.0) * (1.0 + mu * mu) / (2.0 + mieG*mieG);
	}

	float2 getTransmittanceUV(float r, float mu)
	{
		float uR, uMu;
#ifdef TRANSMITTANCE_NON_LINEAR
		uR = sqrt((r - Rg) / (Rt - Rg));
		uMu = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;
#else
		uR = (r - Rg) / (Rt - Rg);
		uMu = (mu + 0.15) / (1.0 + 0.15);
#endif
		return float2(uMu, uR);
	}

	float3 preComputeTransmittance(float r, float mu)
	{
		float2 uv = getTransmittanceUV(r, mu);
		return tex2D(transmittanceSampler, uv).rgb;
	}

	float3 preCompTransmittanceWithShadow(float r, float mu)
	{
		return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? float3(0, 0, 0) : preComputeTransmittance(r, mu);
	}

	float3 preComputeTransmittance(float r, float mu, float3 v, float3 x0)
	{
		float3 result;
		float r1 = length(x0);
		float mu1 = dot(x0, v) / r;
		if (mu > 0.0)
		{
			result = min(preComputeTransmittance(r, mu) / preComputeTransmittance(r1, mu1), 1.0);
		}
		else
		{
			result = min(preComputeTransmittance(r1, -mu1) / preComputeTransmittance(r, -mu), 1.0);
		}
		return result;
	}

	float3 getMie(float4 rayMie)
	{
		return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (betaR.r / betaR);
	}

	float3 preComputeInscatter(inout float3 x, inout float t, float3 v, float3 s, out float r, out float mu, out float3 attenuation)
	{
		float3 result;
		r = length(x);
		mu = dot(x, v) / r;
		float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);

		if (d > 0.0)
		{
			x += d * v;
			t -= d;
			mu = (r * mu + d) / Rt;
			r = Rt;
		}
		if (r <= Rt)
		{
			float nu = dot(v, s);
			float muS = dot(x, s) / r;
			float phaseR = phaseFunctionR(nu);
			float phaseM = phaseFunctionM(nu);

			float4 inscatter = max(texture4D(inscatterSampler, r, mu, muS, nu), 0.0);
			if (t > 0.0)
			{
				float3 x0 = x + t * v;
				float r0 = length(x0);
				float rMu0 = dot(x0, v);
				float mu0 = rMu0 / r0;
				float muS0 = dot(x0, s) / r0;

				attenuation = preComputeTransmittance(r, mu, v, x0);

				if (r0 > Rg + 0.01)
					inscatter = max(inscatter - attenuation.rgbr * texture4D(inscatterSampler, r0, mu0, muS0, nu), 0.0);
			}

			result = max(inscatter.rgb * phaseR + getMie(inscatter) * phaseM, 0.0);
		}
		else
			result = float3(0, 0, 0);

		return result * ISun;
	}

	float3 preComputeSunColor(float3 x, float t, float3 v, float3 s, float r, float mu)
	{
		if (t > 0.0) {
			return float3(0, 0, 0);
		}
		else
		{
			float3 transmittance = r <= Rt ? preCompTransmittanceWithShadow(r, mu) : float3(1.0, 1, 1); 
			float isun = step(cos(M_PI / 180.0), dot(v, s)) * ISun; 
			return transmittance * ISun;
		}
	}

	float limit(float r, float mu)
	{
		float dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
		float delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
		if (delta2 >= 0.0)
		{
			float din = -r * mu - sqrt(delta2);
			if (din >= 0.0)
			{
				dout = min(dout, din);
			}
		}
		return dout;
	}

	//这里求p点ray(r,mu)和大气层交点i之间的透视率 T(p,i)
	float opticalDepth(float H, float r, float mu)
	{
		float result = 0.0;
		float dx = limit(r, mu) / float(TRANSMITTANCE_INTEGRAL_SAMPLES);
		float xi = 0.0;
		float yi = exp(-(r - Rg) / H);
		for (int i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i)
		{
			float xj = float(i) * dx;
			float yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - Rg) / H);
			result += (yi + yj) / 2.0 * dx;
			xi = xj;
			yi = yj;
		}
		//这里判断ray(r,mu)是否和地面相交
		return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 1e9 : result;
	}


	float3 transmittanceWithShadow(float r, float mu)
	{
		return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? float3(0, 0, 0) : transmittance(r, mu);
	}
	float3 irradiance(float r, float muS)
	{
		return transmittance(r, muS) * max(muS, 0.0);
	}

	float3 sunColor(float3 x, float t, float3 v, float3 s, float r, float mu)
	{
		if (t > 0.0)
		{
			return float3(0, 0, 0);
		}
		else
		{
			float3 transmittance = r <= Rt ? transmittanceWithShadow(r, mu) : float3(0, 1, 0); // T(x,xo)
			float isun = step(cos(M_PI / 180.0), dot(v, s)) * ISun; // Lsun
			return transmittance * isun; 
		}
	}

	float3 groundColor(float3 x, float t, float3 v, float3 s, float r, float mu, float3 attenuation)
	{
		float3 result;
		if (t > 0.0) 
		{
			float3 x0 = x + t * v;
			float r0 = length(x0);
			float3 n = x0 / r0;
			
			float4 albedoColor = float4(1, 1, 1, 0);
			float4 reflectance = albedoColor * float4(0.2, 0.2, 0.2, 1.0);
			if (r0 > Rg + 0.01)
			{
				reflectance = float4(0.4, 0.4, 0.4, 0.0);
			}

		
			float muS = dot(n, s);
			float3 sunLight = transmittanceWithShadow(r0, muS);

			
			float3 groundSkyLight = irradiance(r0, muS);

			
			float3 groundColor = reflectance.rgb * (max(muS, 0.0) * sunLight) * ISun / M_PI;

			if (1)
			{
				float3 h = normalize(s - v);
				float fresnel = 0.02 + 0.98 * pow(1.0 - dot(-v, h), 5.0);
				float waterBrdf = fresnel * pow(max(dot(h, n), 0.0), 150.0);
				//groundColor += reflectance.w * max(waterBrdf, 0.0) * sunLight * ISun;
			}

			result = attenuation * groundColor; 
		}
		else 
		{
			result = float3(0, 0, 0);
		}
		
		return result;
	}

	float3 transmittance(float r, float mu)
	{
		float3 depth = betaR * opticalDepth(HR, r, mu) + betaMEx * opticalDepth(HM, r, mu);
		return float3(exp(-depth));
	}

	float3 transmittance(float r, float mu, float d)
	{
		float3 result;
		float r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);
		float mu1 = (r * mu + d) / r1;
		if (mu > 0.0)
		{
			result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);
		}
		else
		{
			result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);
		}
		return result;
	}


	void integrand(float r, float mu, float muS, float nu, float t, out float3 ray, out float3 mie)
	{
		ray = float3(0, 0, 0);
		mie = float3(0, 0, 0);
		float ri = sqrt(r * r + t * t + 2.0 * r * mu * t);
		float muSi = (nu * t + muS * r) / ri;
		ri = max(Rg, ri);
		if (muSi >= -sqrt(1.0 - Rg * Rg / (ri * ri)))
		{
			float3 ti = transmittance(r, mu, t) * transmittance(ri, muSi);
			ray = exp(-(ri - Rg) / HR) * ti;
			mie = exp(-(ri - Rg) / HM) * ti;
		}
	}

	void inscatter(float r, float mu, float muS, float nu, out float3 ray, out float3 mie)
	{
		ray = float3(0, 0, 0);
		mie = float3(0, 0, 0);
		float interectD = limit(r, mu);
		float dx = interectD / float(INSCATTER_INTEGRAL_SAMPLES);
		float xi = 0.0;
		float3 rayi;
		float3 miei;
		integrand(r, mu, muS, nu, 0.0, rayi, miei);
		for (int i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i)
		{
			float xj = float(i) * dx;
			float3 rayj;
			float3 miej;
			integrand(r, mu, muS, nu, xj, rayj, miej);
			ray += (rayi + rayj) / 2.0 * dx;
			mie += (miei + miej) / 2.0 * dx;
			xi = xj;
			rayi = rayj;
			miei = miej;
		}
		ray *= betaR;
		mie *= betaMSca;
	}

	float3 transmittance(float r, float mu, float3 v, float3 x0)
	{
		float3 result;
		float r1 = length(x0);
		float mu1 = dot(x0, v) / r;
		if (mu > 0.0)
		{
			result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);
		}
		else
		{
			result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);
		}
		return result;
	}

	float3 inscatterC(inout float3 x, inout float t, float3 v, float3 s, out float r, out float mu, out float3 attenuation)
	{
		float3 result;
		r = length(x);
		mu = dot(x, v) / r;
		float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);

		if (d > 0.0) 
		{
			
			x += d * v;
			t -= d;
			mu = (r * mu + d) / Rt;
			r = Rt;
		}
		if (r <= Rt)
		{
			float nu = dot(v, s);
			float muS = dot(x, s) / r;
			float phaseR = phaseFunctionR(nu);
			float phaseM = phaseFunctionM(nu);

			//float4 inscatter = max(texture4D(inscatterSampler, r, mu, muS, nu), 0.0);
			float3 ray = float3(0, 0, 0);
			float3 mie = float3(0, 0, 0);
			inscatter(r, mu, muS, nu, ray, mie);

			if (t > 0.0)//地面
			{
				float3 x0 = x + t * v;
				float r0 = length(x0);
				float rMu0 = dot(x0, v);
				float mu0 = rMu0 / r0;
				float muS0 = dot(x0, s) / r0;

				attenuation = transmittance(r, mu, v, x0);

				if (r0 > Rg + 0.01)
				{					
					float3 ray0 = float3(0, 0, 0);
					float3 mie0 = float3(0, 0, 0);
					inscatter(r0, mu0, muS0, nu, ray0, mie0);
					ray = max(ray - attenuation.rgb * ray0, 0);
					mie = max(mie - attenuation.rgb * mie0, 0);	
				}		
			}
		
			//result = max(inscatter.rgb * phaseR + getMie(inscatter) * phaseM, 0.0);
			result = max(ray * phaseR + mie * phaseM, 0.0);
		}
		else// x in space and ray looking in space
		{
			result = float3(1, 0, 0);
		}

		return result * ISun;
	}

	///////////////////////////////////////cloud//////////////////////////////////////////////////////

	float4 cloudRender(float3 eyeDir,float3 sunDir)
	{
		float3 rayDirection = eyeDir;
		float4 color = float4(0.0, 0.0, 0.0, 0.0);
		float hMaxSampler = _MaxIterations;
		float vMaxSampler = _MaxIterations * 0.5f;
		/*float4 noiseSample = tex3D(_Detail3D, eyeDir);
		return noiseSample;*/
		if (rayDirection.y > 0.0f)
		{
			//float2 uv = input.texcoord;
			float3 samplerPoint = InternalRaySphereIntersect(_EarthRadius + _StartHeight, _CameraPosition, rayDirection);
			float3 rayStep = rayDirection * _RayStepLength;
			float i = 0;

			float atmosphereY = 0.0;
			float transmittance = 1.0;
			float rayStepScalar = 1.0;
			float rayBigStepScalar = 10;

			float cosAngle = dot(rayDirection, sunDir);

			float normalizedDepth = 0.0;
			float zeroThreshold = 4.0;
			float zeroAccumulator = 0.0;
			const float3 RandomUnitSphere[6] = { _Random0, _Random1, _Random2, _Random3, _Random4, _Random5 };
			float value = 1.0;
			//float realMaxIterations = lerp(hMaxSampler, vMaxSampler, rayDirection.y);
			//bool insideCloud = false;

			while (true)
			{
				if (i >= _MaxIterations /*realMaxIterations*/ || color.a >= 1.0 || atmosphereY >= 1.0)
					break;

				normalizedDepth = distance(_CameraPosition, samplerPoint) / _MaxDistance;
				//return float4(normalizedDepth, normalizedDepth, normalizedDepth,1);
				float lod = step(_LODDistance, normalizedDepth);
				float4 coverage = SampleCoverage(samplerPoint);

				value = SampleCloud(samplerPoint, color.a, coverage, atmosphereY, lod);
				float4 particle = float4(value, value, value, value);


				if (value > 0.0)
				{
					//return float4(atmosphereY, atmosphereY, atmosphereY, 1);
					//return float4(value, value, value, 1);
					zeroAccumulator = 0.0;
					float T = 1.0 - particle.a;
					transmittance *= T;

					float3 ambientLight = SampleAmbientLight(atmosphereY, normalizedDepth);
					//return float4(ambientLight,1);
					float3 sunLight = SampleLight(sunDir,samplerPoint, particle.a, color.a, cosAngle, normalizedDepth, RandomUnitSphere);
					//return float4(hdr(sunLight),1);
					sunLight *= _LightScalar;
					ambientLight *= _AmbientScalar;

					particle.a = 1.0 - T;
					particle.rgb = sunLight + ambientLight;
					particle.rgb *= particle.a;

					color = (1.0 - color.a) * particle + color;
				}

				zeroAccumulator += float(value <= 0.0);
				//rayStepScalar = 1.0 + step( zeroThreshold, zeroAccumulator) * 0.0;
				i += rayStepScalar;

				samplerPoint += rayStep * rayStepScalar;
				atmosphereY = NormalizedAtmosphereY(samplerPoint);
			}

			float fade = smoothstep(_RayMinimumY,
				_RayMinimumY + (1.0 - _RayMinimumY) * _HorizonFadeScalar, rayDirection.y);
			color *= _HorizonFadeStartAlpha + fade * _OneMinusHorizonFadeStartAlpha;

			//float samplerCount = i / _MaxIterations;
			//return float4(samplerCount, samplerCount, samplerCount, 1);
		}

		return color;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

	float4x4 projMat;
	float4x4 inverProjMat;
	float4x4 viewMat;
	float4x4 inverViewMat;

	float4 g_skyColor;
	inline float3 UVToCameraRay(float2 uv)
	{
		float4 cameraRay = float4(uv * 2.0 - 1.0, 1.0, 1.0);
		cameraRay = mul(inverProjMat, cameraRay);
		cameraRay = cameraRay / cameraRay.w;

		return mul((float3x3)inverViewMat, cameraRay.xyz);
	}

	struct appdata
	{
		float4 vertex : POSITION;
	};

	struct v2f
	{
		float3 worldPos : TEXCOORD0;
	//	float3 sunDir : TEXCOORD1;
		float3 eyeDir : TEXCOORD1;
		float4 vertex : SV_POSITION;
	};


	v2f vert(appdata_img v)
	{
		v2f o;
		o.vertex = UnityObjectToClipPos(v.vertex);
		o.worldPos = mul(unity_ObjectToWorld, v.vertex);

		//sun direction
		//float phiRanian = sunPhi * M_PI / 180.0f;
		//float thetaRanian = sunTheta * M_PI / 180.0f;
		/*float3 sunDir;
		sunDir.x = sin(phiRanian) * cos(thetaRanian);
		sunDir.y = cos(phiRanian);
		sunDir.z = sin(phiRanian) * sin(thetaRanian);
		o.sunDir = normalize(sunDir);*/
		o.eyeDir = UVToCameraRay(v.texcoord);
		return o;
	}

	float4 frag(v2f i) : SV_Target
	{
		float3 rayStart = _WorldSpaceCameraPos;
		float3 rayEnd = i.worldPos;
	//	return float4(1, 0, 0, 1);
		float3 eyeDir = i.eyeDir;// (rayEnd - rayStart);
		//float rayLength = length(eyeDir);

		//eyeDir /= rayLength;
		//return float4(1, 1, 0, 1);
		//float4 color = float4(tex2D(transmittanceSampler, float2(1,0)).xyz,1);
		//return color;
		//float4 color = tex3D(inscatterSampler, eyeDir);
		//return float4(color.xyz,1);
		//float dd = dot(eyeDir, sunDirection.xyz);
		//return float4(dd, dd, dd,1);
		float3 v = normalize(eyeDir);

		float4 cloudColor = cloudRender(v, sunDirection.xyz);

		//return float4(cloudColor.xyz,1);
		float3 x = earthEyePos.xyz;
		float r = length(x);
		//float yyy = r / (Rt * 5);
		//return float4(yyy, yyy, yyy, 1);
		float mu = dot(x, v) / r;
		float3 s = sunDirection.xyz; //i.sunDir;// 
		float t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rg * Rg);

		float3 g = x - float3(0.0, 0.0, Rg + 10.0);
		float a = v.x * v.x + v.y * v.y - v.z * v.z;
		float b = 2.0 * (g.x * v.x + g.y * v.y - g.z * v.z);
		float c = g.x * g.x + g.y * g.y - g.z * g.z;
		float d = -(b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
		bool cone = d > 0.0 && abs(x.z + d * v.z - Rg) <= 10.0;

		if (t > 0.0)
		{
			if (cone && d < t)
				t = d;
		}
		else if (cone)
			t = d;

		float3 attenuation;
		float3 inscatterColor = inscatterC(x, t, v, s, r, mu, attenuation);// preComputeInscatter(x, t, v, s, r, mu, attenuation); //S[L]-T(x,xs)S[l]|xs
		float3 sc = sunColor(x, t, v, s, r, mu);// preComputeSunColor(x, t, v, s, r, mu); //L0
		float3 groundColorVec3 = groundColor(x, t, v, s, r, mu, attenuation); //R[L0]+R[L*]
																			  //return float4(inscatterColor + sunColor + groundColorVec3, 1.0);
		//return float4(HDR(inscatterColor), 1);
		float3 skyColor = inscatterColor + sc + groundColorVec3;
		float3 finalColor = cloudColor.xyz + (1 - cloudColor.a) * skyColor * _LightColor.xyz;
		float3 outColor = HDR(finalColor);
		//float3 outColor = inverGamma(ACESToneMapping(inscatterColor + sunColor + groundColorVec3));
		//float3 outColor = inscatterColor + sunColor + groundColorVec3;
		//return RGBMEncode(inscatterColor + sunColor + groundColorVec3);
		return float4(outColor, 1.0);
	}
		ENDCG
	}
	}
		Fallback off
}
