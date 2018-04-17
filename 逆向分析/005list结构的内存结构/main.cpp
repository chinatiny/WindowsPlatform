#include <stdio.h>
#include <list>

using namespace std;

struct MyNode
{
	struct MyNode* pNext;
	struct MyNode* pPrev;
	int nData;
};

struct MyList
{
#ifdef _DEBUG
	struct MyList* pSelf;
#endif

	struct MyNode* pRoot;
	int nNodeCount;
};

int main(int argc, char* argv[])
{
	list<int> listObj;

	listObj.push_back(1);
	listObj.push_back(2);
	listObj.push_back(3);
	listObj.pop_back();

	listObj.push_back(4);

	MyList* pList = (MyList*)&listObj;
	int size = pList->nNodeCount;
	MyNode* pNode = pList->pRoot;
	while (pNode->pNext != pList->pRoot)
	{
		pNode = pNode->pNext;
		int n = pNode->nData;
		printf("ÔªËØ=%d\n", n);
	}
	return 0;
}