// coursework.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "coursework.h"
#include <locale>
#include <comdef.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <stack>
#include <iterator>

using namespace std;

#define MAX_LOADSTRING 100

int QUANTITY_VARIABLES = 3;

typedef map<string, double> Variables;

typedef map<string, vector<double>> Massives;

enum tokentype {
    var, num, op_br, cl_br, func, op, mas, op_sbr, cl_sbr
};

struct token {
    string name;
    tokentype type;

    token(string str, tokentype typ) {
        name = str;
        type = typ;
    }
    token() {}
};

typedef vector<token> tokens;
set<char> DelimSet;
const string delimiters = " ()+/*-^&|!%[]";
map <string, int> prior;

typedef double(*func_type)(stack<double>&);
typedef map<string, func_type> Ops;
Ops ops;

//ОБЪЯВЛЕНИЕ ФУНКЦИЙ ДЛЯ ВЫЧИСЛЕНИЯ ЗНАЧЕНИЯ ВЫРАЖЕНИЯ, СОДЕРЖАЩЕГО ПЕРЕМЕННЫЕ И ЗАДАННОГО В ВИДЕ СТРОКИ
void ReadExpressionFromStr(std::string inputStr[], std::string& expr, Variables& var, Massives& mas, int quantityVariables); //inputStr: 1-й элемент - выражение, 2-й, 3-й и т.д. - стрики формата: variables_name=variables_value
string MortalShell(std::string inputStr[], int quantityVariables); // inputStr аналогично функции ReadExpressionFromStr
void CreateSetOfDelimiters();
bool IsDelimiter(char sym);
void CreateTokensFromExpression(std::string& expr, tokens& texpr);
void CreatePrior();
void CreatePostfixFromTokens(tokens& texpr, tokens& pexpr);
void CreateOps();
double ResultExpr(tokens& pexpr, Variables& expvars, Massives& varmas);
double fact(double n);
double op_plus(std::stack <double>& s);
double op_minus(std::stack <double>& s);
double op_mul(std::stack <double>& s);
double op_div(std::stack <double>& s);
double op_deg(std::stack <double>& s);
double op_opposite(std::stack <double>& s);
double op_factorial(std::stack <double>& s);
double op_odiv(std::stack <double>& s);
double op_and(std::stack <double>& s);
double op_or(std::stack <double>& s);
double op_not(std::stack <double>& s);
double op_sin(std::stack <double>& s);
double op_cos(std::stack <double>& s);
double op_tan(std::stack <double>& s);
double op_asin(std::stack <double>& s);
double op_acos(std::stack <double>& s);
double op_atan(std::stack <double>& s);

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

static int window_wight = 800;
static int window_height = 600;
static int UX_cord_X = 20;
static int UX_cord_Y = 20;
static int step_X = 40;
static int step_Y = 40;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK       Ordering(HWND fields_values[], int, HWND);

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
    LoadStringW(hInstance, IDC_COURSEWORK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COURSEWORK));

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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(129));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COURSEWORK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(129));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, window_wight, window_height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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

HWND outcome = (HWND)1, edit_A0 = (HWND)1, edit_A1 = (HWND)1, edit_A2 = (HWND)1, edit_A3 = (HWND)1, edit_A4 = (HWND)1;
const int BTN_OUTCOME = 1049;
const int EDIT_OUTCOME = 1050;
const int EDIT_A0 = 1051;
const int EDIT_A1 = 1052;
const int EDIT_A2 = 1053;
const int EDIT_A3 = 1054;
const int EDIT_A4 = 1055;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR buff[1024];
    switch (message)
    {
    case WM_CREATE:
        {
            HWND mainEquation = CreateWindow(L"STATIC", L"A0 * Y'' + A1 * Y' + A2 * Y + A3 * X + A4 = 0", WS_CHILD | WS_VISIBLE, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 0, window_wight - UX_cord_X * 3, 36, hWnd, 0, hInst, 0);
            HFONT hf = CreateFont(36, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Sans Serif");
            SendMessage(mainEquation, WM_SETFONT, (WPARAM)hf, 0);
            
            HWND A0 = CreateWindow(L"STATIC", L"A0:", WS_CHILD | WS_VISIBLE | WS_BORDER, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 1, 55, 36, hWnd, 0, hInst, 0);
            SendMessage(A0, WM_SETFONT, (WPARAM)hf, 0);
            
            edit_A0 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, UX_cord_X + step_X * 0 + 55, UX_cord_Y + step_Y * 1, window_wight - UX_cord_X * 3 - 55, 36, hWnd, (HMENU)EDIT_A0, hInst, 0);
            SendMessage(edit_A0, WM_SETFONT, (WPARAM)hf, 0);
            
            HWND A1 = CreateWindow(L"STATIC", L"A1:", WS_CHILD | WS_VISIBLE | WS_BORDER, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 2, 55, 36, hWnd, 0, hInst, 0);
            SendMessage(A1, WM_SETFONT, (WPARAM)hf, 0);
            
            edit_A1 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, UX_cord_X + step_X * 0 + 55, UX_cord_Y + step_Y * 2, window_wight - UX_cord_X * 3 - 55, 36, hWnd, (HMENU)EDIT_A1, hInst, 0);
            SendMessage(edit_A1, WM_SETFONT, (WPARAM)hf, 0);

            HWND A2 = CreateWindow(L"STATIC", L"A2:", WS_CHILD | WS_VISIBLE | WS_BORDER, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 3, 55, 36, hWnd, 0, hInst, 0);
            SendMessage(A2, WM_SETFONT, (WPARAM)hf, 0);

            edit_A2 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, UX_cord_X + step_X * 0 + 55, UX_cord_Y + step_Y * 3, window_wight - UX_cord_X * 3 - 55, 36, hWnd, (HMENU)EDIT_A2, hInst, 0);
            SendMessage(edit_A2, WM_SETFONT, (WPARAM)hf, 0);
            
            HWND A3 = CreateWindow(L"STATIC", L"A3:", WS_CHILD | WS_VISIBLE | WS_BORDER, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 4, 55, 36, hWnd, 0, hInst, 0);
            SendMessage(A3, WM_SETFONT, (WPARAM)hf, 0);
            
            edit_A3 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, UX_cord_X + step_X * 0 + 55, UX_cord_Y + step_Y * 4, window_wight - UX_cord_X * 3 - 55, 36, hWnd, (HMENU)EDIT_A3, hInst, 0);
            SendMessage(edit_A3, WM_SETFONT, (WPARAM)hf, 0);

            HWND A4 = CreateWindow(L"STATIC", L"A4:", WS_CHILD | WS_VISIBLE | WS_BORDER, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 5, 55, 36, hWnd, 0, hInst, 0);
            SendMessage(A4, WM_SETFONT, (WPARAM)hf, 0);

            edit_A4 = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, UX_cord_X + step_X * 0 + 55, UX_cord_Y + step_Y * 5, window_wight - UX_cord_X * 3 - 55, 36, hWnd, (HMENU)EDIT_A4, hInst, 0);
            SendMessage(edit_A4, WM_SETFONT, (WPARAM)hf, 0);
            
            outcome = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, UX_cord_X + step_X * 0, UX_cord_Y + step_Y * 8 + 25, window_wight - UX_cord_X * 3, 150, hWnd, (HMENU)EDIT_OUTCOME, hInst, 0);
            SendMessage(outcome, WM_SETFONT, (WPARAM)hf, 0);
            
            HWND btnOutcome = CreateWindow(L"button", L"Найти решение", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, UX_cord_X + step_X * 4, UX_cord_Y + step_Y * 6 + 50, 300, 36, hWnd, (HMENU)BTN_OUTCOME, hInst, NULL);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case BTN_OUTCOME:
                GetWindowText((HWND)outcome, buff, 1024);
                SetWindowText((HWND)outcome, L"Hello!\r\nFill the fields!\r\n1\r\n2\r\n3");
                if (!Ordering(new HWND[] {edit_A0, edit_A1, edit_A2, edit_A3, edit_A4 }, 5, outcome))
                {
                    SetWindowText((HWND)outcome, L"Проверьте поля для ввода!\r\nЕсть как минимум одна ошибка.");
                }
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
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CTLCOLOREDIT:
        //SetBkMode((HDC)wParam, TRANSPARENT);
        SetBkColor((HDC)wParam, RGB(201, 201, 201));
        return (INT_PTR)GetStockObject(NULL_BRUSH);
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

BOOL CALLBACK Ordering(HWND fields_values[], int quantity_fields, HWND field_ans)
{
    TCHAR buff[1024];
    wstring inputData[] = {L"", L"", L"", L"", L""};
    for (int i = 0; i < quantity_fields; i++) {
        GetWindowText(fields_values[i], buff, 1024);
        inputData[i] = buff;
    }
    try {

        delete[] inputData;
        return true;
    }
    catch (exception e) {
        delete[] inputData;
        return false;
    }
}

void ReadExpressionFromStr(string inputStr[], string& expr, Variables& var, Massives& mas, int quantityVariables) {
    expr = inputStr[0].substr(0);
    string temp;
    int pos;
    for (int i = 1; i < quantityVariables; i++) {
        temp = inputStr[i].substr(0);
        pos = temp.find('=');
        if (pos > 0) {
            string name = temp.substr(0, pos);
            double value = atof(temp.substr(pos + 1).c_str());
            var[name] = value;
        }
        pos = temp.find('{');
        if (pos > 0) {
            string name = temp.substr(0, pos);
            int pos1 = pos, pos2;
            do {
                pos2 = temp.find(',');
                double value = atof(temp.substr(pos1 + 1, pos2).c_str());
                mas[name].push_back(value);
                if (pos2 == -1)
                    break;
                temp[pos2] = ' ';
                pos1 = pos2;
            } while (pos2 > 0);
        }
    }
}

string MortalShell(string inputStr[], int quantityVariables) {
    tokens texpr, pexpr;
    Variables expvars;
    Massives expmasvars;
    string expr;

    ReadExpressionFromStr(inputStr, expr, expvars, expmasvars, quantityVariables);

    CreateTokensFromExpression(expr, texpr);

    CreatePostfixFromTokens(texpr, pexpr);

    return std::to_string(ResultExpr(pexpr, expvars, expmasvars));
}

void CreateSetOfDelimiters() {
    for (int i = 0; i < delimiters.size(); i++)
        DelimSet.insert(delimiters[i]);
    return;
}

bool IsDelimiter(char sym) {
    return DelimSet.count(sym) > 0;
}

void CreateTokensFromExpression(string& expr, tokens& texpr) {
    CreateSetOfDelimiters();
    string ex = expr + " ";
    string name;

    int i = 0;
    while (i < ex.size() - 1) {
        name = "";
        if (IsDelimiter(ex[i])) {
            if (ex[i] == ' ') {
                i++;
                continue;
            }
            name = ex[i];
            i++;
        }

        else {
            while (!IsDelimiter(ex[i])) {
                name += ex[i];
                i++;
            }
        }
        texpr.push_back(token(name, var));
    }

    for (int j = 0; j < texpr.size(); j++) {
        if (texpr[j].name[0] == '[') {
            texpr[j].type = op_sbr;
            continue;
        }
        if (texpr[j].name[0] == ']') {
            texpr[j].type = cl_sbr;
            continue;
        }
        if (texpr[j].name[0] == '(') {
            texpr[j].type = op_br;
            continue;
        }
        if (texpr[j].name[0] == ')') {
            texpr[j].type = cl_br;
            continue;
        }
        if (isdigit(texpr[j].name[0])) {
            texpr[j].type = num;
            continue;
        }

        if (isalpha(texpr[j].name[0])) {
            if (j < texpr.size() - 1 && texpr[j + 1].name[0] == '[')
                texpr[j].type = mas;
        }

        if (isalpha(texpr[j].name[0])) {
            if (j < texpr.size() - 1 && texpr[j + 1].name[0] == '(')
                texpr[j].type = func;
            continue;
        }

        texpr[j].type = op;
    }

    for (int j = 0; j < texpr.size(); j++) {
        if (texpr[j].name == "-" && (j == 0 || texpr[j - 1].type == op_br))
            texpr[j].name = "opposite";
        if (texpr[j].name == "!" && (j == texpr.size() - 1 || texpr[j + 1].type == cl_br || texpr[j + 1].type == op))
            texpr[j].name = "factorial";
    }

    return;
}

void CreatePrior() {
    prior["+"] = 10;
    prior["-"] = 10;
    prior["*"] = 20;
    prior["/"] = 20;
    prior["^"] = 30;
    prior["opposite"] = 10;
    prior["factorial"] = 30;
    prior["%"] = 20;
    prior["&"] = 5;
    prior["|"] = 5;
    prior["!"] = 40;
}

void CreatePostfixFromTokens(tokens& texpr, tokens& pexpr) {
    //Задаем приоритеты операций
    CreatePrior();
    stack <token> TStack;

    //Ловим токены и работаем по алгоритму
    for (int i = 0; i < texpr.size(); i++) {
        switch (texpr[i].type) {
        case var:
        case num:
            pexpr.push_back(texpr[i]);
            break;

        case op_br:
            TStack.push(texpr[i]);
            break;

        case cl_br:
            while (TStack.top().type != op_br) {
                pexpr.push_back(TStack.top());
                TStack.pop();
            }
            TStack.pop();
            break;

        case op_sbr:
            TStack.push(texpr[i]);
            break;

        case cl_sbr:
            while (TStack.top().type != op_sbr) {
                pexpr.push_back(TStack.top());
                TStack.pop();
            }
            TStack.pop();
            break;

        case op:
            if (TStack.size()) {
                while (TStack.size() && ((TStack.top().type == op && prior[texpr[i].name] <= prior[TStack.top().name]) ||
                    TStack.top().type == func ||
                    TStack.top().type == mas)) {
                    pexpr.push_back(TStack.top());
                    TStack.pop();
                }
            }
            TStack.push(texpr[i]);
            break;

        case mas:
            while (TStack.size() && TStack.top().type == mas) {
                pexpr.push_back(TStack.top());
                TStack.pop();
            }
            TStack.push(texpr[i]);
            break;

        case func:
            while (TStack.size() && TStack.top().type == func) {
                pexpr.push_back(TStack.top());
                TStack.pop();
            }
            TStack.push(texpr[i]);
            break;
        }
    }

    while (TStack.size()) {
        pexpr.push_back(TStack.top());
        TStack.pop();
    }

    return;
}

void CreateOps() {
    ops["+"] = op_plus;
    ops["-"] = op_minus;
    ops["*"] = op_mul;
    ops["/"] = op_div;
    ops["^"] = op_deg;
    ops["opposite"] = op_opposite;
    ops["factorial"] = op_factorial;
    ops["%"] = op_odiv;
    ops["&"] = op_and;
    ops["|"] = op_or;
    ops["!"] = op_not;
    ops["sin"] = op_sin;
    ops["cos"] = op_cos;
    ops["tan"] = op_tan;
    ops["acos"] = op_acos;
    ops["asin"] = op_asin;
    ops["atan"] = op_atan;

    return;
}

double ResultExpr(tokens& pexpr, Variables& expvars, Massives& varmas) {
    CreateOps();
    stack <double> s;

    for (int i = 0; i < pexpr.size(); i++) {
        switch (pexpr[i].type) {
        case num: {
            s.push(atoi(pexpr[i].name.c_str()));
        }
                break;

        case var: {
            Variables::iterator Vit;
            for (Vit = expvars.begin(); Vit != expvars.end(); Vit++) {
                if (Vit->first == pexpr[i].name) {
                    s.push(Vit->second);
                    break;
                }
            }
        }
                break;

        case func:
        case op: {
            Ops::iterator Oit;
            for (Oit = ops.begin(); Oit != ops.end(); Oit++) {
                if (Oit->first == pexpr[i].name) {
                    s.push(Oit->second(s));
                    break;
                }
            }
        }
               break;

        case mas: {
            int index = s.top();
            s.pop();

            Massives::iterator it;
            for (it = varmas.begin(); it != varmas.end(); it++) {
                if (it->first == pexpr[i].name)
                    s.push(it->second[index]);
            }
        }
        }
    }

    return s.top();
}

double fact(double n) {
    if (n == 0)
        return 1;
    return n * fact(n - 1);
}

double op_plus(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    __asm {
        fld a;
        fld b;
        fadd;
        fstp a;
    }
    return a;
}

double op_minus(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    __asm {
        fld b;
        fld a;
        fsub;
        fstp a;
    }
    return a;
}

double op_mul(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    __asm {
        fld a;
        fld b;
        fmul;
        fstp a;
    }
    return a;
}

double op_div(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    __asm {
        fld b;
        fld a;
        fdiv;
        fstp a;
    }
    return a;
}

double op_deg(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    //b^a!!
    return pow(b, a);
}

double op_opposite(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return -a;
}

double op_factorial(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return fact(a);
}

double op_odiv(stack <double>& s) {
    long long a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    return b % a;
}

double op_and(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    return a && b;
}

double op_or(stack <double>& s) {
    double a, b;
    a = s.top();
    s.pop();
    b = s.top();
    s.pop();
    return a || b;
}

double op_not(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return !a;
}

double op_sin(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    __asm {
        fld a;
        fsin;
        fstp a;
    }
    return a;
}

double op_cos(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    __asm {
        fld a;
        fcos;
        fstp a;
    }
    return a;
}

double op_tan(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    __asm {
        fld a;
        fptan;
        fxch st(1);
        fstp a;
    }
    return a;
}

double op_asin(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return asin(a);
}

double op_acos(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return acos(a);
}

double op_atan(stack <double>& s) {
    double a;
    a = s.top();
    s.pop();
    return atan(a);
}


