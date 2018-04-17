#include <windows.h>
#include <tchar.h>

int main(int argc, char* argv[])
{
	__try{
		_tprintf(_T("case1 in  try\n"));
	}
	__finally{
		_tprintf(_T("case1 in  finally\n"));
	}

	for (int i = 0; i < 2; i++)
	{
		__try{
			_tprintf(_T("case2 in  try\n"));
			continue;
		}
		__finally{
			_tprintf(_T("case2 in  finally\n"));
		}
	}

	for (int i = 0; i < 2; i++)
	{
		__try{
			_tprintf(_T("case3 in  try\n"));
			break;
		}
		__finally{
			_tprintf(_T("case3 in  finally\n"));
		}
	}

	__try{
		_tprintf(_T("case4 in  try\n"));
		goto __CASE4_OUT;
	}
	__finally{
		_tprintf(_T("case4 in  finally\n"));
	}

__CASE4_OUT:
	_tprintf(_T("case4 out\n"));


	__try{
		_tprintf(_T("case5 in  try\n"));
		__leave;
	}
	__finally{
		_tprintf(_T("case5 in  finally\n"));
	}

	__try{
		_tprintf(_T("case6 in  try\n"));
		return 1;
	}
	__finally{
		_tprintf(_T("case6 in  finally\n"));
	}

	system("pause");
	return 0;
}