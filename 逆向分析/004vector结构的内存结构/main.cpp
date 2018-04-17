#include <vector>
using namespace std;

struct MyVector
{
#ifdef _DEBUG
	struct MyVector** pSelf;
#endif
	int* pDataStart;
	int* pDataEnd;
	int* pBufEnd;
};

int main(int argc, char* argv[])
{

	// 动态数组，数据存储在堆内存中
	// 当元素发生改变之后，会动态增加内存。
	vector<int> vecObj;
	vecObj.push_back(1);
	vecObj.push_back(2);
	vecObj.push_back(3);
	vecObj.pop_back();
	vecObj.push_back(4);

	// 遍历vector
	for (size_t i = 0; i < vecObj.size(); i++)
	{
		printf("vecObj[%d] = %d\n", i, vecObj[i]);
	}
	// 遍历vector
	vector<int>::iterator iter = vecObj.begin();
	while (iter != vecObj.end())
	{
		int n = *iter;
		printf("vecObj i = %d\n", n);
		iter++;
	}

	// 定义结构，操作vector
	MyVector* pVector = (MyVector*)&vecObj;
	size_t size = pVector->pDataEnd - pVector->pDataStart;
	for (size_t i = 0; i < size; i++)
	{
		//pVector->pDataStart[i] = 3;
		int n = pVector->pDataStart[i];
		printf("元素=%d\n", n);
	}

	return 0;
}