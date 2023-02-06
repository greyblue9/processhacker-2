/*
 * Process Hacker Extended Tools -
 *   options dialog
 *
 * Copyright (C) 2010 wj32
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "exttools.h"
#include "resource.h"
#include <windowsx.h>

INT_PTR CALLBACK OptionsDlgProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    );

VOID EtShowOptionsDialog(
    _In_ HWND ParentWindowHandle
    )
{
    DialogBox(
        PluginInstance->DllBase,
        MAKEINTRESOURCE(IDD_OPTIONS),
        ParentWindowHandle,
        OptionsDlgProc
        );
}

INT_PTR CALLBACK OptionsDlgProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    )
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            Button_SetCheck(GetDlgItem(hwndDlg, IDC_ENABLEETWMONITOR), PhGetIntegerSetting(SETTING_NAME_ENABLE_ETW_MONITOR) ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(hwndDlg, IDC_ENABLEGPUMONITOR), PhGetIntegerSetting(SETTING_NAME_ENABLE_GPU_MONITOR) ? BST_CHECKED : BST_UNCHECKED);

            if (WindowsVersion < WINDOWS_7)
                EnableWindow(GetDlgItem(hwndDlg, IDC_ENABLEGPUMONITOR), FALSE);
        }
        break;
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDCANCEL:
                EndDialog(hwndDlg, IDCANCEL);
                break;
            case IDOK:
                {
                    PhSetIntegerSetting(SETTING_NAME_ENABLE_ETW_MONITOR,
                        Button_GetCheck(GetDlgItem(hwndDlg, IDC_ENABLEETWMONITOR)) == BST_CHECKED);
                    PhSetIntegerSetting(SETTING_NAME_ENABLE_GPU_MONITOR,
                        Button_GetCheck(GetDlgItem(hwndDlg, IDC_ENABLEGPUMONITOR)) == BST_CHECKED);

                    EndDialog(hwndDlg, IDOK);
                }
                break;
            }
        }
        break;
    }

    return FALSE;
}
