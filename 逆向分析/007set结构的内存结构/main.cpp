#include <stdio.h>
#include <set>
using namespace std;

struct MySetNode
{
	struct MySetNode* pLeft;
	struct MySetNode* pParent;
	struct MySetNode* pRight;
	int unknown;
	int nValue;
};
struct MySet
{
#ifdef _DEBUG
	struct MySet** pSelf;
#endif
	struct MySetNode* pRoot;
	int nNodeCount;
};

void EnumMapNode(MySetNode* pNode, MySetNode* pRoot) 
{
	if (pNode == pRoot) return;
	//printf("key=%d, value=%d\n", pNode->nkey, pNode->nValue);
	EnumMapNode(pNode->pLeft, pRoot);
	printf("value=%d\n", pNode->nValue);
	EnumMapNode(pNode->pRight, pRoot);
}

int main(int argc, char* argv[])
{
	set<int> intSet;
	intSet.insert(1);
	intSet.insert(2);
	intSet.insert(3);
	intSet.insert(4);
	intSet.insert(5);

	MySet * pSet = (MySet*)&intSet;
	MySetNode* pNode = pSet->pRoot->pParent;
	EnumMapNode(pNode, pSet->pRoot);

	system("pause");
	return 0;
}