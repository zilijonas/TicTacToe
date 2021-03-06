// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MAINICON));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//

//Global Game Vars:
HFONT hFont = CreateFont(32, 0, 1, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
HFONT hFontTitle = CreateFont(82, 0, 4, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Comic Sans MS");
const int CELL_SIZE = 120;
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
int playerTurn = 1;
int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int winner = 0;
int wins[3];
int score[2] = { 0, 0 };
int hWndMetrics [2] = { 2, 3 };

BOOL GetGameBoardRect(HWND hwnd, RECT * pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc)) {
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;

		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;

		return TRUE;
	}
	SetRectEmpty(pRect);
	return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumberFromClick(HWND hwnd, int x, int y)
{
	POINT pt = { x, y };
	RECT rc;

	if (GetGameBoardRect(hwnd, &rc)) 
	{
		if (PtInRect(&rc, pt))
		{
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			return column + row * 3;
		}
	}

	return -1;
}

BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoardRect(hWnd, &rcBoard))
	{		
		int y = index / 3; //Eilute
		int x = index % 3; //Stulpelis

		pRect->left = rcBoard.left + x * CELL_SIZE + 1;
		pRect->top = rcBoard.top + y * CELL_SIZE + 1;

		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}

	return FALSE;
}

/*
 Returns:
  0 - No Winner
  1 - Player 1 wins
  2 - Player 2 wins
  3 - Draw

  0, 1, 2
  3, 4, 5
  6, 7, 8
*/

int GetWinner(int wins[3])
{
	int cells[] = { 0,1,2,  3,4,5,  6,7,8,  0,3,6,  1,4,7,  2,5,8,  0,4,8,  2,4,6 };

	for (int i = 0; i < ARRAYSIZE(cells); i+=3)
	{
		if (0 != gameBoard[cells[i]] && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			wins[0] = cells[i];
			wins[1] = cells[i+1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
		if (0 == gameBoard[i])
			return 0;

	return 3;
}

void ShowTurn(HWND hwnd, HDC hdc)
{
	RECT rc;

	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2";

	const WCHAR * pszTurnText = NULL;

	switch (winner)
	{
	case 0:
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;
	case 1:
		pszTurnText = L"Player 1 is the winner!";
		break;
	case 2:
		pszTurnText = L"Player 2 is the winner!";
		break;
	case 3:
		pszTurnText = L"It's a draw!";
		break;
	}

	if (NULL != pszTurnText && GetClientRect(hwnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hFont);
		SetTextColor(hdc, RGB(255, 255, 0));
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER | DT_VCENTER);
	}
}

void DrawIconCentered(HDC hdc, RECT * pRect, HICON hIcon)
{
	static const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
	static const int ICON_HEIGH = GetSystemMetrics(SM_CYICON);

	if (NULL != pRect)
	{
		int left = pRect->left+((pRect->right - pRect->left) - ICON_WIDTH)/2;
		int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGH) / 2;
		DrawIcon(hdc, left, top, hIcon);
	}
}

void WinnerFields(HWND hwnd, HDC hdc)
{
	RECT rcWin;

	for (int i = 0; i < 3; ++i)
	{
		if (GetCellRect(hwnd, wins[i], &rcWin))
		{
			FillRect(hdc, &rcWin, winner == 1 ? hbr1 : hbr2);
			DrawIconCentered(hdc, &rcWin, winner == 1 ? hIcon1 : hIcon2);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			hbr1 = CreateSolidBrush(RGB(255, 0, 0));
			hbr2 = CreateSolidBrush(RGB(0, 0, 255));

			hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
			hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));

			RECT rcC;
			if (GetClientRect(hWnd, &rcC))
			{
					HWND hwndButton = CreateWindow(
					L"BUTTON",  // Predefined class; Unicode assumed 
					L"New Game",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					rcC.right / 2 - 100,         // x position 
					rcC.bottom - 85,         // y position 
					200,        // Button width
					35,        // Button height
					hWnd,     // Parent window
					(HMENU) ID_NEWGAME,       // No menu.
					(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					NULL);      // Pointer not needed. 
			}
		}
		break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_NEWGAME:
				{
					playerTurn = 1;
					winner = 0;
					ZeroMemory(gameBoard, sizeof(gameBoard));
						
					InvalidateRect(hWnd, NULL, TRUE); 
					UpdateWindow(hWnd);
				}
				break;
			case ID_RESETSCORE:
			{
				int ret = MessageBox(hWnd, L"Are you sure you want to reset score?",
					L"RESET SCORE",
					MB_YESNO | MB_ICONQUESTION);
				if (IDYES == ret)
				{
					playerTurn = 1;
					winner = 0;
					ZeroMemory(gameBoard, sizeof(gameBoard));
					ZeroMemory(score, sizeof(score));

					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd); 
				}
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

	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			
			if (0 == playerTurn)
				break;
			int index = GetCellNumberFromClick(hWnd, xPos, yPos);
			
			HDC hdc = GetDC(hWnd);
			if (NULL != hdc)
			{
				if (index != -1)
				{
					RECT rcCell;
					if (0 == gameBoard[index] && GetCellRect(hWnd, index, &rcCell))
					{
						gameBoard[index] = playerTurn;

						DrawIconCentered(hdc, &rcCell, playerTurn == 1 ? hIcon1 : hIcon2);

						winner = GetWinner(wins);
						if (1 == winner || 2 == winner)
						{
							WinnerFields(hWnd, hdc);
							(winner == 1) ? score[0]++ : score[1]++;

							ShowTurn(hWnd, hdc);

							MessageBox(hWnd, 
								(winner == 1) ? L"Player 1 has won!" : L"Player 2 has won!", 
								L"Congratulations!", 
								MB_OK | MB_ICONINFORMATION);
							playerTurn = 0;
						}
						else if (3 == winner)
						{
							MessageBox(hWnd,
								L"Friendship wins!",
								L"It's a draw.",
								MB_OK | MB_ICONEXCLAMATION);
							playerTurn = 0;
						}
						else if (0 == winner)
							playerTurn = (playerTurn == 2) ? 1 : 2;

						ShowTurn(hWnd, hdc);
					}
					
				}

				ReleaseDC(hWnd, hdc);
			}
		}
		break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;

			pMinMax->ptMinTrackSize.x = CELL_SIZE * 6;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
			pMinMax->ptMaxTrackSize.x = CELL_SIZE * 6;
			pMinMax->ptMaxTrackSize.y = CELL_SIZE * 5;
		}
		break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;

			if (GetGameBoardRect(hWnd, &rc))
			{
				RECT rcClient;

				if (GetClientRect(hWnd, &rcClient))
				{

					const WCHAR szPlayer1 [] = L"Player 1";
					const WCHAR szPlayer2 [] = L"Player 2";
					TCHAR buff[10];
					
					
					SetBkMode(hdc, TRANSPARENT);

					SelectObject(hdc, hFontTitle);
					TextOut(hdc, rcClient.right / 2 - 150, 6, L"TicTacToe", 9);

					SelectObject(hdc, hFont);

					SetTextColor(hdc, RGB(255, 0, 0));
					TextOut(hdc, 16, 16, szPlayer1, ARRAYSIZE(szPlayer1));
					DrawIcon(hdc, 42, 60, hIcon1);
					SetTextColor(hdc, RGB(255, 255, 0));
					TextOut(hdc, 16, rcClient.bottom - 80, buff, wsprintf(buff, TEXT("Score: %d"), score[0]));

					SetTextColor(hdc, RGB(0, 0, 255));
					TextOut(hdc, rcClient.right - 122, 16, szPlayer2, ARRAYSIZE(szPlayer2));
					DrawIcon(hdc, rcClient.right - 84, 60, hIcon2);
					SetTextColor(hdc, RGB(255, 255, 0));
					TextOut(hdc, rcClient.right - 128, rcClient.bottom - 80, buff, wsprintf(buff, TEXT("Score: %d"), score[1]));

					ShowTurn(hWnd, hdc);
				}

				FillRect(hdc, &rc, (HBRUSH)LTGRAY_BRUSH);


				for (int i = 0; i < 4; ++i) {
					DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom); //Vertikalios
					DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i); //Horizontalios
				}

				RECT rcCell;

				//Perpiesti uzimtus laukelius
				for (int i = 0; i < 9; ++i)
				{
					if (0 != gameBoard[i] && GetCellRect(hWnd, i, &rcCell))
					{
						DrawIconCentered(hdc, &rcCell, gameBoard[i] == 1 ? hIcon1 : hIcon2);
					}
				}

				if (1 == winner || 2 == winner)
					WinnerFields(hWnd, hdc);
			}
			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		{
			DeleteObject(hbr1);
			DeleteObject(hbr2);
			DestroyIcon(hIcon1);
			DestroyIcon(hIcon2);
			PostQuitMessage(0);
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
