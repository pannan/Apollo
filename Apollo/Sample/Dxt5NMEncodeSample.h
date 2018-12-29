#pragma once

#include "SampleBase.h"

namespace Apollo
{
	class Dxt5NMEnCodeSample : public SampleBase
	{
	public:

		Dxt5NMEnCodeSample();
		~Dxt5NMEnCodeSample();

		virtual void  init();

		virtual void	render(){}

		virtual Camera* getCamera() { return nullptr; }

	protected:


	protected:

	private:
	};
}
