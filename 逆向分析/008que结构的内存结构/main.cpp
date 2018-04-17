#include <stdio.h>
#include <queue>

using namespace std;

//队列数据
struct QueData
{
	int nData[4];
};

//模拟队列数据结构
struct MyQue
{
	struct MyQue** pSelf;
	QueData** pPointerArry; //指针数组
	int nArryNum; //指针数组的大小
	int nUnknow;
	int nElementNum; //元素数量
};

int main(int argc, char* argv[])
{
	queue<int> intQue;
	intQue.push(1);
	intQue.push(2);
	intQue.push(3);
	intQue.push(4);
	intQue.push(5);
	intQue.push(6);

	for (int i = 100; i < 1000; i++)
	{
		intQue.push(i);
	}

	MyQue *pMyQue = (MyQue*)&intQue;
	int nElementNum = 0;
	int nArryrIndex = 0;
	while (nElementNum < pMyQue->nElementNum && nArryrIndex < pMyQue->nArryNum)
	{
		QueData* pQueData = pMyQue->pPointerArry[nArryrIndex];
		for (int i = 0; i < _countof(pQueData->nData) && nElementNum < pMyQue->nElementNum; i++)
		{
			printf("value:%d\n", pQueData->nData[i]);
			nElementNum++;
		}
		nArryrIndex++;
	}
	system("pause");
	return 0;
}