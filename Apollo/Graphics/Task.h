#pragma once

namespace Apollo
{
	class Task
	{
	public:
		Task();
		virtual ~Task();

		// Update and draw functions.  The update function allows the view to use
		// time varying parameters.  Any animation or time varying quantities should
		// be updated here.  The draw function will be used to set the necessary
		// render states and render the geometry that is stored in the object list.

		virtual void Update(float fTime) = 0;
		virtual void QueuePreTasks(RendererDX11* pRenderer) = 0;
		virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager) = 0;

		// The render view must set all of its rendering parameters needed to 
		// execute itself with the SetRenderParams function.  The SetUsageParams
		// is used to set semantics that are needed to use the output of the 
		// render view by other objects.

		virtual void SetRenderParams(IParameterManager* pParamManager) = 0;
		virtual void SetUsageParams(IParameterManager* pParamManager) = 0;

		virtual std::wstring GetName() = 0;
	};
}
