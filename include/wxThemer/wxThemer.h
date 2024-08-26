#ifndef WXTHEMER_H
#define WXTHEMER_H

#include <wx/wx.h>
#include <vector>
#include <algorithm>
#include <iostream>

class wxThemer
{
public:
    // data stutures
    struct Theme
    {
        wxString themeName;
        wxColour accentColor,
            backgroundColor,
            buttonColor,
            buttonTextColor,
            foregroundColor,
            midColor,
            textColor;
        std::unordered_map<wxString, wxColour *> colorMap;

        friend std::ostream &operator<<(std::ostream &os, const Theme &theme);

        // Default constructor
        Theme()
            : themeName("Default Theme"),
              accentColor(*wxBLUE),
              backgroundColor(*wxWHITE),
              buttonColor(wxColour(200, 200, 200)),
              buttonTextColor(*wxBLACK),
              foregroundColor(wxColour(100, 100, 100)),
              midColor(wxColour(150, 150, 150)),
              textColor(*wxBLACK)
        {
            // Initialize the map with color names and corresponding pointers
            colorMap["accentColor"] = &accentColor;
            colorMap["backgroundColor"] = &backgroundColor;
            colorMap["buttonColor"] = &buttonColor;
            colorMap["buttonTextColor"] = &buttonTextColor;
            colorMap["foregroundColor"] = &foregroundColor;
            colorMap["midColor"] = &midColor;
            colorMap["textColor"] = &textColor;
        }

        // Parameterized constructor
        Theme(const wxString &name, const wxColour &accent, const wxColour &background, const wxColour &button,
              const wxColour &buttonText, const wxColour &foreground, const wxColour &mid, const wxColour &text)
            : themeName(name), accentColor(accent), backgroundColor(background), buttonColor(button),
              buttonTextColor(buttonText), foregroundColor(foreground), midColor(mid), textColor(text)
        {
            // Initialize the map with color names and corresponding pointers
            colorMap["accentColor"] = &accentColor;
            colorMap["backgroundColor"] = &backgroundColor;
            colorMap["buttonColor"] = &buttonColor;
            colorMap["buttonTextColor"] = &buttonTextColor;
            colorMap["foregroundColor"] = &foregroundColor;
            colorMap["midColor"] = &midColor;
            colorMap["textColor"] = &textColor;
        }
    };

    // default light theme settings will be pulled from wxSystemSettings
    Theme WinOSLightDefaultTheme = {
        "WinOS Light",                                          // theme name
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),    // accent
        wxColor(171, 171, 171),                                 // bg - that pale "windows grey" colour
        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE),      // button bg
        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT),      // button text
        *wxBLACK,                                               // fg - seems to be for thing like text labels
        wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE), // mid
        wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)    // text
    };

    // default dark theme settings are assigned here, but should be tailored to suit on a per-OS basis in class functions
    Theme WinOSDarkDefaultTheme = {
        "WinOS Dark",            // theme name
        *wxBLUE,                 // accent
        wxColour(30, 30, 30),    // bg
        wxColour(50, 50, 50),    // button bg
        wxColour(180, 180, 180), // button text
        wxColour(100, 100, 100), // fg
        wxColour(65, 65, 65),    // mid
        wxColour(220, 220, 220)  // text
    };

    // the current active theme
    Theme ActiveTheme = {
        "Active Theme",          // theme name
        *wxBLUE,                 // accent
        wxColour(30, 30, 30),    // bg
        wxColour(50, 50, 50),    // button bg
        wxColour(180, 180, 180), // button text
        wxColour(100, 100, 100), // fg
        wxColour(65, 65, 65),    // mid
        wxColour(220, 220, 220)  // text
    };

    // Collection of themes
    std::vector<Theme> m_themes = {WinOSLightDefaultTheme, WinOSDarkDefaultTheme};

    // public methods
    wxThemer();
    ~wxThemer();
    wxThemer &operator=(const wxString &themeName);

    void DetectOS();
    void DetectOSThemeValues();

    wxColour GetAccentColor() const;             // returns the active theme's accent colour
    wxColour GetColor(wxString colorName) const; // returns the active theme's colorName colour (if it exists) else returns nullcolour
    wxString GetOSType() const;                  // returns the user OS
    bool IsDarkMode() const;                     // returns if the user OS is expecting dark mode or not

    bool SetColor(wxString colorName, const wxColour &newColor, wxString themeName = "Active Theme"); // set a colour value in a theme

    bool ApplyThemeToObject(wxObject *object, bool refresh = true); // method to colour a valid/handled wx object based on its type
    bool ApplyThemeToAllRecursively(wxObject *object);              // method to colour all valid/handled wx objects in object's heirarchy
    void ApplyThemeToOSControlledAreas(wxWindow *window);           // method to colour OS controlled portions of wxWindow object
    bool SearchForTheme(const wxString &themeName);                 // method to search for Theme with themeName in all available wxThemer theme objects; return true if found, false if not found

    // private members
private:
    wxString m_themeName;
    wxString m_osType;
    mutable bool m_isDarkMode;
    wxColour m_accentColor,
        m_backgroundColor,
        m_buttonColor,
        m_buttonTextColor,
        m_foregroundColor,
        m_midColor,
        m_textColor;
    // program's Active theme (use a copy of a default for the user OS)
    Theme m_ActiveTheme;
};

#endif // WXTHEMER_H
