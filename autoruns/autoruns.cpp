//  Главный Модуль

#include "stdafx.h"
#include "autoruns.h"
#include <vector>
#include "inputbox.h"
#include "stddialog.h"

#define MAX_LOADSTRING 100

// Глобальные переменные
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

HWND hWnd;
HWND hbutton1, hbutton2, hlistBox1;

// Отправить объявления функций, включенных в этот модуль кода ( Регистрация Класса )
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	
	MSG msg;
	HACCEL hAccelTable;

	// Подгрузка строк с названием приложения
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_AUTORUNS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения, если нет, F
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTORUNS));

	// Сообщение о нахождения окна в системе
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}




//  ФУНКЦИЯ: MyRegisterClass()
//	Создание окна
//  НАЗНАЧЕНИЕ: регистрирует класс окна.

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTORUNS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_AUTORUNS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//	Инициализация окна
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 620, 470, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

    CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Обновить", 
        WS_CHILD | WS_VISIBLE, 12,27,75,23, hWnd, (HMENU)IDC_BUTTON1, NULL, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Удалить", 
        WS_CHILD | WS_VISIBLE, 92,27,75,23, hWnd, (HMENU)IDC_BUTTON2, NULL, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Добавить", 
        WS_CHILD | WS_VISIBLE, 172,27,75,23, hWnd, (HMENU)IDC_BUTTON3, NULL, NULL);

    CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Сохранить", 
        WS_CHILD | WS_VISIBLE, 252,27,75,23, hWnd, (HMENU)IDC_BUTTON4, NULL, NULL);

   hlistBox1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"listbox", L"", 
	   WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL, 
	   12,56,572,355, hWnd, (HMENU) IDC_LISTBOX1, NULL, NULL);

	HFONT hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT); //
	SendMessage( hlistBox1,WM_SETFONT,(WPARAM)hFont,0);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

struct TItem
{
	wchar_t name[1000], val[1000], subkey[1000];
	HKEY key;
};

std::vector<TItem*> items;

void showError( DWORD err, wchar_t *msg )
{
	if (err == 0) return;

	wchar_t s[1000], buf[1000];

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM , NULL, err, 0, s, 1000, NULL );

	wsprintf( buf, L"%s\nError %d (0x%x) from %s\n", s, err, err, msg );

	MessageBox( hWnd, buf, L"Error", 0 );
}

void enumKey( HKEY regkey, wchar_t *subkey, wchar_t *pref )
{
	HKEY key;
	DWORD r;
	DWORD val_cnt;

	r = RegOpenKeyEx( regkey, subkey, 0, KEY_READ, &key );
	if (r != 0) 
	{
		showError( r, L"RegOpenKeyEx" );
		return;
	}

	r = RegQueryInfoKey(key, 
		NULL, NULL, 0, NULL, NULL, NULL, &val_cnt, NULL, NULL, NULL, NULL );
	if (r != 0) 
	{
		showError( r, L"RegQueryInfoKey" );
		return;
	}

	for( int i = 0; i < val_cnt; i++ )
	{
		TItem *item = new TItem();

		DWORD name_l, val_l;

		item->name[0] = 0;
		name_l = 1000;
		item->val[0] = 0;
		val_l = sizeof(item->val);

		item->key = regkey;
		wcscpy_s( item->subkey, subkey );
		r = RegEnumValue( key, i, &item->name[0], &name_l, 0, NULL, (LPBYTE) &item->val, &val_l );
		if (r != 0) 
		{
			showError( r, L"RegEnumValue" );
			return;
		}

		if ((item->name[0] == 0) | (item->val[0] == 0))
			continue;

		wchar_t buf[1000];

		wsprintf( buf, L"%s",  item->name, item->val );
		SendMessage( hlistBox1, LB_ADDSTRING, 0, (LPARAM) &buf[0] );

		items.push_back( item );
	}
}

void showList()
{
    // Clear listbox
	SendMessage( hlistBox1, LB_RESETCONTENT, 0,0 );
	items.clear();

	enumKey(  HKEY_LOCAL_MACHINE, 
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"HKLM" );
	enumKey(  HKEY_CURRENT_USER, 
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"HKCU" );
}

void delItem()
{
	int i = SendMessage( hlistBox1, LB_GETCURSEL, 0, 0);

	if (i < 0) return;

	TItem *item = items[i];

	DWORD r;
	HKEY key;

	r = RegOpenKeyEx( item->key, item->subkey, 0, KEY_READ | KEY_SET_VALUE, &key );
	if (r != 0) 
	{
		showError( r, L"RegOpenKeyEx" );
		return;
	}

	r = RegDeleteValue( key, item->name );
	if (r != 0) 
	{
		showError( r, L"RegDeleteValue" );
		return;
	}

	SendMessage( hlistBox1, LB_DELETESTRING, i, 0 );
	items.erase( items.begin() + i );
}

void addItem()
{
	wchar_t fname[300];
	int fi;

	LoadFileDialog( fname, 300 );

	DWORD r;
	HKEY key;

	r = RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
		0, KEY_READ | KEY_SET_VALUE, &key );
	if (r != 0) 
	{
		showError( r, L"RegOpenKeyEx" );
		return;
	}

	fi = wcslen( fname ) - 1;

	while ( (fi >= 0) & (fname[fi] != '\\') )
		fi--;
	fi++;

	r = RegSetValueEx( key, &fname[fi], 0, REG_EXPAND_SZ, (BYTE*) &fname, wcslen(fname)*2 ); 
}

void saveList()
{
	wchar_t fname[300];

	SaveFileDialog( fname, 300 );

	FILE *f = _wfopen( fname, L"wt" );

	for (int i = 0; i < items.size(); i++) {
		fwprintf(f, L"%s: %s\n", items[i]->subkey, items[i]->val);
	}

	fclose(f);
}


//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-	Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:			// Выбор комманд
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 
		switch (wmId)		// Выбор сообщения
		{
		case IDC_BUTTON1:
			showList();
			break;
		case IDC_BUTTON2:
			delItem();
			break;
		case IDC_BUTTON3:
			addItem();
			break;
		case IDC_BUTTON4:
			saveList();
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
