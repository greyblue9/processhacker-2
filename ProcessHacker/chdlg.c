/*
 * Process Hacker -
 *   choice dialog
 *
 * Copyright (C) 2010-2013 wj32
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

#include <phapp.h>
#include <settings.h>
#include <windowsx.h>

typedef struct _CHOICE_DIALOG_CONTEXT
{
    PWSTR Title;
    PWSTR Message;
    PWSTR *Choices;
    ULONG NumberOfChoices;
    PWSTR Option;
    ULONG Flags;
    PPH_STRING *SelectedChoice;
    PBOOLEAN SelectedOption;
    PWSTR SavedChoicesSettingName;

    HWND ComboBoxHandle;
} CHOICE_DIALOG_CONTEXT, *PCHOICE_DIALOG_CONTEXT;

INT_PTR CALLBACK PhpChoiceDlgProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    );

/**
 * Prompts the user for input.
 *
 * \remarks If \c PH_CHOICE_DIALOG_PASSWORD is specified, the string
 * returned in \a SelectedChoice is NOT auto-dereferenced.
 */
BOOLEAN PhaChoiceDialog(
    _In_ HWND ParentWindowHandle,
    _In_ PWSTR Title,
    _In_ PWSTR Message,
    _In_opt_ PWSTR *Choices,
    _In_opt_ ULONG NumberOfChoices,
    _In_opt_ PWSTR Option,
    _In_ ULONG Flags,
    _Inout_ PPH_STRING *SelectedChoice,
    _Inout_opt_ PBOOLEAN SelectedOption,
    _In_opt_ PWSTR SavedChoicesSettingName
    )
{
    CHOICE_DIALOG_CONTEXT context;

    context.Title = Title;
    context.Message = Message;
    context.Choices = Choices;
    context.NumberOfChoices = NumberOfChoices;
    context.Option = Option;
    context.Flags = Flags;
    context.SelectedChoice = SelectedChoice;
    context.SelectedOption = SelectedOption;
    context.SavedChoicesSettingName = SavedChoicesSettingName;

    return DialogBoxParam(
        PhInstanceHandle,
        MAKEINTRESOURCE(IDD_CHOOSE),
        ParentWindowHandle,
        PhpChoiceDlgProc,
        (LPARAM)&context
        ) == IDOK;
}

INT_PTR CALLBACK PhpChoiceDlgProc(
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
            PCHOICE_DIALOG_CONTEXT context = (PCHOICE_DIALOG_CONTEXT)lParam;
            ULONG type;
            SIZE_T i;
            HWND comboBoxHandle;
            HWND checkBoxHandle;
            RECT checkBoxRect;
            RECT rect;
            ULONG diff;

            SetProp(hwndDlg, PhMakeContextAtom(), (HANDLE)context);
            PhCenterWindow(hwndDlg, GetParent(hwndDlg));

            SetWindowText(hwndDlg, context->Title);
            SetWindowText(GetDlgItem(hwndDlg, IDC_MESSAGE), context->Message);

            type = context->Flags & PH_CHOICE_DIALOG_TYPE_MASK;

            // Select the control to show, depending on the type. This is
            // because it is impossible to change the style of the combo box
            // after it is created.
            switch (type)
            {
            case PH_CHOICE_DIALOG_USER_CHOICE:
                comboBoxHandle = GetDlgItem(hwndDlg, IDC_CHOICEUSER);
                ShowWindow(GetDlgItem(hwndDlg, IDC_CHOICEUSER), SW_SHOW);
                break;
            case PH_CHOICE_DIALOG_PASSWORD:
                comboBoxHandle = GetDlgItem(hwndDlg, IDC_CHOICESIMPLE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_CHOICESIMPLE), SW_SHOW);

                // Disable combo box features since it isn't a combo box.
                context->SavedChoicesSettingName = NULL;
                break;
            case PH_CHOICE_DIALOG_CHOICE:
            default:
                comboBoxHandle = GetDlgItem(hwndDlg, IDC_CHOICE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_CHOICE), SW_SHOW);
                break;
            }

            context->ComboBoxHandle = comboBoxHandle;

            checkBoxHandle = GetDlgItem(hwndDlg, IDC_OPTION);

            if (type == PH_CHOICE_DIALOG_PASSWORD)
            {
                // Nothing
            }
            else if (type == PH_CHOICE_DIALOG_USER_CHOICE && context->SavedChoicesSettingName)
            {
                PPH_STRING savedChoices = PhGetStringSetting(context->SavedChoicesSettingName);
                ULONG_PTR indexOfDelim;
                PPH_STRING savedChoice;

                i = 0;

                // Split the saved choices using the delimiter.
                while (i < savedChoices->Length / 2)
                {
                    // BUG BUG BUG - what if the user saves "\s"?
                    indexOfDelim = PhFindStringInString(savedChoices, i, L"\\s");

                    if (indexOfDelim == -1)
                        indexOfDelim = savedChoices->Length / 2;

                    savedChoice = PhSubstring(savedChoices, i, indexOfDelim - i);

                    if (savedChoice->Length != 0)
                    {
                        PPH_STRING unescaped;

                        unescaped = PhUnescapeStringForDelimiter(savedChoice, '\\');
                        ComboBox_InsertString(comboBoxHandle, -1, unescaped->Buffer);
                        PhDereferenceObject(unescaped);
                    }

                    PhDereferenceObject(savedChoice);

                    i = indexOfDelim + 2;
                }

                PhDereferenceObject(savedChoices);
            }
            else
            {
                for (i = 0; i < context->NumberOfChoices; i++)
                {
                    ComboBox_AddString(comboBoxHandle, context->Choices[i]);
                }

                context->SavedChoicesSettingName = NULL; // make sure we don't try to save the choices
            }

            if (type == PH_CHOICE_DIALOG_PASSWORD)
            {
                if (*context->SelectedChoice)
                    SetWindowText(comboBoxHandle, (*context->SelectedChoice)->Buffer);

                Edit_SetSel(comboBoxHandle, 0, -1);
            }
            else if (type == PH_CHOICE_DIALOG_USER_CHOICE || type == PH_CHOICE_DIALOG_CHOICE)
            {
                // If we failed to choose a default choice based on what was specified,
                // select the first one if possible, or set the text directly.
                if (!(*context->SelectedChoice) || PhSelectComboBoxString(
                    comboBoxHandle, (*context->SelectedChoice)->Buffer, FALSE) == CB_ERR)
                {
                    if (type == PH_CHOICE_DIALOG_USER_CHOICE && *context->SelectedChoice)
                    {
                        SetWindowText(comboBoxHandle, (*context->SelectedChoice)->Buffer);
                    }
                    else if (type == PH_CHOICE_DIALOG_CHOICE && context->NumberOfChoices != 0)
                    {
                        ComboBox_SetCurSel(comboBoxHandle, 0);
                    }
                }

                if (type == PH_CHOICE_DIALOG_USER_CHOICE)
                    ComboBox_SetEditSel(comboBoxHandle, 0, -1);
            }

            if (context->Option)
            {
                SetWindowText(checkBoxHandle, context->Option);

                if (context->SelectedOption)
                    Button_SetCheck(checkBoxHandle, *context->SelectedOption ? BST_CHECKED : BST_UNCHECKED);
            }
            else
            {
                // Hide the check box and move the buttons up.

                ShowWindow(checkBoxHandle, SW_HIDE);
                GetWindowRect(checkBoxHandle, &checkBoxRect);
                MapWindowPoints(NULL, hwndDlg, (POINT *)&checkBoxRect, 2);
                GetWindowRect(GetDlgItem(hwndDlg, IDOK), &rect);
                MapWindowPoints(NULL, hwndDlg, (POINT *)&rect, 2);
                diff = rect.top - checkBoxRect.top;

                // OK
                rect.top -= diff;
                rect.bottom -= diff;
                SetWindowPos(GetDlgItem(hwndDlg, IDOK), NULL, rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOACTIVATE | SWP_NOZORDER);

                // Cancel
                GetWindowRect(GetDlgItem(hwndDlg, IDCANCEL), &rect);
                MapWindowPoints(NULL, hwndDlg, (POINT *)&rect, 2);
                rect.top -= diff;
                rect.bottom -= diff;
                SetWindowPos(GetDlgItem(hwndDlg, IDCANCEL), NULL, rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOACTIVATE | SWP_NOZORDER);

                // Window
                GetWindowRect(hwndDlg, &rect);
                rect.bottom -= diff;
                SetWindowPos(hwndDlg, NULL, rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOACTIVATE | SWP_NOZORDER);
            }

            SendMessage(hwndDlg, WM_NEXTDLGCTL, (WPARAM)comboBoxHandle, TRUE);
        }
        break;
    case WM_DESTROY:
        {
            RemoveProp(hwndDlg, PhMakeContextAtom());
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
                    PCHOICE_DIALOG_CONTEXT context = (PCHOICE_DIALOG_CONTEXT)GetProp(hwndDlg, PhMakeContextAtom());
                    PPH_STRING selectedChoice;

                    if ((context->Flags & PH_CHOICE_DIALOG_TYPE_MASK) != PH_CHOICE_DIALOG_PASSWORD)
                    {
                        selectedChoice = PhAutoDereferenceObject(PhGetWindowText(context->ComboBoxHandle));
                        *context->SelectedChoice = selectedChoice;
                    }
                    else
                    {
                        // Password values are never auto-dereferenced.
                        selectedChoice = PhGetWindowText(context->ComboBoxHandle);
                        *context->SelectedChoice = selectedChoice;
                    }

                    if (context->Option && context->SelectedOption)
                        *context->SelectedOption = Button_GetCheck(GetDlgItem(hwndDlg, IDC_OPTION)) == BST_CHECKED;

                    if (context->SavedChoicesSettingName)
                    {
                        PH_STRING_BUILDER savedChoices;
                        ULONG i;
                        ULONG choicesToSave = PH_CHOICE_DIALOG_SAVED_CHOICES;
                        PPH_STRING choice;
                        PPH_STRING escaped;

                        PhInitializeStringBuilder(&savedChoices, 100);

                        // Push the selected choice to the top, then save the others.

                        if (selectedChoice->Length != 0)
                        {
                            escaped = PhEscapeStringForDelimiter(selectedChoice, '\\');
                            PhAppendStringBuilder(&savedChoices, &escaped->sr);
                            PhDereferenceObject(escaped);
                            PhAppendStringBuilder2(&savedChoices, L"\\s");
                        }

                        for (i = 1; i < choicesToSave; i++)
                        {
                            choice = PhGetComboBoxString(context->ComboBoxHandle, i - 1);

                            if (!choice)
                                break;

                            // Don't save the choice if it's the same as the one
                            // entered by the user (since we already saved it above).
                            if (PhEqualString(choice, selectedChoice, FALSE))
                            {
                                PhDereferenceObject(choice);
                                choicesToSave++; // useless for now, but may be needed in the future
                                continue;
                            }

                            escaped = PhEscapeStringForDelimiter(choice, '\\');
                            PhAppendStringBuilder(&savedChoices, &escaped->sr);
                            PhDereferenceObject(escaped);
                            PhDereferenceObject(choice);

                            PhAppendStringBuilder2(&savedChoices, L"\\s");
                        }

                        if (PhEndsWithString2(savedChoices.String, L"\\s", FALSE))
                            PhRemoveEndStringBuilder(&savedChoices, 2);

                        PhSetStringSetting2(context->SavedChoicesSettingName, &savedChoices.String->sr);
                        PhDeleteStringBuilder(&savedChoices);
                    }

                    EndDialog(hwndDlg, IDOK);
                }
                break;
            }
        }
        break;
    }

    return FALSE;
}
