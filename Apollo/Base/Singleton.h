#pragma once


namespace Apollo 
{
	template<class T, bool mustDelete = true>
	class Singleton
	{
		static T*	_instance;		/// ʵ����ָ̬��
	public:
		static T& getInstance()
		{
			if (!_instance)
			{
				_instance = new T;
				if (mustDelete) atexit(releaseInstance);
			}

			return *_instance;
		}

	protected:
		/// ʹ�ñ���������Ϊ���û�������ջ������һ��ʵ��
		Singleton() { }
		static void __cdecl releaseInstance()
		{
			if (_instance && mustDelete)
			{
				delete _instance;
				_instance = 0;
			}
		}
	};

	/// ��̬ʵ��ָ���ʼ��
	template <class T, bool mustDelete> T* Singleton<T, mustDelete>::_instance = NULL;



	/// ��չ�ĵ�ʵ��ģ�壬�����Ķ���Ĵ���������
	/// �����ⲿnew��delete�����ֵ�ʵ��ĺô����ⲿ�ܿ��ƹ����������˳��
	template <typename T>
	class SingletonEx
	{
	protected:
		static T*	_instance;

	public:
		SingletonEx()
		{
			assert(!_instance);
#ifdef RKT_WIN32
#	if defined(RKT_COMPILER_MSVC) && RKT_COMPILER_VER < 1200	 
			int offset = (int)(T*)1 - (int)(SingletonEx<T>*)(T*)1;
			_instance = (T*)((int)this + offset);
#	else
			_instance = static_cast<T*>(this);
#	endif
#else
			_instance = static_cast<T*>(this);
#endif
		}

		~SingletonEx()
		{
			assert(_instance);
			_instance = 0;
		}

		static T& getInstance() { assert(_instance); return (*_instance); }
		static T* getInstancePtr() { return _instance; }
	};

	template <typename T> T* SingletonEx<T>::_instance = 0;


}

