#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

struct Task {
    std::string description;
    std::string date;
    std::string priority;

    Task(const std::string& desc, const std::string& dt, const std::string& prio) : description(desc), date(dt), priority(prio) {}
};

// Global variables
std::vector<Task> tasks;
HWND hwndList;
HWND hwndEdit;
HWND hwndDateEdit;
HWND hwndPriorityEdit;
HWND hwndAddButton;
HWND hwndDescriptionLabel;
HWND hwndDateLabel;
HWND hwndPriorityLabel;
HWND hwndMadeByLabel; // Added for "Made by-Eshan Singh"
CHAR szClassName[] = "ToDoApp";

bool IsValidDate(const std::string& date) {
    std::istringstream ss(date);
    int day, month, year;
    if (ss >> day >> month >> year) {
        if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 1000 && year <= 9999) {
            return true;
        }
    }
    return false;
}

bool CompareTasks(const Task& a, const Task& b) {
    if (a.priority == b.priority) {
        return a.date < b.date;
    }
    return a.priority < b.priority;
}

// Function to add a task to the list
void AddTask(const std::string& task, const std::string& date, const std::string& priority) {
    if (!IsValidDate(date)) {
        MessageBoxA(NULL, "Invalid date format (dd mm yyyy)", "Error", MB_ICONERROR | MB_OK);
        return;
    }

    tasks.emplace_back(task, date, priority);

    // Sort tasks based on priority and date
    std::sort(tasks.begin(), tasks.end(), CompareTasks);

    SendMessageA(hwndList, LB_RESETCONTENT, 0, 0); // Clear the list box

    for (const Task& t : tasks) {
        std::string item = t.priority + " - " + t.date + " - " + t.description;
        SendMessageA(hwndList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
    }

    SetWindowTextA(hwndEdit, "");
    SetWindowTextA(hwndDateEdit, "");
    SetWindowTextA(hwndPriorityEdit, "");
}

// Resize and reposition controls based on window size
void ResizeControls(HWND hwnd, int width, int height) {
    int labelWidth = 120;
    int editWidth = 200;
    int buttonWidth = 100;
    int labelX = 10;
    int editX = labelX + labelWidth + 10;
    int buttonX = 10;
    int ySpacing = 30;
    int yStart = 10;

    // Update control positions and sizes based on the window size
    SetWindowPos(hwndDescriptionLabel, NULL, labelX, yStart, labelWidth, 20, SWP_NOZORDER);
    SetWindowPos(hwndEdit, NULL, editX, yStart, editWidth, 25, SWP_NOZORDER);

    yStart += ySpacing;
    SetWindowPos(hwndDateLabel, NULL, labelX, yStart, labelWidth, 40, SWP_NOZORDER);
    SetWindowPos(hwndDateEdit, NULL, editX, yStart, editWidth, 25, SWP_NOZORDER);

    yStart += 2 * ySpacing;
    SetWindowPos(hwndPriorityLabel, NULL, labelX, yStart, labelWidth, 40, SWP_NOZORDER);
    SetWindowPos(hwndPriorityEdit, NULL, editX, yStart, editWidth, 25, SWP_NOZORDER);

    yStart += 2 * ySpacing;
    SetWindowPos(hwndList, NULL, 10, yStart, width - 30, height - 320, SWP_NOZORDER);
    SetWindowPos(hwndAddButton, NULL, buttonX, height - 40, buttonWidth, 30, SWP_NOZORDER);

    // Dynamically position the "Made by-Eshan Singh" label on the right side
    int labelwidth = 180;
    int labelRight = width - 5;
    int labelHeight = 40;
    SetWindowPos(hwndMadeByLabel, NULL, labelRight - labelWidth, height - labelHeight - 10, labelWidth, labelHeight, SWP_NOZORDER);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int width, height;
    RECT rect;

    switch (msg) {
    case WM_CREATE:
        hwndDescriptionLabel = CreateWindowA("STATIC", "Task Description:", WS_CHILD | WS_VISIBLE, 10, 10, 120, 20, hwnd, NULL, NULL, NULL);
        hwndEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 140, 10, 200, 25, hwnd, NULL, NULL, NULL);

        hwndDateLabel = CreateWindowA("STATIC", "Date (dd mm yyyy):", WS_CHILD | WS_VISIBLE, 10, 40, 120, 40, hwnd, NULL, NULL, NULL);
        hwndDateEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 140, 40, 200, 25, hwnd, NULL, NULL, NULL);

        hwndPriorityLabel = CreateWindowA("STATIC", "Priority (high/low):", WS_CHILD | WS_VISIBLE, 10, 80, 120, 40, hwnd, NULL, NULL, NULL);
        hwndPriorityEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 140, 80, 200, 25, hwnd, NULL, NULL, NULL);

        hwndList = CreateWindowA("LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 10, 120, 330, 150, hwnd, NULL, NULL, NULL);
        hwndAddButton = CreateWindowA("BUTTON", "Add Task", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 280, 100, 30, hwnd, (HMENU)1, NULL, NULL);

        // Create a static control for "Made by-Eshan Singh" text
        hwndMadeByLabel = CreateWindowA("STATIC", "Made by-\nEshan Singh", WS_CHILD | WS_VISIBLE, 10, 320, 180, 20, hwnd, NULL, NULL, NULL);
        break;
    case WM_SIZE:
        GetClientRect(hwnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        ResizeControls(hwnd, width, height);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            int textLength = GetWindowTextLengthA(hwndEdit);
            if (textLength > 0) {
                CHAR buffer[256];
                GetWindowTextA(hwndEdit, buffer, textLength + 1);
                CHAR dateBuffer[256];
                GetWindowTextA(hwndDateEdit, dateBuffer, sizeof(dateBuffer));
                CHAR priorityBuffer[256];
                GetWindowTextA(hwndPriorityEdit, priorityBuffer, sizeof(priorityBuffer));

                std::string taskDescription(buffer);
                std::string taskDate(dateBuffer);
                std::string taskPriority(priorityBuffer);

                if (!taskDescription.empty() && !taskPriority.empty()) {
                    AddTask(taskDescription, taskDate, taskPriority);
                }
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXA wc;
    HWND hwnd;
    MSG Msg;

    // Register window class
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
    wc.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create window
    hwnd = CreateWindowExA(WS_EX_CLIENTEDGE, szClassName, "To-Do App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Maximize the window by default
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);

    UpdateWindow(hwnd);

    // Message loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}
