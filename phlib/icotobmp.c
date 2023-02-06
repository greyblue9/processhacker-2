#include <phgui.h>
#include <uxtheme.h>

// code from http://msdn.microsoft.com/en-us/library/bb757020.aspx

typedef HPAINTBUFFER (*_BeginBufferedPaint)(
    _In_ HDC hdcTarget,
    _In_ const RECT *prcTarget,
    _In_ BP_BUFFERFORMAT dwFormat,
    _In_ BP_PAINTPARAMS *pPaintParams,
    _Out_ HDC *phdc
    );

typedef HRESULT (*_EndBufferedPaint)(
    _In_ HPAINTBUFFER hBufferedPaint,
    _In_ BOOL fUpdateTarget
    );

typedef HRESULT (*_GetBufferedPaintBits)(
    _In_ HPAINTBUFFER hBufferedPaint,
    _Out_ RGBQUAD **ppbBuffer,
    _Out_ int *pcxRow
    );

static BOOLEAN ImportsInitialized = FALSE;
static _BeginBufferedPaint BeginBufferedPaint_I = NULL;
static _EndBufferedPaint EndBufferedPaint_I = NULL;
static _GetBufferedPaintBits GetBufferedPaintBits_I = NULL;

static HBITMAP PhpCreateBitmap32(
    _In_ HDC hdc,
    _In_ ULONG Width,
    _In_ ULONG Height,
    _Outptr_opt_ PVOID *Bits
    )
{
    BITMAPINFO bitmapInfo;

    memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    bitmapInfo.bmiHeader.biWidth = Width;
    bitmapInfo.bmiHeader.biHeight = Height;
    bitmapInfo.bmiHeader.biBitCount = 32;

    return CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, Bits, NULL, 0);
}

static BOOLEAN PhpHasAlpha(
    _In_ PULONG Argb,
    _In_ ULONG Width,
    _In_ ULONG Height,
    _In_ ULONG RowWidth
    )
{
    ULONG delta;
    ULONG x;
    ULONG y;

    delta = RowWidth - Width;

    for (y = Width; y; y--)
    {
        for (x = Height; x; x--)
        {
            if (*Argb++ & 0xff000000)
                return TRUE;
        }

        Argb += delta;
    }

    return FALSE;
}

static VOID PhpConvertToPArgb32(
    _In_ HDC hdc,
    _Inout_ PULONG Argb,
    _In_ HBITMAP Bitmap,
    _In_ ULONG Width,
    _In_ ULONG Height,
    _In_ ULONG RowWidth
    )
{
    BITMAPINFO bitmapInfo;
    PVOID bits;

    memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    bitmapInfo.bmiHeader.biWidth = Width;
    bitmapInfo.bmiHeader.biHeight = Height;
    bitmapInfo.bmiHeader.biBitCount = 32;

    bits = PhAllocate(Width * sizeof(ULONG) * Height);

    if (GetDIBits(hdc, Bitmap, 0, Height, bits, &bitmapInfo, DIB_RGB_COLORS) == Height)
    {
        PULONG argbMask;
        ULONG delta;
        ULONG x;
        ULONG y;

        argbMask = (PULONG)bits;
        delta = RowWidth - Width;

        for (y = Height; y; y--)
        {
            for (x = Width; x; x--)
            {
                if (*argbMask++)
                {
                    *Argb++ = 0; // transparent
                }
                else
                {
                    *Argb++ |= 0xff000000; // opaque
                }
            }

            Argb += delta;
        }
    }

    PhFree(bits);
}

static VOID PhpConvertToPArgb32IfNeeded(
    _In_ HPAINTBUFFER PaintBuffer,
    _In_ HDC hdc,
    _In_ HICON Icon,
    _In_ ULONG Width,
    _In_ ULONG Height
    )
{
    RGBQUAD *quad;
    ULONG rowWidth;

    if (SUCCEEDED(GetBufferedPaintBits_I(PaintBuffer, &quad, &rowWidth)))
    {
        PULONG argb = (PULONG)quad;

        if (!PhpHasAlpha(argb, Width, Height, rowWidth))
        {
            ICONINFO iconInfo;

            if (GetIconInfo(Icon, &iconInfo))
            {
                if (iconInfo.hbmMask)
                {
                    PhpConvertToPArgb32(hdc, argb, iconInfo.hbmMask, Width, Height, rowWidth);
                }

                DeleteObject(iconInfo.hbmColor);
                DeleteObject(iconInfo.hbmMask);
            }
        }
    }
}

HBITMAP PhIconToBitmap(
    _In_ HICON Icon,
    _In_ ULONG Width,
    _In_ ULONG Height
    )
{
    HBITMAP bitmap;
    RECT iconRectangle;
    HDC screenHdc;
    HDC hdc;
    HBITMAP oldBitmap;
    BLENDFUNCTION blendFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    BP_PAINTPARAMS paintParams = { sizeof(paintParams) };
    HDC bufferHdc;
    HPAINTBUFFER paintBuffer;

    iconRectangle.left = 0;
    iconRectangle.top = 0;
    iconRectangle.right = Width;
    iconRectangle.bottom = Height;

    if (!ImportsInitialized)
    {
        HMODULE uxtheme;

        uxtheme = GetModuleHandle(L"uxtheme.dll");
        BeginBufferedPaint_I = (PVOID)GetProcAddress(uxtheme, "BeginBufferedPaint");
        EndBufferedPaint_I = (PVOID)GetProcAddress(uxtheme, "EndBufferedPaint");
        GetBufferedPaintBits_I = (PVOID)GetProcAddress(uxtheme, "GetBufferedPaintBits");
        ImportsInitialized = TRUE;
    }

    if (!BeginBufferedPaint_I || !EndBufferedPaint_I || !GetBufferedPaintBits_I)
    {
        // Probably XP.

        screenHdc = GetDC(NULL);
        hdc = CreateCompatibleDC(screenHdc);
        bitmap = CreateCompatibleBitmap(screenHdc, Width, Height);
        ReleaseDC(NULL, screenHdc);

        oldBitmap = SelectObject(hdc, bitmap);
        FillRect(hdc, &iconRectangle, (HBRUSH)(COLOR_WINDOW + 1));
        DrawIconEx(hdc, 0, 0, Icon, Width, Height, 0, NULL, DI_NORMAL);
        SelectObject(hdc, oldBitmap);

        DeleteDC(hdc);

        return bitmap;
    }

    screenHdc = GetDC(NULL);
    hdc = CreateCompatibleDC(screenHdc);
    bitmap = PhpCreateBitmap32(screenHdc, Width, Height, NULL);
    ReleaseDC(NULL, screenHdc);
    oldBitmap = SelectObject(hdc, bitmap);

    paintParams.dwFlags = BPPF_ERASE;
    paintParams.pBlendFunction = &blendFunction;

    paintBuffer = BeginBufferedPaint_I(hdc, &iconRectangle, BPBF_DIB, &paintParams, &bufferHdc);
    DrawIconEx(bufferHdc, 0, 0, Icon, Width, Height, 0, NULL, DI_NORMAL);
    // If the icon did not have an alpha channel, we need to convert the buffer to PARGB.
    PhpConvertToPArgb32IfNeeded(paintBuffer, hdc, Icon, Width, Height);
    // This will write the buffer contents to the destination bitmap.
    EndBufferedPaint_I(paintBuffer, TRUE);

    SelectObject(hdc, oldBitmap);
    DeleteDC(hdc);

    return bitmap;
}
