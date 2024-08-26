#include "wxThemer/wxThemer.h"
#include <wx/wx.h>
#include <iostream>

#ifdef __WXMSW__
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

wxThemer::wxThemer()
{
    DetectOS();
    DetectOSThemeValues();
}

wxThemer::~wxThemer() {}

// operator overloads
// wxThemer =
wxThemer &wxThemer::operator=(const wxString &themeName)
{
    m_themeName = themeName;
    SearchForTheme(themeName); // Switch to the theme using the provided name
    return *this;
}
// wxThemer::Theme <<
std::ostream &operator<<(std::ostream &os, const wxThemer::Theme &theme)
{
    os << theme.themeName << '\n'
       << "Accent Color: " << theme.accentColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Background Color: " << theme.backgroundColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Button Color: " << theme.buttonColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Button Text Color: " << theme.buttonTextColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Foreground Color: " << theme.foregroundColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Mid Color: " << theme.midColor.GetAsString(wxC2S_CSS_SYNTAX) << '\n'
       << "Text Color: " << theme.textColor.GetAsString(wxC2S_CSS_SYNTAX) << std::endl;
    return os;
}

bool wxThemer::SearchForTheme(const wxString &themeName)
{
    // dev-note: you cannot *apply* the theme in this function because you do not know which object to apply
    //           the theme to.
    //           this function is solely for searching for themeName and if found, setting the m_ActiveTheme
    //           to the Theme object with the matching themeName.

    // Search all available Theme objects for one with themeName for its name
    auto it = std::find_if(m_themes.begin(), m_themes.end(),
                           [&themeName](const Theme &theme)
                           {
                               return theme.themeName == themeName;
                           });

    if (it != m_themes.end())
    {
        // Theme found, set it as the active theme
        m_ActiveTheme = *it;
        std::cout << "Switching to: "
                  << m_ActiveTheme << std::endl;
        return true;
    }
    // Theme not found, keep the current theme
    return false;
}

bool wxThemer::ApplyThemeToAllRecursively(wxObject *object)
{
    bool success = false;
    if (object == nullptr)
        return success;

    // Check if the object is a wxWindow (as wxWindow has color-related methods)
    wxWindow *widget = dynamic_cast<wxWindow *>(object);
    if (widget)
    {
        ApplyThemeToObject(widget, false); // apply theme settings to object, but don't allow refresh of object
        // Recurse over the children
        for (auto child : widget->GetChildren())
        {
            ApplyThemeToAllRecursively(child);
        }
        success = true;
        widget->Refresh();
    }
    return success;
}

bool wxThemer::ApplyThemeToObject(wxObject *object, bool refresh)
{
    bool success = false;
    // Check if the object is a wxWindow (as wxWindow has color-related methods)
    wxWindow *window = dynamic_cast<wxWindow *>(object);
    if (window)
    {
        // Apply the theme colors using standard wxWindow properties
        window->SetForegroundColour(m_ActiveTheme.foregroundColor);
        window->SetBackgroundColour(m_ActiveTheme.backgroundColor);

        // dev-note: before adding custom handling here, ensure the object type can't be themed using standard
        //           setbackgroundcolour() etc. functions first!

        // Set additional colors for specific types of wxWindow derived objects, like text color for wxButton, wxStaticText, etc.
        // wxButton *button = dynamic_cast<wxButton *>(window);
        // if (button)
        // {
        //     // button needs SetOwn for text colour to change
        //     button->SetForegroundColour(m_ActiveTheme.buttonTextColor);
        //     button->SetBackgroundColour(m_ActiveTheme.buttonColor);
        // }
        // Add additional handling for other widget types here

        // Refresh the window to apply changes
        if (refresh)
        {
            window->Refresh();
        }
        success = true;
    }
    return success;
}

void wxThemer::ApplyThemeToOSControlledAreas(wxWindow *window)
{
    if (m_osType == "Windows")
    {
        HWND hwnd = reinterpret_cast<HWND>(window->GetHWND());
        if (hwnd)
        {
            // Use the correct attribute value based on the Windows version
            const int DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19; // Version 1809 - 1909
            const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;             // Version 2004 and later
            int WINOS_DARKMODE_VAL = 20;                              // Default to modern value

            // Initialize the OSVERSIONINFOEXW structure
            OSVERSIONINFOEXW osvi = {sizeof(osvi), 10, 0, 18362};
            DWORDLONG const dwlConditionMask = VerSetConditionMask(
                VerSetConditionMask(
                    VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL),
                    VER_MINORVERSION, VER_GREATER_EQUAL),
                VER_BUILDNUMBER, VER_GREATER_EQUAL);

            if (!VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask))
            {
                WINOS_DARKMODE_VAL = DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1;
            }

            std::cout << "Trying to change OS controlled areas theme with attribute value: " << WINOS_DARKMODE_VAL << std::endl;

            BOOL useDarkMode = m_isDarkMode ? TRUE : FALSE;
            HRESULT hr = DwmSetWindowAttribute(hwnd, WINOS_DARKMODE_VAL, &useDarkMode, sizeof(useDarkMode));

            if (FAILED(hr))
            {
                std::cout << "Failed to set dark mode for the title bar. HRESULT: " << std::hex << hr << std::endl;
            }
            // Force a redraw
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            RedrawWindow(hwnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
            std::cout << "HWND: " << hwnd << std::endl;
            std::cout << "Window Class: " << window->GetClassInfo()->GetClassName() << std::endl;
            std::cout << "Window Label: " << window->GetLabel().ToStdString() << std::endl;
            if (dynamic_cast<wxFrame *>(window))
            {
                std::cout << "The window is of type wxFrame." << std::endl;
            }
            else if (dynamic_cast<wxDialog *>(window))
            {
                std::cout << "The window is of type wxDialog." << std::endl;
            }
            else
            {
                std::cout << "The window is of an unknown type." << std::endl;
            }
        }
    }
}

// void wxThemer::ApplyThemeToOSControlledAreas(wxWindow *window)
// {
//     if (m_isDarkMode)
//     {
//         // Apply dark mode to the title bar (Windows 10+)
//         // To apply dark mode colors to the title bar and borders
//         HWND hwnd = reinterpret_cast<HWND>(window->GetHWND()); // Assuming you have a GetHWND() method or similar

//         if (hwnd)
//         {
//             // Set the window frame color
//             COLORREF darkColor = RGB(30, 30, 30); // Example dark color

//             // Windows 10+ API for custom window color
//             DWM_COLORIZATION_PARAMS colorParams;
//             colorParams.Color = darkColor;
//             colorParams.Color = (colorParams.Color & 0xFFFFFF) | 0xFF000000; // ARGB format
//             DwmSetWindowAttribute(hwnd, DWMWA_COLORIZATION_COLOR, &colorParams, sizeof(colorParams));
//         }
//     }
//     else
//     {
//         // Apply light mode to the title bar (Windows 10+)
//         HWND hwnd = reinterpret_cast<HWND>(window->GetHWND()); // Assuming you have a GetHWND() method or similar

//         if (hwnd)
//         {
//             // Set the window frame color
//             COLORREF lightColor = RGB(255, 255, 255); // Example light color

//             // Windows 10+ API for custom window color
//             DWM_COLORIZATION_PARAMS colorParams;
//             colorParams.Color = lightColor;
//             colorParams.Color = (colorParams.Color & 0xFFFFFF) | 0xFF000000; // ARGB format
//             DwmSetWindowAttribute(hwnd, DWMWA_COLORIZATION_COLOR, &colorParams, sizeof(colorParams));
//         }
//     }
// }

void wxThemer::DetectOS()
{
    // Platform-specific code to detect OS type
#ifdef __WXMSW__
    m_osType = "Windows";
#elif __WXMAC__
    m_osType = "macOS";
#elif __WXGTK__
    m_osType = "Linux";
#else
    m_osType = "Unknown";
#endif
}

wxString wxThemer::GetOSType() const
{
    return m_osType;
}

void wxThemer::DetectOSThemeValues()
{
    // This function is where you set the member values to whatever the OS theme should look like
    // Implementation will depend on the platform and if the platform is using dark mode or not
    // m_isDarkMode = false; // Example placeholder value

#ifdef _WIN32
    if (m_osType == "Windows")
    {
        // get Light/Dark Mode setting from Windows Registry
        DWORD value = 0;
        DWORD valueSize = sizeof(value);
        HKEY hKey;

        // get WinOS Dark Mode value
        if (RegOpenKeyEx(HKEY_CURRENT_USER,
                         LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
                         0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            if (RegQueryValueEx(hKey, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &valueSize) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
            }
            RegCloseKey(hKey);
        }
        // for WinOS, isDarkMode equals inverse of value in Windows registry (Registry indicates Light Mode)
        m_isDarkMode = !static_cast<bool>(value);
        std::cout << "WinOS Dark Mode [bool]: " << std::boolalpha << m_isDarkMode << std::endl;

        DWORD colorizationColor;
        BOOL opaqueBlend;

        // Retrieve the colorization color
        HRESULT hr = DwmGetColorizationColor(&colorizationColor, &opaqueBlend);
        if (SUCCEEDED(hr))
        {
            // Extract RGB components from ARGB color
            BYTE red = GetRValue(colorizationColor);
            BYTE green = GetGValue(colorizationColor);
            BYTE blue = GetBValue(colorizationColor);

            // Return as wxColour
            m_ActiveTheme.accentColor = wxColour(red, green, blue);
        }
        // else don't change the accentColor value in the active theme

        // set theme to WinOS Dark Mode else set to WinOS Light Mode
        this->ActiveTheme = m_isDarkMode ? this->WinOSDarkDefaultTheme : this->WinOSLightDefaultTheme;
    }
#endif // _WIN32
}

wxColour wxThemer::GetColor(wxString colorName) const
{
    auto it = m_ActiveTheme.colorMap.find(colorName);
    if (it != m_ActiveTheme.colorMap.end())
    {
        return *(it->second);
    }
    else
    {
        // Handle the case where the color name isn't found
        wxLogError("Color name '%s' not found in theme '%s'", colorName, m_ActiveTheme.themeName);
    }
    return wxNullColour; // Or return a default color
}

wxColour wxThemer::GetAccentColor() const
{
    return m_ActiveTheme.accentColor;
}

bool wxThemer::IsDarkMode() const
{
    return m_isDarkMode;
}

bool wxThemer::SetColor(wxString colorName, const wxColour &newColor, wxString themeName)
{
    bool success = false;

    Theme *targetTheme = nullptr;
    if (themeName == "Active Theme")
    {
        targetTheme = &m_ActiveTheme;
    }
    else
    {
        auto it = std::find_if(m_themes.begin(), m_themes.end(),
                               [&themeName](Theme &theme)
                               {
                                   return theme.themeName == themeName;
                               });
        if (it != m_themes.end())
        {
            targetTheme = &(*it);
        }
    }

    if (targetTheme)
    {
        if (colorName == "accentColor")
        {
            targetTheme->accentColor = newColor;
        }
        else if (colorName == "backgroundColor")
        {
            targetTheme->backgroundColor = newColor;
        }
        else if (colorName == "buttonColor")
        {
            targetTheme->buttonColor = newColor;
        }
        else if (colorName == "buttonTextColor")
        {
            targetTheme->buttonTextColor = newColor;
        }
        else if (colorName == "foregroundColor")
        {
            targetTheme->foregroundColor = newColor;
        }
        else if (colorName == "midColor")
        {
            targetTheme->midColor = newColor;
        }
        else if (colorName == "textColor")
        {
            targetTheme->textColor = newColor;
        }
        success = true; // Assume success after setting the color
    }
    return success;
}
