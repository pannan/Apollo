#pragma once

namespace Apollo
{
	class Material
	{
	public:

		virtual void bind()const = 0;

		virtual void unBind()const = 0;

		std::string					m_albedoMap;
		std::string					m_normalMap;

	protected:
	private:
	};

	typedef std::shared_ptr<Material> MaterialPtr;
}