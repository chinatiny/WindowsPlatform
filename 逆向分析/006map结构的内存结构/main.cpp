#include <map>
using namespace std;

struct MyMapNode
{
	struct MyMapNode* pLeft;
	struct MyMapNode* pParent;
	struct MyMapNode* pRight;
	int unknown;
	int nkey;
	int nValue;
};
struct MyMap
{
#ifdef _DEBUG
	struct MyMap** pSelf;
#endif
	struct MyMapNode* pRoot;
	int nNodeCount;
};

void EnumMapNode(MyMapNode* pNode, MyMapNode* pRoot) {
	if (pNode == pRoot) return;
	//printf("key=%d, value=%d\n", pNode->nkey, pNode->nValue);
	EnumMapNode(pNode->pLeft, pRoot);
	printf("key=%d, value=%d\n", pNode->nkey, pNode->nValue);
	EnumMapNode(pNode->pRight, pRoot);
}

int main(int argc, char* argv[])
{
	map<int, int> mapObj;
	typedef pair <int, int> Int_Pair;
	mapObj.insert(Int_Pair(1, 0x11));
	mapObj.insert(Int_Pair(2, 0x22));
	mapObj.insert(Int_Pair(3, 0x33));
	mapObj.insert(Int_Pair(4, 0x44));
	mapObj.insert(Int_Pair(5, 0x55));
	mapObj.insert(Int_Pair(6, 0x66));

	MyMap* pMap = (MyMap*)&mapObj;
	MyMapNode* pNode = pMap->pRoot->pParent;
	EnumMapNode(pNode, pMap->pRoot);
	return 0;
}