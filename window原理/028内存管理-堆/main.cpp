// 内存管理_堆.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

class Tank {

private:

	static HANDLE	m_hHeap;				// 堆的句柄
	static int		m_nHeapReferenceCount;  // 在堆上创建对象的个数

private:
	int m_nX;
	int m_nY;
	int m_nDirection;
	int m_nId;

public:

	// 重载new运算符
	void* operator new(size_t nSize){

		// 创建堆
		if (m_nHeapReferenceCount == 0) {
			// 创建堆.
			m_hHeap = HeapCreate(0, 0, 0);
		}

		// 增加堆中的对象个数.
		++m_nHeapReferenceCount;

		// 在堆中分配一块内存来保存对象.
		return HeapAlloc(m_hHeap, 0, sizeof(Tank));
	}

	// 重载delete运算符
	void operator delete(void* pObj) {

		if (pObj == NULL)
			return;

		// 在堆中释放对象的空间.
		HeapFree(m_hHeap, 0, pObj);

		// 减掉一个堆中对象个数.
		--m_nHeapReferenceCount;

		// 如果堆中没有对象了, 销毁堆.
		if (m_nHeapReferenceCount <= 0) {

			// 销毁堆.
			HeapDestroy(m_hHeap);
			m_hHeap = NULL;
		}
	}
};


HANDLE	Tank::m_hHeap;				  // 堆的句柄
int		Tank::m_nHeapReferenceCount;  // 在堆上创建对象的个数


int _tmain(int argc, _TCHAR* argv[])
{
	//HeapCreate HeapAlloc(new), HeapFree(delete);
	//HANDLE hHeap = HeapCreate(0, 0, 0);
	//int* pArray = (int*)HeapAlloc(hHeap,
	//							  0, 
	//							  10 * sizeof(int)
	//							);
	HANDLE	hHeap = NULL;
	Tank*	pTank = NULL;


	// 创建一个堆
	hHeap = HeapCreate(0, /* 创建堆的标志, 不使用任何特殊标志 */
		0, /* 堆的初始大小, 如果是0,则默认为一个内存分页的大小 */
		0   /* 堆的最大大小, 如果是0,则堆的大小可以自动增长.*/
		);
	if (hHeap == NULL) {
		printf("创建堆失败\n");
		return 0;
	}

	// 从堆中分配内存, 就如同使用new 或 malloc一样.
	pTank = (Tank*)HeapAlloc(hHeap,			  /*堆的句柄*/
		HEAP_ZERO_MEMORY, /*分配时把内存初始化为0*/
		sizeof(Tank)     /*分配的大小*/
		);

	if (pTank == NULL) {
		printf("在堆中申请内存失败\n");
	}

	HeapFree(hHeap, 0, pTank);


	// 使用重载的new 和 delete
	Tank* pTank1 = new Tank();
	delete pTank1;
	return 0;
}

