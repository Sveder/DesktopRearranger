//********************************************************************************\\
//                             Desktop Rearranger
//     Rearranger the icons on the desktop in various forms, most notably, a
//     clock.
//
//     Written by Michael Sverdlin, Mikle3@gmail.com.
//     If anyone use this in an interesting way or modifies it in any interesting
//     way, email me.
//
//
//
// License
// -------
// This work is licensed under the Creative Commons Attribution 3.0 Unported
// License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by/3.0/ or send a letter to Creative
// Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
//********************************************************************************//

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

#define	MAX_X			(GetSystemMetrics(16))
#define MAX_Y			(GetSystemMetrics(17))
#define MAX_ICONS		(100)
#define MAX_IN_LINE		(20)
#define MAX_IN_COLUMN	(14)
#define ICON_X			(MAX_X / MAX_IN_LINE + 10)
#define ICON_Y			(MAX_Y / MAX_IN_COLUMN + 10)

void ErrorExit(LPTSTR lspzFunction); //Print the error string and exits.
BOOL DeterminePositions(INT iNumOfItems, POINT** ppntArr);

ULONG g_ulIterNum = 0;
LONG  g_lArrSize  = 0;

BOOL RearrangeWindow(HWND hWin)
{
	POINT *pntArrayOfDoom = NULL;
	int iNumOfItems = 0;

	//Get the number of items, to be used to determine how to show the icons:
	iNumOfItems = ListView_GetItemCount(hWin);

	if (!iNumOfItems)
	{
		MessageBox(NULL, L"There are no icons on your desktop, leave me be.", L"Error", MB_OK);
		ExitProcess(1);
	}

	do
	{
		printf("The item count is %i\n", iNumOfItems);
		if (!DeterminePositions(iNumOfItems, &pntArrayOfDoom))
		{
			printf("Error determining positions!\n");
			continue;
		}

		for (int i = 0; i < iNumOfItems; ++i)
		{
			if (!ListView_SetItemPosition(hWin, i, pntArrayOfDoom[i].x, pntArrayOfDoom[i].y))
			{
				printf("Error moving icon %i.\n", i);
			}
		}

		iNumOfItems = ListView_GetItemCount(hWin);
		if (18 == iNumOfItems)
		{
			Sleep(60 * 1000);
		}
		else if ((18 < iNumOfItems) && (65 > iNumOfItems))
		{
			Sleep(2000);
			g_ulIterNum++;
		}
	} while ((17 < iNumOfItems) && (65 > iNumOfItems));

	if (NULL != pntArrayOfDoom)
	{
		HeapFree(GetProcessHeap(), 0, pntArrayOfDoom);
	}

	return TRUE;
}

int main(int argc, char* argv[])
{
	//Get the program manager window whose grand child is the desktop:
	HWND hwndProgramManagerWindow = FindWindow(L"Progman", NULL);
	if (NULL == hwndProgramManagerWindow)
		ErrorExit(L"FindWindow");

	//Get the father of the desktop:
	HWND hwndFatherWindow = FindWindowEx(hwndProgramManagerWindow, NULL, L"SHELLDLL_DefView", NULL);
	if (NULL == hwndFatherWindow)
		ErrorExit(L"first FindWindowEx");

	//Finally get the allmighty desktop window:
	HWND hwndDesktop = FindWindowEx(hwndFatherWindow, NULL, L"SysListView32", NULL);
	if (NULL == hwndDesktop)
		ErrorExit(L"second FindWindowEx");

	RearrangeWindow(hwndDesktop);
	return 0;
}

BOOL DeterminePositions(int iNumOfItems, POINT **ppntArr)
{
	POINT *pstbArray = NULL;
	int i = 0, iMode = 0;
	float angle_delta, current_angle;
	BOOL bIsEvenNumOfItems = iNumOfItems % 2 ? TRUE : FALSE;

	pstbArray = *ppntArr;
	if (NULL == pstbArray)
	{
		pstbArray = (POINT*)HeapAlloc(GetProcessHeap(), 0, sizeof(POINT) * iNumOfItems);
		if (NULL == pstbArray)
		{
			return FALSE;
		}

		g_lArrSize = iNumOfItems;
		*ppntArr = pstbArray;
	}
	else if (g_lArrSize < iNumOfItems)
	{
		pstbArray = (POINT*)HeapReAlloc(GetProcessHeap(), 0, pstbArray, sizeof(POINT) * iNumOfItems);
		if (NULL == pstbArray)
		{
			return FALSE;
		}

		g_lArrSize = iNumOfItems;
		*ppntArr = pstbArray;
	}

	if (MAX_IN_COLUMN < iNumOfItems)
		iMode = 6;
	if (MAX_IN_LINE < iNumOfItems)
		iMode = 2;
	if (MAX_IN_COLUMN + MAX_IN_COLUMN < iNumOfItems)
		iMode = 5;
	if (MAX_IN_LINE + MAX_IN_LINE < iNumOfItems)
		iMode = 5;
	if (18 == iNumOfItems)
		iMode = 7;
	if ((18 < iNumOfItems) && (31 > iNumOfItems))
		iMode = 9;
	if ((30 < iNumOfItems) && (56 > iNumOfItems))
		iMode = 8;
	if ((55 < iNumOfItems) && (66 > iNumOfItems))
		iMode = 9;

	switch (iMode)
	{
		case 0:
			for (i = 0; i < iNumOfItems; ++i)
			{
				pstbArray->x = i * ICON_X + 50;
				pstbArray->y = i * ICON_Y + 20;
				pstbArray++;
			}
			break;

		case 1:
			for (i = 0; i < iNumOfItems; ++i)
			{
				pstbArray->x = i * ICON_X + 50;
				pstbArray->y = MAX_Y - 20;
				pstbArray++;
			}
			break;

		case 3:
			for (i = 0; i < iNumOfItems / 2; ++i)
			{
				pstbArray->x = i * ICON_X + 50;
				pstbArray->y = MAX_Y - 20;
				pstbArray++;
				pstbArray->x = i * ICON_X + 50;
				pstbArray->y = 50;
				pstbArray++;
			}
			if (!bIsEvenNumOfItems)
			{
				printf("Mooo");
				pstbArray->x = MAX_X / 2;
				pstbArray->y = MAX_Y / 2;
			}
			break;

		case 5:
			angle_delta = 4 * asinf(1) / iNumOfItems;
			current_angle = 0;
			for (int i = 0; i < iNumOfItems; ++i)
			{
				pstbArray->x = int(576 + 200 * cosf(current_angle));
				pstbArray->y = int(432 + 200 * sinf(current_angle)) - 50;
				pstbArray++;
				current_angle += angle_delta;
			}
			break;

		case 2:
		{
			int iStartingX = 200;
			int iStartingY = 25;
			int iEndingX = MAX_X - 200;
			int iEndingY = MAX_Y;
			int iDeltaX = (iEndingX - iStartingX) / (iNumOfItems / 2);
			int iDeltaY = (iEndingY - iStartingY) / (iNumOfItems / 2);

			for (i = 0; i < iNumOfItems / 2; ++i)
			{
				pstbArray->x = iStartingX + (i * iDeltaX);
				pstbArray->y = iStartingY + (i * iDeltaY);
				pstbArray++;
				pstbArray->x = iEndingX - (i * iDeltaX);
				pstbArray->y = iStartingY + (i * iDeltaY);
				pstbArray++;
			}
			if (!bIsEvenNumOfItems)
			{
				printf("Mooo");
				pstbArray->x = MAX_X / 2;
				pstbArray->y = MAX_Y / 2;
			}
			break;
		}
		case 4:
			break;

		case 7:
		{
			time_t ltime = 0;
			struct tm *today = NULL;
			int iHour = 0;
			int iMin = 0;
			ULONG ulRadius = 360;

			angle_delta = 4 * asinf(1) / 12;
			current_angle = 0;
			for (int i = 0; i < 12; ++i)
			{
				pstbArray->x = int(576 + ulRadius * cosf(current_angle));
				pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
				pstbArray++;
				current_angle += angle_delta;
			}

			time(&ltime);
			today = localtime(&ltime);
			iHour = today->tm_hour % 12;
			iMin = today->tm_min;

			pstbArray->x = int(576);
			pstbArray->y = int(432) - 50;
			pstbArray++;

			current_angle = angle_delta * (iHour + 9);
			ulRadius = 160;
			pstbArray->x = int(576 + ulRadius * cosf(current_angle));
			pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
			pstbArray++;
			ulRadius = 80;
			pstbArray->x = int(576 + ulRadius * cosf(current_angle));
			pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
			pstbArray++;


			angle_delta = 4 * asinf(1) / 60;
			current_angle = angle_delta * (iMin + 45);

			ulRadius = 280;
			pstbArray->x = int(576 + ulRadius * cosf(current_angle));
			pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
			pstbArray++;
			ulRadius = 190;
			pstbArray->x = int(576 + ulRadius * cosf(current_angle));
			pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
			pstbArray++;
			ulRadius = 100;
			pstbArray->x = int(576 + ulRadius * cosf(current_angle));
			pstbArray->y = int(432 + ulRadius * sinf(current_angle)) - 50;
			pstbArray++;

			break;
		}
		case 8:
		{		
			float fPi = 2 * asinf(1);
			float fCurrentRadius = 100;
			ULONG ulIconSize = 60;
			float fPhase = g_ulIterNum * 2 * fPi / 60;

			current_angle = fPhase;

			for (i = 0; i < iNumOfItems; ++i)
			{
				pstbArray[(i + g_ulIterNum) % iNumOfItems].x = int(576 + fCurrentRadius * cosf(current_angle));
				pstbArray[(i + g_ulIterNum) % iNumOfItems].y = int(432 + fCurrentRadius * sinf(current_angle)) - 50;
				angle_delta = ulIconSize / fCurrentRadius;
				current_angle += angle_delta;
				fCurrentRadius += 2 + ulIconSize * ulIconSize / (2 * fPi * fCurrentRadius);
			}
			break;
		}
		case 9:
		{
			float fPi = 2 * asinf(1);
			float fCurrentRadius = 80;
			ULONG ulIconSize = 60;
			float fPhase = g_ulIterNum * 2 * fPi / 60;
			ULONG ulBrunchesNum = 5;
			float fBrunchPhase = 2 * fPi / ulBrunchesNum;
			int j = 0;
			int k = 0;

			current_angle = fPhase;

			for (i = 0; i < iNumOfItems / ulBrunchesNum; ++i)
			{
				for (j = 0; j < ulBrunchesNum; ++j)
				{
					pstbArray[i * ulBrunchesNum + j].x = int(576 + fCurrentRadius * cosf(current_angle + j * fBrunchPhase));
					pstbArray[i * ulBrunchesNum + j].y = int(432 + fCurrentRadius * sinf(current_angle + j * fBrunchPhase)) - 50;
				}
				angle_delta = ulIconSize / fCurrentRadius;
				current_angle += angle_delta;
				fCurrentRadius += 10 * ulIconSize * ulIconSize / (2 * fPi * fCurrentRadius);
			}

			for (k = iNumOfItems - iNumOfItems % ulBrunchesNum; k < iNumOfItems; ++k)
			{
				pstbArray[k].x = int(576 + fCurrentRadius * cosf(current_angle + k * fBrunchPhase));
				pstbArray[k].y = int(432 + fCurrentRadius * sinf(current_angle + k * fBrunchPhase)) - 50;
			}
			break;
		}
		default:
			return FALSE;
	}
	
	return TRUE;
}

void ErrorExit(LPTSTR lpszFunction)
{
	TCHAR szBuf[120];
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf,
		0, NULL);

	wsprintf(szBuf, L"%s failed with error %d: %s", lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, szBuf, L"Error", MB_OK);

	LocalFree(lpMsgBuf);
	ExitProcess(dw);
}