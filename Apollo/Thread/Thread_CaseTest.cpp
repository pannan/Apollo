#include "stdafx.h"
#include "Dimensional/test_case.h"
#include <thread>

NAME_SPACE_BEGIN_APOLLO

using namespace std;
void fun1(int n)  //��ʼ�����캯��  
{
	cout << "Thread " << n << " executing\n";
	n += 10;
	this_thread::sleep_for(chrono::milliseconds(10));
}
void fun2(int & n) //�������캯��  
{
	cout << "Thread " << n << " executing\n";
	n += 20;
	this_thread::sleep_for(chrono::milliseconds(10));
}

void show()
{
	cout << "hello cplusplus!" << endl;
}

const int N = 100000000;
int num = 0;
void run(int i)
{
	cout << "start run:" << i << endl;
	for (int i = 0; i < N; i++)
	{
		num++;
	}

	cout << "end run:" << i << endl;
}

class ThreadTest : public dimensional::TestCase
{
public:
	template<typename T>
	ThreadTest(const std::string& name, T test) : TestCase("ThreadTest " + name, static_cast<Test>(test))
	{
		
	}

	void		initTest()
	{
		int n = 0;
		thread t1;               //t1����һ��thread  
		thread t2(fun1, n + 1);  //����ֵ����  
		t2.join();
		cout << "n=" << n << '\n';
		n = 10;
		thread t3(fun2, ref(n)); //����  
		thread t4(move(t3));     //t4ִ��t3��t3����thread  
		t4.join();
		cout << "n=" << n << '\n';
	}

	void  initTest2()
	{
		//ջ��  
		thread t1(show);   //���ݺ�����ʼ��ִ��  
		thread t2(show);
		thread t3(show);
		//����Ҫ��join����Ȼ�����thread����������
		//join�������̵߳ȴ��������߳�ִ����ɲ��˳�
		//��Ȼ�������߳���ִ��ʱ���߳��Ѿ��˳�������������
		t1.join();
		t2.join();
		t3.join();

		bool b0 = t1.joinable();

		while (t1.joinable() == false)
		{

		}

		//�߳�����  
		//thread th[3]{ thread(show), thread(show), thread(show) };
		//����  
		/*thread *pt1(new thread(show));
		thread *pt2(new thread(show));
		thread *pt3(new thread(show));*/
		//�߳�ָ������  
	//	thread *pth(new thread[3]{ thread(show), thread(show), thread(show) });
		return ;
	}

	void  getCPUCount()
	{
		auto n = thread::hardware_concurrency();//��ȡcpu���ĸ���  
		cout << n << endl;
		return ;
	}

	void sumTest()
	{
		clock_t start = clock();
		thread t1(run,1);
		//join��������ǰ�̣߳�ֱ��this�߳�ִ�����
		t1.join();
		thread t2(run, 2);		
		t2.join();
		clock_t end = clock();
		cout << "num=" << num << ",��ʱ " << end - start << " ms" << endl;
		return;
	}
};

//ThreadTest createThreadTest("createThreadTest",	&ThreadTest::initTest);

//ThreadTest createThreadTest2("createThreadTest2", &ThreadTest::initTest2);

//ThreadTest initTest2("initTest2", &ThreadTest::initTest2);

//ThreadTest sumTest("sumTest", &ThreadTest::sumTest);


NAME_SPACE_END