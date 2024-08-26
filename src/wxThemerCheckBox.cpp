#include "wxThemer/wxThemerCheckBox.h"

wxThemerCheckBox::wxThemerCheckBox(wxWindow *parent, wxWindowID id, const wxString &label, const wxPoint &pos, const wxSize &size, long style)
    : wxCheckBox(parent, id, label, pos, size, style)
{
}

void wxThemerCheckBox::ApplyTheme(const wxThemer &themer)
{
    SetForegroundColour(themer.GetColor("foregroundColor"));
    SetBackgroundColour(themer.GetColor("backgroundColor"));
    // Apply accent color if desired
    // SetBackgroundColour(themer.GetAccentColor());
    Refresh();
}
