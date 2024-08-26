#ifndef WXTHEMERCHECKBOX_H
#define WXTHEMERCHECKBOX_H

#include "wxThemer.h"

class wxThemerCheckBox : public wxCheckBox
{
public:
    wxThemerCheckBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

    void ApplyTheme(const wxThemer& themer);
};

#endif // WXTHEMERCHECKBOX_H
