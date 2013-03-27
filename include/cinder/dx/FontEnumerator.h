#pragma once
#include <wrl.h>
#include <dwrite.h>
#include <strsafe.h>
#include <assert.h>
#include <time.h>
#include <WinNls.h>
#include <datetimeapi.h>
#include <agile.h>

namespace FontEnumeration
{
    public ref class FontEnumerator sealed
    {
    public:
        Platform::Array<Platform::String^>^ ListSystemFonts();
    };
}