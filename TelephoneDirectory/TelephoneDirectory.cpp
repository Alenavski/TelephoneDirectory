// TelephoneDirectory.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "TelephoneDirectory.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <string>
#include <sstream>

#define MAX_LOADSTRING 100
#define IDM_CODE_SAMPLES 1111

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
const std::vector<std::wstring> lvColumns{ L"Фамилия", L"Имя", L"Отчество", L"Улица", L"Дом", L"Корпус", L"Квартира", L"Телефон"};
std::vector<std::shared_ptr<TelephoneRecord> >* records;
HWND hwndListView;
HWND hChildWnd1;
HWND hEdt1 = NULL;
HWND hEdt2 = NULL;
HWND hEdt3 = NULL;
HWND hBtn = NULL;
WCHAR Surname[MAX_LOADSTRING];
WCHAR Street[MAX_LOADSTRING];
WCHAR Telephone[MAX_LOADSTRING];

// Отправить объявления функций, включенных в этот модуль кода:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcFormSearch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateListView(HWND hwndParent);
VOID FillListView(HWND hWndListView, std::vector<std::shared_ptr<TelephoneRecord> >* records);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TELEPHONEDIRECTORY, szWindowClass, MAX_LOADSTRING);
    
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    hInst = hInstance;

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TELEPHONEDIRECTORY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    if (!RegisterClassExW(&wcex))
        return -1;

    HMENU menu = CreateMenu();
    HMENU menuItem = CreatePopupMenu();

    AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)menuItem, L"Файл");
    AppendMenu(menuItem, MF_STRING, 0, L"Загрузить справочник");
    AppendMenu(menuItem, MF_STRING, 1, L"Найти запись");

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, menu, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    SetMenu(hWnd, menu);
    hwndListView = CreateListView(hWnd);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    WNDCLASSEX wcChild1 = wcex;

    wcChild1.lpfnWndProc = (WNDPROC)WndProcFormSearch;
    wcChild1.lpszClassName = L"Search";
    wcChild1.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcChild1.hIcon = LoadIcon(NULL, IDI_ASTERISK);
    wcChild1.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcChild1.lpszMenuName = NULL;

    if (!RegisterClassExW(&wcChild1))
        return -1;

    hChildWnd1 = CreateWindowEx(NULL, L"Search", L"", WS_OVERLAPPEDWINDOW, 0, 0, 400, 300, hWnd, NULL, hInst, NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TELEPHONEDIRECTORY));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

HWND CreateListView(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect(hwndParent, &rcClient);


    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindowEx(NULL, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hwndParent, (HMENU)IDM_CODE_SAMPLES, hInst, NULL);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cx = 140;
    for (int i = 0; i < 8; i++) {
        lvc.pszText = (LPWSTR)lvColumns[i].c_str();
        ListView_InsertColumn(hWndListView, i, &lvc);
    }

    return (hWndListView);
}

VOID FillListView(HWND hWndListView, std::vector<std::shared_ptr<TelephoneRecord>> *records)
{
    ListView_DeleteAllItems(hWndListView);
    LVITEM lvI;

    lvI.pszText = LPSTR_TEXTCALLBACK;
    lvI.mask = LVIF_TEXT;
    lvI.iSubItem = 0;
    for (int i = 0; i < records->size(); i++) {
        lvI.iItem = i;

        ListView_InsertItem(hWndListView, &lvI);
        ListView_SetItemText(hWndListView, i, 0, (LPWSTR)(*records)[i]->Surname.c_str());
        ListView_SetItemText(hWndListView, i, 1, (LPWSTR)(*records)[i]->Name.c_str());
        ListView_SetItemText(hWndListView, i, 2, (LPWSTR)(*records)[i]->Patronymic.c_str());
        ListView_SetItemText(hWndListView, i, 3, (LPWSTR)(*records)[i]->Street.c_str());
        ListView_SetItemText(hWndListView, i, 4, (LPWSTR)(*records)[i]->House.c_str());
        ListView_SetItemText(hWndListView, i, 5, (LPWSTR)(*records)[i]->Building.c_str());
        ListView_SetItemText(hWndListView, i, 6, (LPWSTR)(*records)[i]->Flat.c_str());
        ListView_SetItemText(hWndListView, i, 7, (LPWSTR)(*records)[i]->Telephone.c_str());
    }
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HMODULE dll = NULL;
    //std::wstring FileName = L"..\\Test.txt";
    std::wstring FileName = L"..\\ReferenceBook.txt";
    
    if (message == WM_CREATE) {
        dll = LoadLibrary(L"./DirectoryWorker.dll");
        if (dll == NULL) {
            MessageBox(hWnd, L"Something is wrong", 0, 0);
        }
        LoadTelephoneBook = (LoadBook*)GetProcAddress(dll, "LoadTelephoneBook");
        SearchTelephoneRecord = (SearchRecord*)GetProcAddress(dll, "SearchTelephoneRecord");
        GetAllRecords = (getRecords*)GetProcAddress(dll, "getAllRecords");
    }

    
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case 0:
                if (LoadTelephoneBook(&FileName)) {
                    MessageBox(hWnd, L"Success", 0, 0);
                }
                records = (std::vector<std::shared_ptr<TelephoneRecord> >*)GetAllRecords();
                FillListView(hwndListView, records);
                break;
            case 1:
                ShowWindow(hChildWnd1, SW_NORMAL);
                UpdateWindow(hChildWnd1);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        FreeLibrary(dll);

        PostQuitMessage(0);        
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProcFormSearch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    
    if (message == WM_CREATE) {
        hEdt1 = CreateWindowEx(NULL, L"edit", L"Фамилия", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 50, 250, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdt1, SW_SHOWNORMAL);

        hEdt2 = CreateWindowEx(NULL, L"edit", L"Улица", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 100, 250, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdt2, SW_SHOWNORMAL);

        hEdt3 = CreateWindowEx(NULL, L"edit", L"Телефон", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 150, 250, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdt3, SW_SHOWNORMAL);

        hBtn = CreateWindowEx(NULL, L"button", L"Найти", WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 200, 120, 30, hWnd, (HMENU)10000, hInst, NULL);
        ShowWindow(hBtn, SW_SHOWNORMAL);
    }
    // Обработчик сообщений
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case 10000:
            int Len = GetWindowText(hEdt1, Surname, MAX_LOADSTRING);            
            Len = GetWindowText(hEdt2, Street, MAX_LOADSTRING);
            Len = GetWindowText(hEdt3, Telephone, MAX_LOADSTRING);
            std::wstring* keySurname = new std::wstring(Surname);
            std::wstring* keyStreet = new std::wstring(Street);
            std::wstring* keyTelephone = new std::wstring(Telephone);
            records = (std::vector<std::shared_ptr<TelephoneRecord> >*)SearchTelephoneRecord(keySurname, keyStreet, keyTelephone);
            FillListView(hwndListView, records);
            ShowWindow(hWnd, SW_HIDE);
        }
    }
        break;
    case WM_DESTROY: 
        ShowWindow(hWnd, SW_HIDE);
        break; 
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}