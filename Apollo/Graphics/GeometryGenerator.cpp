#include "stdafx.h"
#include "GeometryGenerator.h"
#include "LogManager.h"

NAME_SPACE_BEGIN_APOLLO

void GeometryGenerator::generatorSphere(int phiStepCount,int thetaStepCount,std::vector<Vector3>& outPos,std::vector<unsigned short>& outIndex)
{
	if (phiStepCount <= 1 || thetaStepCount <= 1)
	{
		LogManager::getInstance().log("phiStepCount <= 1 || thetaStepCount <= 1");
		return;
	}

	const size_t vertexCount = phiStepCount * thetaStepCount;
	outPos.resize(vertexCount);

	//theta [0,2 * PI] phi[0,PI]
	for (int phiStep = 0; phiStep < phiStepCount; ++phiStep)
	{
		for (int thetaStep = 0; thetaStep < thetaStepCount; ++thetaStep)
		{
			float fPhi = (float)phiStep / phiStepCount;
			float phiRadian = fPhi * APOLLO_PI;
			float y = cos(phiRadian);
			float xz = sin(phiRadian);

			float fTheta = (float)thetaStep / thetaStepCount;
			float thetaRadian = fTheta * 2 * APOLLO_PI;
			float x = xz * cos(thetaRadian);
			float z = xz * sin(thetaRadian);

			size_t index = phiStep * thetaStepCount + thetaStep;
			outPos[index] = Vector3(x, y, z);
		}
	}

	//两个三角形组成一个四边形
	size_t indexCount = (phiStepCount - 1) * (thetaStepCount - 1) * 6;
	outIndex.resize(indexCount);

	for (int phiStep = 0; phiStep < phiStepCount - 1; ++phiStep)
	{
		for (int thetaStep = 0; thetaStep < thetaStepCount - 1; ++thetaStep)
		{
			size_t indexBuffer0 = phiStep * thetaStepCount + thetaStep;
			size_t indexBuffer1 = (phiStep + 1) * thetaStepCount + thetaStep;
			size_t indexBuffer2 = (phiStep + 1) * thetaStepCount + thetaStep + 1;
			size_t indexBuffer3 = phiStep * thetaStepCount + thetaStep + 1;

			size_t index = phiStep * (thetaStepCount - 1) * 6 + thetaStep * 6;
			outIndex[index] = indexBuffer0;
			outIndex[index + 1] = indexBuffer1;
			outIndex[index + 2] = indexBuffer2;

			outIndex[index + 3] = indexBuffer2;
			outIndex[index + 4] = indexBuffer3;
			outIndex[index + 5] = indexBuffer0;
		}
	}
}

NAME_SPACE_END