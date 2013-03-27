#include "cinder/dx/FontEnumerator.h"

using namespace FontEnumeration;
using namespace Platform;
using namespace Microsoft::WRL;

Array<String^>^ FontEnumerator::ListSystemFonts()
{
    ComPtr<IDWriteFactory> pDWriteFactory;

    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        &pDWriteFactory
        );

    ComPtr<IDWriteFontCollection> pFontCollection;

    // Get the system font collection.
    if (SUCCEEDED(hr))
    {
        hr = pDWriteFactory->GetSystemFontCollection(&pFontCollection);
    }

    UINT32 familyCount = 0;

    // Get the number of font families in the collection.
    if (SUCCEEDED(hr))
    {
        familyCount = pFontCollection->GetFontFamilyCount();
    }

    Array<String^>^ strings = ref new Array<String^>(familyCount);

    if (strings == nullptr)
    {
        return nullptr;
    }

    uint32 index = 0;
    BOOL exists = false;
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

    // Get the default locale for this user.
    int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

    for (UINT32 i = 0; i < familyCount; ++i)
    {
        ComPtr<IDWriteFontFamily> pFontFamily;

        // Get the font family.
        if (SUCCEEDED(hr))
        {
            hr = pFontCollection->GetFontFamily(i, &pFontFamily);
        }

        ComPtr<IDWriteLocalizedStrings> pFamilyNames;

        // Get a list of localized strings for the family name.
        if (SUCCEEDED(hr))
        {
            hr = pFontFamily->GetFamilyNames(&pFamilyNames);
        }

        if (SUCCEEDED(hr))
        {
            // If the default locale is returned, find that locale name.
            if (defaultLocaleSuccess)
            {
                hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
            }
            if (SUCCEEDED(hr) && !exists) // if the above find did not find a match, retry with US English
            {
                hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
            }
        }

        // If the specified locale doesn't exist, select the first on the list.
        if (!exists)
        {
            index = 0;
        }

        UINT32 length = 0;

        // Get the string length.
        if (SUCCEEDED(hr))
        {
            hr = pFamilyNames->GetStringLength(index, &length);
        }

        // Allocate a string big enough to hold the name.
        wchar_t* name = new (std::nothrow) wchar_t[length+1];
        if (name == nullptr)
        {
            hr = E_OUTOFMEMORY;
        }

        // Get the family name.
        if (SUCCEEDED(hr))
        {
            hr = pFamilyNames->GetString(index, name, length+1);
        }

        // Add the family name to the String Array.
        if (SUCCEEDED(hr))
        {
            strings[i] = ref new String(name);
        }

        delete [] name;
    }
    return strings;
}
