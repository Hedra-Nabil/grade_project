#include <Windows.h>
#include <commctrl.h>   
#include <wchar.h>
#include <cstdio> 

// هذا السطر هو الأهم لتشغيل الألوان (Red/Yellow/Green)
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib") 

// تعريف المعرفات (IDs)
#define ID_EDIT_GRADE   1
#define ID_PROGRESS     2
#define ID_LBL_STATUS   3
#define ID_LBL_GPA      4
#define ID_BTN_REPORT   5  // أعدنا تعريف زر الريبورت

// دالة حساب المعدل التراكمي
float CalculateGPA(int score) {
    if (score >= 90) return 4.0f;
    if (score >= 85) return 3.7f;
    if (score >= 80) return 3.3f;
    if (score >= 75) return 3.0f;
    if (score >= 70) return 2.7f;
    if (score >= 65) return 2.3f;
    if (score >= 60) return 2.0f;
    if (score >= 50) return 1.0f;
    return 0.0f;
}

// دالة حساب التقدير النصي
const wchar_t* GetLetterGrade(int score) {
    if (score >= 85) return L"Excellent (A)";
    if (score >= 75) return L"Very Good (B)";
    if (score >= 65) return L"Good (C)";
    if (score >= 50) return L"Pass (D)";
    return L"Fail (F)";
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit, hProgress, hLabelStatus, hLabelGPA;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        // 1. النصوص والعناوين
        CreateWindow(L"STATIC", L"Enter Grade:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);

        // 2. مربع الإدخال
        hEdit = CreateWindow(
            L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_CENTER,
            120, 18, 60, 25,
            hwnd, (HMENU)ID_EDIT_GRADE, NULL, NULL
        );

        // 3. شريط التقدم (Progress Bar)
        hProgress = CreateWindow(
            PROGRESS_CLASS, NULL,
            WS_VISIBLE | WS_CHILD | PBS_SMOOTH,
            20, 60, 340, 30,
            hwnd, (HMENU)ID_PROGRESS, NULL, NULL
        );
        SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

        // 4. نصوص عرض النتائج
        hLabelStatus = CreateWindow(L"STATIC", L"Status: Waiting", WS_VISIBLE | WS_CHILD, 20, 105, 300, 20, hwnd, (HMENU)ID_LBL_STATUS, NULL, NULL);
        hLabelGPA = CreateWindow(L"STATIC", L"GPA: 0.0", WS_VISIBLE | WS_CHILD, 20, 130, 300, 20, hwnd, (HMENU)ID_LBL_GPA, NULL, NULL);

        // 5. زر الريبورت (تمت إعادته)
        CreateWindow(
            L"BUTTON", L"Get Full Report",
            WS_VISIBLE | WS_CHILD,
            100, 160, 150, 30,
            hwnd, (HMENU)ID_BTN_REPORT, NULL, NULL
        );

        return 0;
    }

    case WM_COMMAND:
    {
        // أولاً: التعامل مع التغيير في مربع النص (Event Driven)
        if (LOWORD(wParam) == ID_EDIT_GRADE && HIWORD(wParam) == EN_CHANGE)
        {
            wchar_t buffer[10];
            GetWindowText(hEdit, buffer, 10);

            int grade = (wcslen(buffer) > 0) ? _wtoi(buffer) : 0;
            if (grade > 100) grade = 100;

            // تحديث الشريط
            SendMessage(hProgress, PBM_SETPOS, grade, 0);

            // منطق تغيير الألوان (يعتمد على السطر السحري في الأعلى)
            if (grade < 50)
            {
                // راسب -> لون أحمر
                SendMessage(hProgress, PBM_SETSTATE, PBST_ERROR, 0);
            }
            else if (grade < 75)
            {
                // متوسط -> لون أصفر
                SendMessage(hProgress, PBM_SETSTATE, PBST_PAUSED, 0);
            }
            else
            {
                // ممتاز -> لون أخضر
                SendMessage(hProgress, PBM_SETSTATE, PBST_NORMAL, 0);
            }

            // تحديث النصوص
            wchar_t statusText[100];
            wchar_t gpaText[100];
            swprintf_s(statusText, L"Status: %s", GetLetterGrade(grade));
            swprintf_s(gpaText, L"GPA Score: %.1f", CalculateGPA(grade));
            SetWindowText(hLabelStatus, statusText);
            SetWindowText(hLabelGPA, gpaText);
        }

        // ثانياً: التعامل مع زر الريبورت
        else if (LOWORD(wParam) == ID_BTN_REPORT)
        {
            wchar_t buffer[10];
            GetWindowText(hEdit, buffer, 10);
            int grade = (wcslen(buffer) > 0) ? _wtoi(buffer) : 0;

            wchar_t reportMsg[500];
            swprintf_s(reportMsg,
                L"--- STUDENT OFFICIAL REPORT ---\n\n"
                L"Grade: %d / 100\n"
                L"Rating: %s\n"
                L"GPA: %.1f\n"
                L"Result: %s\n\n"
                L"Keep up the hard work!",
                grade,
                GetLetterGrade(grade),
                CalculateGPA(grade),
                (grade >= 50) ? L"PASSED" : L"FAILED"
            );

            MessageBox(hwnd, reportMsg, L"Final Report", MB_OK | MB_ICONINFORMATION);
        }

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    wchar_t CLASS_NAME[] = L"CompleteGraderApp";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // إضافة مؤشر الماوس العادي

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Student Grading System",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 250,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}