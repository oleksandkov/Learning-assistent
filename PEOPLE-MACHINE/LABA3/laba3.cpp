// ScheduleApp -- Win32 C++ (simplified)
// Compile: g++ laba3.cpp -o laba3.exe -std=c++17 -lcomctl32 -mwindows
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <algorithm>
using std::vector;
using std::wstring;

struct Lesson
{
    wstring Subject, Time, Room, Group, Week, Teacher, Type;
};
vector<Lesson> g_lessons;
static HWND g_main, g_lv;
static HFONT g_font;

#define ID_ADD 101
#define ID_SEARCH 102
#define ID_CLEAR 103
#define ID_LV 104

// ---- Simple one-field InputBox ----
static const wchar_t *g_prompt;
static wchar_t g_ibuf[256];
static HWND g_iHwnd;

static LRESULT CALLBACK IBoxProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CREATE)
    {
        HINSTANCE hi = (HINSTANCE)GetWindowLongPtr(hw, GWLP_HINSTANCE);
        CreateWindowEx(0, L"STATIC", g_prompt, WS_CHILD | WS_VISIBLE,
                       8, 8, 284, 18, hw, 0, hi, 0);
        HWND he = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", g_ibuf,
                                 WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
                                 8, 30, 284, 22, hw, (HMENU)1, hi, 0);
        SetFocus(he);
        CreateWindowEx(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                       60, 62, 80, 24, hw, (HMENU)IDOK, hi, 0);
        CreateWindowEx(0, L"BUTTON", L"Скасувати", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                       155, 62, 110, 24, hw, (HMENU)IDCANCEL, hi, 0);
    }
    if (msg == WM_COMMAND)
    {
        if (LOWORD(wp) == IDOK)
        {
            GetDlgItemText(hw, 1, g_ibuf, 256);
            DestroyWindow(hw);
        }
        else if (LOWORD(wp) == IDCANCEL)
        {
            g_ibuf[0] = 0;
            DestroyWindow(hw);
        }
    }
    if (msg == WM_DESTROY)
        g_iHwnd = nullptr;
    return DefWindowProc(hw, msg, wp, lp);
}

static bool InputBox(HWND parent, const wchar_t *title, const wchar_t *prompt, wstring &out)
{
    static bool reg = false;
    if (!reg)
    {
        WNDCLASS wc{};
        wc.lpfnWndProc = IBoxProc;
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.hInstance = (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);
        wc.lpszClassName = L"IBox";
        RegisterClass(&wc);
        reg = true;
    }
    g_prompt = prompt;
    g_ibuf[0] = 0;
    g_iHwnd = CreateWindowEx(WS_EX_DLGMODALFRAME, L"IBox", title,
                             WS_POPUP | WS_CAPTION | WS_SYSMENU, 0, 0, 310, 110,
                             parent, 0, (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), 0);
    RECT rp, rc;
    GetWindowRect(parent, &rp);
    GetWindowRect(g_iHwnd, &rc);
    SetWindowPos(g_iHwnd, 0,
                 (rp.left + rp.right) / 2 - (rc.right - rc.left) / 2,
                 (rp.top + rp.bottom) / 2 - (rc.bottom - rc.top) / 2,
                 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    EnableWindow(parent, FALSE);
    ShowWindow(g_iHwnd, SW_SHOW);
    MSG msg;
    while (g_iHwnd && GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    EnableWindow(parent, TRUE);
    SetForegroundWindow(parent);
    if (g_ibuf[0])
    {
        out = g_ibuf;
        return true;
    }
    return false;
}

// ---- ListView helpers ----
static void LVAddCol(int i, const wchar_t *t, int w)
{
    LVCOLUMN c{};
    c.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    c.iSubItem = i;
    c.pszText = const_cast<wchar_t *>(t);
    c.cx = w;
    ListView_InsertColumn(g_lv, i, &c);
}
static void LVSet(int row, int col, const wstring &txt)
{
    LVITEM li{};
    li.mask = LVIF_TEXT;
    li.iItem = row;
    li.iSubItem = col;
    li.pszText = const_cast<wchar_t *>(txt.c_str());
    if (col == 0)
        ListView_InsertItem(g_lv, &li);
    else
        ListView_SetItem(g_lv, &li);
}
static void RefreshLV(const vector<Lesson> &src)
{
    ListView_DeleteAllItems(g_lv);
    for (int i = 0; i < (int)src.size(); i++)
    {
        LVSet(i, 0, src[i].Subject);
        LVSet(i, 1, src[i].Time);
        LVSet(i, 2, src[i].Room);
        LVSet(i, 3, src[i].Group);
        LVSet(i, 4, src[i].Week);
        LVSet(i, 5, src[i].Teacher);
        LVSet(i, 6, src[i].Type);
    }
}

// ---- Case-insensitive substring ----
static bool ContainsIC(const wstring &s, const wstring &sub)
{
    if (sub.empty())
        return true;
    wstring a = s, b = sub;
    std::transform(a.begin(), a.end(), a.begin(), towlower);
    std::transform(b.begin(), b.end(), b.begin(), towlower);
    return a.find(b) != wstring::npos;
}

// ---- Actions ----
static void AddLesson(HWND hw)
{
    Lesson l;
    if (!InputBox(hw, L"Предмет", L"Предмет:", l.Subject))
        return;
    if (!InputBox(hw, L"Час", L"Час (напр. Пн 08:30):", l.Time))
        return;
    if (!InputBox(hw, L"Аудиторія", L"Аудиторія:", l.Room))
        return;
    if (!InputBox(hw, L"Група", L"Група:", l.Group))
        return;
    if (!InputBox(hw, L"Тиждень", L"Тиждень (1 / 2 / Обидва):", l.Week))
        return;
    if (!InputBox(hw, L"Викладач", L"Прізвище викладача:", l.Teacher))
        return;
    if (!InputBox(hw, L"Тип", L"Тип (Лекція/Практика/Лабораторна):", l.Type))
        return;
    g_lessons.push_back(l);
    RefreshLV(g_lessons);
}

static void SearchLessons(HWND hw)
{
    wstring sub, teacher, group;
    InputBox(hw, L"Пошук — предмет", L"Предмет (порожньо = всі):", sub);
    InputBox(hw, L"Пошук — викладач", L"Викладач:", teacher);
    InputBox(hw, L"Пошук — група", L"Група:", group);
    vector<Lesson> res;
    for (auto &l : g_lessons)
        if (ContainsIC(l.Subject, sub) && ContainsIC(l.Teacher, teacher) && ContainsIC(l.Group, group))
            res.push_back(l);
    RefreshLV(res);
    wchar_t buf[64];
    swprintf(buf, 64, L"Розклад — знайдено: %d", (int)res.size());
    SetWindowText(g_main, buf);
}

// ---- Layout ----
static void Layout(HWND hw)
{
    RECT r;
    GetClientRect(hw, &r);
    MoveWindow(GetDlgItem(hw, ID_ADD), 8, 8, 150, 28, TRUE);
    MoveWindow(GetDlgItem(hw, ID_SEARCH), 166, 8, 110, 28, TRUE);
    MoveWindow(GetDlgItem(hw, ID_CLEAR), 284, 8, 130, 28, TRUE);
    MoveWindow(g_lv, 0, 44, r.right, r.bottom - 44, TRUE);
}

// ---- Main WndProc ----
static LRESULT CALLBACK WndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hi = ((LPCREATESTRUCT)lp)->hInstance;
        g_font = CreateFont(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        auto mk = [&](const wchar_t *t, int id)
        {
            HWND h = CreateWindowEx(0, L"BUTTON", t, BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
                                    0, 0, 0, 0, hw, (HMENU)(INT_PTR)id, hi, 0);
            SendMessage(h, WM_SETFONT, (WPARAM)g_font, TRUE);
        };
        mk(L"Додати заняття", ID_ADD);
        mk(L"Пошук", ID_SEARCH);
        mk(L"Скинути фільтр", ID_CLEAR);

        g_lv = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"",
                              LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_CHILD | WS_VISIBLE,
                              0, 0, 0, 0, hw, (HMENU)(INT_PTR)ID_LV, hi, 0);
        ListView_SetExtendedListViewStyle(g_lv, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        SendMessage(g_lv, WM_SETFONT, (WPARAM)g_font, TRUE);
        LVAddCol(0, L"Предмет", 180);
        LVAddCol(1, L"Час", 100);
        LVAddCol(2, L"Аудиторія", 90);
        LVAddCol(3, L"Група", 90);
        LVAddCol(4, L"Тиждень", 70);
        LVAddCol(5, L"Викладач", 160);
        LVAddCol(6, L"Тип", 100);

        g_lessons = {
            {L"Математика", L"Пн 08:30", L"201", L"КН-11", L"1", L"Іваненко І.І.", L"Лекція"},
            {L"Програмування", L"Вт 10:15", L"302", L"КН-11", L"Обидва", L"Петренко О.В.", L"Практика"},
            {L"Фізика", L"Ср 13:45", L"105", L"КН-12", L"2", L"Коваль М.С.", L"Лабораторна"},
            {L"Алгоритми", L"Чт 08:30", L"210", L"ІПЗ-21", L"1", L"Іваненко І.І.", L"Лекція"},
        };
        RefreshLV(g_lessons);
        break;
    }
    case WM_SIZE:
        Layout(hw);
        break;
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
        case ID_ADD:
            AddLesson(hw);
            break;
        case ID_SEARCH:
            SearchLessons(hw);
            break;
        case ID_CLEAR:
            RefreshLV(g_lessons);
            SetWindowText(g_main, L"Розклад занять");
            break;
        }
        break;
    case WM_DESTROY:
        DeleteObject(g_font);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hw, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int show)
{
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&icc);
    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.lpszClassName = L"ScheduleApp";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    RegisterClass(&wc);
    g_main = CreateWindowEx(0, L"ScheduleApp", L"Розклад занять",
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 520,
                            0, 0, hi, 0);
    ShowWindow(g_main, show);
    UpdateWindow(g_main);
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}