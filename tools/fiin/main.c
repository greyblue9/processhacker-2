/*
 * Process Hacker -
 *   FiIn
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

#include <ph.h>
#include <md5.h>
#include <sha.h>

#define FI_ARG_HELP 1
#define FI_ARG_ACTION 2
#define FI_ARG_NATIVE 3
#define FI_ARG_PATTERN 4
#define FI_ARG_CASESENSITIVE 5
#define FI_ARG_OUTPUT 6
#define FI_ARG_FORCE 7
#define FI_ARG_LENGTH 8

#define HASH_MD5 1
#define HASH_SHA1 2
#define HASH_CRC32 3

PPH_STRING FiArgFileName;
BOOLEAN FiArgHelp;
PPH_STRING FiArgAction;
BOOLEAN FiArgNative;
PPH_STRING FiArgPattern;
BOOLEAN FiArgCaseSensitive;
PPH_STRING FiArgOutput;
BOOLEAN FiArgForce;
ULONG64 FiArgLength = MAXULONG64;

ULONG64 FipDirFileCount;
ULONG64 FipDirDirCount;
ULONG64 FipDirTotalSize;
ULONG64 FipDirTotalAllocSize;

static BOOLEAN NTAPI FiCommandLineCallback(
    _In_opt_ PPH_COMMAND_LINE_OPTION Option,
    _In_opt_ PPH_STRING Value,
    _In_opt_ PVOID Context
    )
{
    if (Option)
    {
        switch (Option->Id)
        {
        case FI_ARG_HELP:
            FiArgHelp = TRUE;
            break;
        case FI_ARG_ACTION:
            PhSwapReference(&FiArgAction, Value);
            break;
        case FI_ARG_NATIVE:
            FiArgNative = TRUE;
            break;
        case FI_ARG_PATTERN:
            PhSwapReference(&FiArgPattern, Value);
            break;
        case FI_ARG_CASESENSITIVE:
            FiArgCaseSensitive = TRUE;
            break;
        case FI_ARG_OUTPUT:
            PhSwapReference(&FiArgOutput, Value);
            break;
        case FI_ARG_FORCE:
            FiArgForce = TRUE;
            break;
        case FI_ARG_LENGTH:
            PhStringToInteger64(&Value->sr, 0, (PLONG64)&FiArgLength);
            break;
        }
    }
    else
    {
        if (!FiArgAction)
            PhSwapReference(&FiArgAction, Value);
        else if (!FiArgFileName)
            PhSwapReference(&FiArgFileName, Value);
    }

    return TRUE;
}

VOID FiPrintHelp(
    VOID
    )
{
    wprintf(
        L"FiIn, by wj32.\n"
        L"fiin action [-C] [-f] [-L length] [-N] [-o filename] [-p pattern] [filename]\n"
        L"\taction      The action to be performed.\n"
        L"\t-C          Specifies that file names are case-sensitive.\n"
        L"\t-f          Forces the action to succeed by overwriting files.\n"
        L"\t-L length   Specifies the length for an operation.\n"
        L"\t-N          Specifies that file names are in native format.\n"
        L"\t-o filename Specifies the output file name, or the command line.\n"
        L"\t-p pattern  A search pattern for listings.\n"
        L"\n"
        L"Actions:\n"
        L"copy\n"
        L"del\n"
        L"dir\n"
        L"execute\n"
        L"hash\n"
        L"map\n"
        L"mkdir\n"
        L"rename\n"
        L"streams\n"
        L"touch\n"
        );
}

PPH_STRING FiFormatFileName(
    _In_ PPH_STRING FileName
    )
{
    if (!FiArgNative)
    {
        PPH_STRING fileName;
        PPH_STRING fullPath;
        UNICODE_STRING fileNameUs;

        // Get the full path first so we can detect long file names.
        fullPath = PhGetFullPath(FileName->Buffer, NULL);

        if (fullPath)
        {
            if (fullPath->Length / 2 > MAX_PATH)
            {
                wprintf(L"Warning: Detected long file name \"%s\".\n", fullPath->Buffer);

                fileName = PhConcatStrings2(L"\\??\\", fullPath->Buffer);
                PhDereferenceObject(fullPath);

                return fileName;
            }

            PhDereferenceObject(fullPath);
        }

        if (RtlDosPathNameToNtPathName_U(
            FileName->Buffer,
            &fileNameUs,
            NULL,
            NULL
            ))
        {
            fileName = PhCreateStringFromUnicodeString(&fileNameUs);
            RtlFreeHeap(RtlProcessHeap(), 0, fileNameUs.Buffer);

            return fileName;
        }
        else
        {
            wprintf(L"Warning: Unable to convert the file name \"%s\" to a NT file name.\n", FileName->Buffer);
            // Fallback method.
            return PhConcatStrings2(L"\\??\\", FileName->Buffer);
        }
    }
    else
    {
        // Some limited conversions are applied for convenience.

        if (FileName->Length >= 2 * 2 && iswalpha(FileName->Buffer[0]) && FileName->Buffer[1] == ':')
        {
            return PhConcatStrings2(L"\\??\\", FileName->Buffer);
        }
        else if (FileName->Buffer[0] != '\\')
        {
            return PhFormatString(
                L"\\??\\%s%s",
                NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath.Buffer,
                FileName->Buffer
                );
        }
        else
        {
            return PhReferenceObject(FileName);
        }
    }
}

BOOLEAN FiCreateFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PPH_STRING FileName,
    _In_opt_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_opt_ ULONG Options
    )
{
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK isb;
    PPH_STRING fileName;
    UNICODE_STRING fileNameUs;

    if (!FileAttributes)
        FileAttributes = FILE_ATTRIBUTE_NORMAL;
    if (!Options)
        Options = FILE_SYNCHRONOUS_IO_NONALERT;

    // Not needed, because we handle Win32 paths anyway.
    //if (!(FiArgNative))
    //{
    //    status = PhCreateFileWin32(
    //        FileHandle,
    //        FileName->Buffer,
    //        DesiredAccess,
    //        FileAttributes,
    //        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    //        CreateDisposition,
    //        Options
    //        );

    //    if (!NT_SUCCESS(status))
    //    {
    //        wprintf(L"Error creating/opening file: %s\n", PhGetNtMessage(status)->Buffer);
    //        return FALSE;
    //    }

    //    return TRUE;
    //}

    fileName = FiFormatFileName(FileName);

    if (!PhStringRefToUnicodeString(&fileName->sr, &fileNameUs))
    {
        PhDereferenceObject(fileName);
        return FALSE;
    }

    InitializeObjectAttributes(
        &oa,
        &fileNameUs,
        (!FiArgCaseSensitive ? OBJ_CASE_INSENSITIVE : 0),
        NULL,
        NULL
        );

    status = NtCreateFile(
        &fileHandle,
        DesiredAccess,
        &oa,
        &isb,
        NULL,
        FileAttributes,
        ShareAccess,
        CreateDisposition,
        Options,
        NULL,
        0
        );

    if (!NT_SUCCESS(status))
    {
        wprintf(L"Error creating/opening file: %s\n", PhGetNtMessage(status)->Buffer);
        return FALSE;
    }

    *FileHandle = fileHandle;

    return TRUE;
}

BOOLEAN NTAPI FipEnumDirectoryFileForDir(
    _In_ PFILE_DIRECTORY_INFORMATION Information,
    _In_opt_ PVOID Context
    )
{
    PPH_STRING date, time, size;
    SYSTEMTIME systemTime;

    PhLargeIntegerToLocalSystemTime(&systemTime, &Information->LastWriteTime);
    date = PhFormatDate(&systemTime, NULL);
    time = PhFormatTime(&systemTime, NULL);
    size = PhFormatUInt64(Information->EndOfFile.QuadPart, TRUE);

    wprintf(
        L"%-10s %12s %c%c%c%c%c%c%c%c %11s %.*s\n",
        date->Buffer,
        time->Buffer,
        (Information->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? '+' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_HIDDEN) ? 'h' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_SYSTEM) ? 's' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_READONLY) ? 'r' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? 'z' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_ENCRYPTED) ? 'e' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) ? '%' : ' ',
        (Information->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ? '*' : ' ',
        size->Buffer,
        Information->FileNameLength / 2,
        Information->FileName
        );

    PhDereferenceObject(date);
    PhDereferenceObject(time);
    PhDereferenceObject(size);

    if (Information->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        FipDirDirCount++;
    else
        FipDirFileCount++;

    FipDirTotalSize += Information->EndOfFile.QuadPart;
    FipDirTotalAllocSize += Information->AllocationSize.QuadPart;

    return TRUE;
}

int __cdecl main(int argc, char *argv[])
{
    static PH_COMMAND_LINE_OPTION options[] =
    {
        { FI_ARG_HELP, L"h", NoArgumentType },
        { FI_ARG_ACTION, L"a", MandatoryArgumentType },
        { FI_ARG_NATIVE, L"N", NoArgumentType },
        { FI_ARG_PATTERN, L"p", MandatoryArgumentType },
        { FI_ARG_CASESENSITIVE, L"C", NoArgumentType },
        { FI_ARG_OUTPUT, L"o", MandatoryArgumentType },
        { FI_ARG_FORCE, L"f", NoArgumentType },
        { FI_ARG_LENGTH, L"L", MandatoryArgumentType }
    };
    PH_STRINGREF commandLine;
    NTSTATUS status = STATUS_SUCCESS;

    if (!NT_SUCCESS(PhInitializePhLibEx(0, 0, 0)))
        return 1;

    PhUnicodeStringToStringRef(&NtCurrentPeb()->ProcessParameters->CommandLine, &commandLine);

    if (!PhParseCommandLine(
        &commandLine,
        options,
        sizeof(options) / sizeof(PH_COMMAND_LINE_OPTION),
        PH_COMMAND_LINE_IGNORE_FIRST_PART,
        FiCommandLineCallback,
        NULL
        ) || FiArgHelp)
    {
        FiPrintHelp();
        return 0;
    }

    if (!FiArgFileName && (
        FiArgAction &&
        PhEqualString2(FiArgAction, L"dir", TRUE)
        ))
    {
        FiArgFileName = PhCreateStringFromUnicodeString(&NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath);
    }

    if (!FiArgAction)
    {
        FiPrintHelp();
        return 1;
    }
    else if (PhEqualString2(FiArgAction, L"map", TRUE))
    {
        WCHAR deviceNameBuffer[7] = L"\\??\\ :";
        ULONG i;
        WCHAR targetNameBuffer[0x100];
        UNICODE_STRING targetName;

        targetName.Buffer = targetNameBuffer;
        targetName.MaximumLength = sizeof(targetNameBuffer);

        for (i = 0; i < 26; i++)
        {
            HANDLE linkHandle;
            OBJECT_ATTRIBUTES oa;
            UNICODE_STRING deviceName;

            deviceNameBuffer[4] = (WCHAR)('A' + i);
            deviceName.Buffer = deviceNameBuffer;
            deviceName.Length = 6 * sizeof(WCHAR);

            InitializeObjectAttributes(
                &oa,
                &deviceName,
                OBJ_CASE_INSENSITIVE,
                NULL,
                NULL
                );

            if (NT_SUCCESS(NtOpenSymbolicLinkObject(
                &linkHandle,
                SYMBOLIC_LINK_QUERY,
                &oa
                )))
            {
                if (NT_SUCCESS(NtQuerySymbolicLinkObject(
                    linkHandle,
                    &targetName,
                    NULL
                    )))
                {
                    wprintf(L"%c: %.*s\n", 'A' + i, targetName.Length / 2, targetName.Buffer);
                }

                NtClose(linkHandle);
            }
        }
    }
    else if (!FiArgFileName)
    {
        wprintf(L"Error: file name missing.\n");
        FiPrintHelp();
        return 1;
    }
    else if (PhEqualString2(FiArgAction, L"hash", TRUE))
    {
        HANDLE fileHandle;
        LARGE_INTEGER fileSize;
        IO_STATUS_BLOCK isb;
        ULONG mode;

        if (!FiArgOutput)
            mode = HASH_MD5;
        else if (PhEqualString2(FiArgOutput, L"md5", TRUE))
            mode = HASH_MD5;
        else if (PhEqualString2(FiArgOutput, L"sha1", TRUE))
            mode = HASH_SHA1;
        else if (PhEqualString2(FiArgOutput, L"crc32", TRUE))
            mode = HASH_CRC32;
        else
        {
            wprintf(L"Invalid hash algorithm. Possibilities: md5, sha1, crc32\n");
            return 1;
        }

        if (FiCreateFile(
            &fileHandle,
            FILE_GENERIC_READ,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_SEQUENTIAL_ONLY
            ))
        {
            if (NT_SUCCESS(status = PhGetFileSize(fileHandle, &fileSize)))
            {
                MD5_CTX md5Context;
                A_SHA_CTX shaContext;
                ULONG crc;
                UCHAR buffer[PAGE_SIZE * 4];
                ULONG64 bytesRemaining;

                bytesRemaining = fileSize.QuadPart;

                switch (mode)
                {
                case HASH_MD5:
                    MD5Init(&md5Context);
                    break;
                case HASH_SHA1:
                    A_SHAInit(&shaContext);
                    break;
                case HASH_CRC32:
                    crc = 0;
                    break;
                }

                while (bytesRemaining)
                {
                    status = NtReadFile(
                        fileHandle,
                        NULL,
                        NULL,
                        NULL,
                        &isb,
                        buffer,
                        sizeof(buffer),
                        NULL,
                        NULL
                        );

                    if (!NT_SUCCESS(status))
                        break;

                    switch (mode)
                    {
                    case HASH_MD5:
                        MD5Update(&md5Context, buffer, (ULONG)isb.Information);
                        break;
                    case HASH_SHA1:
                        A_SHAUpdate(&shaContext, buffer, (ULONG)isb.Information);
                        break;
                    case HASH_CRC32:
                        crc = PhCrc32(crc, buffer, isb.Information);
                        break;
                    }

                    bytesRemaining -= (ULONG)isb.Information;
                }

                if (status == STATUS_END_OF_FILE)
                    status = STATUS_SUCCESS;

                switch (mode)
                {
                case HASH_MD5:
                    {
                        MD5Final(&md5Context);
                        wprintf(L"%s", PhBufferToHexString(md5Context.digest, 16)->Buffer);
                    }
                    break;
                case HASH_SHA1:
                    {
                        UCHAR hash[20];

                        A_SHAFinal(&shaContext, hash);
                        wprintf(L"%s", PhBufferToHexString(hash, 20)->Buffer);
                    }
                    break;
                case HASH_CRC32:
                    {
                        wprintf(L"%08x", crc);
                    }
                    break;
                }

                if (!NT_SUCCESS(status))
                    wprintf(L"Warning: I/O error encountered: %s\n", PhGetNtMessage(status)->Buffer);
            }

            NtClose(fileHandle);
        }

        if (!NT_SUCCESS(status))
        {
            wprintf(L"Error: %s\n", PhGetNtMessage(status)->Buffer);
            return 1;
        }
    }
    else if (PhEqualString2(FiArgAction, L"execute", TRUE))
    {
        if (FiArgNative)
        {
            if (!NT_SUCCESS(status = PhCreateProcess(
                FiFormatFileName(FiArgFileName)->Buffer,
                FiArgOutput ? &FiArgOutput->sr : NULL,
                NULL,
                NULL,
                NULL,
                0,
                NULL,
                NULL,
                NULL,
                NULL
                )))
            {
                wprintf(L"Error: %s\n", PhGetNtMessage(status)->Buffer);
                return 1;
            }
        }
        else
        {
            if (!NT_SUCCESS(status = PhCreateProcessWin32(
                FiArgFileName->Buffer,
                PhGetString(FiArgOutput),
                NULL,
                NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath.Buffer,
                PH_CREATE_PROCESS_NEW_CONSOLE,
                NULL,
                NULL,
                NULL
                )))
            {
                wprintf(L"Error: %s\n", PhGetNtMessage(status)->Buffer);
                return 1;
            }
        }
    }
    else if (PhEqualString2(FiArgAction, L"del", TRUE))
    {
        HANDLE fileHandle;

        if (FiCreateFile(
            &fileHandle,
            DELETE | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            FILE_DISPOSITION_INFORMATION dispositionInfo;
            IO_STATUS_BLOCK isb;

            dispositionInfo.DeleteFile = TRUE;
            if (!NT_SUCCESS(status = NtSetInformationFile(fileHandle, &isb, &dispositionInfo,
                sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation)))
            {
                wprintf(L"Error deleting file: %s\n", PhGetNtMessage(status)->Buffer);
            }

            NtClose(fileHandle);
        }
    }
    else if (PhEqualString2(FiArgAction, L"touch", TRUE))
    {
        HANDLE fileHandle;

        if (FiCreateFile(
            &fileHandle,
            FILE_READ_ATTRIBUTES | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN_IF,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            NtClose(fileHandle);
        }
    }
    else if (PhEqualString2(FiArgAction, L"mkdir", TRUE))
    {
        HANDLE fileHandle;

        if (FiCreateFile(
            &fileHandle,
            FILE_READ_ATTRIBUTES | SYNCHRONIZE,
            FiArgFileName,
            FILE_ATTRIBUTE_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_CREATE,
            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            NtClose(fileHandle);
        }
    }
    else if (PhEqualString2(FiArgAction, L"rename", TRUE))
    {
        HANDLE fileHandle;
        PPH_STRING newFileName;

        if (!FiArgOutput)
        {
            wprintf(L"Error: new file name missing.\n");
            FiPrintHelp();
            return 1;
        }

        newFileName = FiFormatFileName(FiArgOutput);

        if (FiCreateFile(
            &fileHandle,
            DELETE | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            PFILE_RENAME_INFORMATION renameInfo;
            ULONG renameInfoSize;
            IO_STATUS_BLOCK isb;

            renameInfoSize = FIELD_OFFSET(FILE_RENAME_INFORMATION, FileName) + (ULONG)newFileName->Length;
            renameInfo = PhAllocate(renameInfoSize);
            renameInfo->ReplaceIfExists = FiArgForce;
            renameInfo->RootDirectory = NULL;
            renameInfo->FileNameLength = (ULONG)newFileName->Length;
            memcpy(renameInfo->FileName, newFileName->Buffer, newFileName->Length);

            status = NtSetInformationFile(fileHandle, &isb, renameInfo, renameInfoSize, FileRenameInformation);
            PhFree(renameInfo);

            if (!NT_SUCCESS(status))
            {
                wprintf(L"Error renaming file: %s\n", PhGetNtMessage(status)->Buffer);
            }

            NtClose(fileHandle);
        }
    }
    else if (PhEqualString2(FiArgAction, L"copy", TRUE))
    {
        HANDLE fileHandle;
        HANDLE outFileHandle;
        LARGE_INTEGER fileSize;
        FILE_BASIC_INFORMATION basicInfo;

        if (!FiArgOutput)
        {
            wprintf(L"Error: output file name missing.\n");
            FiPrintHelp();
            return 1;
        }

        if (FiCreateFile(
            &fileHandle,
            FILE_READ_ATTRIBUTES | FILE_READ_DATA | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE | FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT
            ) && FiCreateFile(
            &outFileHandle,
            FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | SYNCHRONIZE,
            FiArgOutput,
            0,
            FILE_SHARE_READ | FILE_SHARE_DELETE,
            !FiArgForce ? FILE_CREATE : FILE_OVERWRITE_IF,
            FILE_NON_DIRECTORY_FILE | FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
#define COPY_BUFFER_SIZE 0x10000
            IO_STATUS_BLOCK isb;
            PVOID buffer;
            ULONG64 bytesToCopy = FiArgLength;

            if (NT_SUCCESS(PhGetFileSize(fileHandle, &fileSize)))
            {
                PhSetFileSize(outFileHandle, &fileSize);
            }

            buffer = PhAllocatePage(COPY_BUFFER_SIZE, NULL);

            if (!buffer)
            {
                wprintf(L"Error allocating buffer.\n");
                return 1;
            }

            while (bytesToCopy)
            {
                status = NtReadFile(
                    fileHandle,
                    NULL,
                    NULL,
                    NULL,
                    &isb,
                    buffer,
                    bytesToCopy >= COPY_BUFFER_SIZE ? COPY_BUFFER_SIZE : (ULONG)bytesToCopy,
                    NULL,
                    NULL
                    );

                if (status == STATUS_END_OF_FILE)
                {
                    break;
                }
                else if (!NT_SUCCESS(status))
                {
                    wprintf(L"Error reading from file: %s\n", PhGetNtMessage(status)->Buffer);
                    break;
                }

                status = NtWriteFile(
                    outFileHandle,
                    NULL,
                    NULL,
                    NULL,
                    &isb,
                    buffer,
                    (ULONG)isb.Information, // number of bytes read
                    NULL,
                    NULL
                    );

                if (!NT_SUCCESS(status))
                {
                    wprintf(L"Error writing to output file: %s\n", PhGetNtMessage(status)->Buffer);
                    break;
                }

                bytesToCopy -= (ULONG)isb.Information;
            }

            PhFreePage(buffer);

            // Copy basic attributes over.
            if (NT_SUCCESS(NtQueryInformationFile(
                fileHandle,
                &isb,
                &basicInfo,
                sizeof(FILE_BASIC_INFORMATION),
                FileBasicInformation
                )))
            {
                NtSetInformationFile(
                    outFileHandle,
                    &isb,
                    &basicInfo,
                    sizeof(FILE_BASIC_INFORMATION),
                    FileBasicInformation
                    );
            }

            NtClose(fileHandle);
            NtClose(outFileHandle);
        }
    }
    else if (PhEqualString2(FiArgAction, L"dir", TRUE))
    {
        HANDLE fileHandle;
        UNICODE_STRING pattern;
        PPH_STRING totalSize, totalAllocSize;

        if (FiCreateFile(
            &fileHandle,
            FILE_LIST_DIRECTORY | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            FipDirFileCount = 0;
            FipDirDirCount = 0;
            FipDirTotalSize = 0;
            FipDirTotalAllocSize = 0;

            if (FiArgPattern)
                PhStringRefToUnicodeString(&FiArgPattern->sr, &pattern);

            PhEnumDirectoryFile(
                fileHandle,
                FiArgPattern ? &pattern : NULL,
                FipEnumDirectoryFileForDir,
                NULL
                );
            NtClose(fileHandle);

            totalSize = PhFormatUInt64(FipDirTotalSize, TRUE);
            totalAllocSize = PhFormatUInt64(FipDirTotalAllocSize, TRUE);

            wprintf(
                L"%12I64u file(s)  %11s bytes\n"
                L"%12I64u dir(s)   %11s bytes allocated\n",
                FipDirFileCount,
                totalSize->Buffer,
                FipDirDirCount,
                totalAllocSize->Buffer
                );

            PhDereferenceObject(totalSize);
            PhDereferenceObject(totalAllocSize);
        }
    }
    else if (PhEqualString2(FiArgAction, L"streams", TRUE))
    {
        HANDLE fileHandle;
        PVOID streams;
        PFILE_STREAM_INFORMATION stream;

        if (FiCreateFile(
            &fileHandle,
            FILE_READ_ATTRIBUTES | SYNCHRONIZE,
            FiArgFileName,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
            ))
        {
            if (NT_SUCCESS(PhEnumFileStreams(fileHandle, &streams)))
            {
                stream = PH_FIRST_STREAM(streams);

                while (stream)
                {
                    PPH_STRING size, allocationSize;

                    size = PhFormatUInt64(stream->StreamSize.QuadPart, TRUE);
                    allocationSize = PhFormatUInt64(stream->StreamAllocationSize.QuadPart, TRUE);

                    wprintf(
                        L"%11s %11s %.*s\n",
                        size->Buffer,
                        allocationSize->Buffer,
                        stream->StreamNameLength / 2,
                        stream->StreamName
                        );

                    PhDereferenceObject(size);
                    PhDereferenceObject(allocationSize);

                    stream = PH_NEXT_STREAM(stream);
                }
            }

            NtClose(fileHandle);
        }
    }
    else
    {
        wprintf(L"Error: invalid action \"%s\".\n", FiArgAction->Buffer);
        FiPrintHelp();
        return 1;
    }
}
