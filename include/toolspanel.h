#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include "defs.h"
#include <wx/wxprec.h>
#include <wx/collpane.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if wxUSE_COLOURPICKERCTRL
    #include <wx/clrpicker.h>
#endif

class ToolsPanel: public wxScrolledWindow {
    public:
        ToolsPanel(wxWindow *parent, wxWindowID id);
        unsigned int widthCrop() const;
        unsigned int hegihtCrop() const;
        void widthCrop(unsigned int width);
        void heightCrop(unsigned int height);
        void growState(bool state);
        void growChoice(int choice);
        int growChoice() const;
        ~ToolsPanel();

    private:
        void setBindings();
        void createTools();
        void overlayTools();
        void updateVirtualSize(wxCollapsiblePaneEvent &event);
        void showPresetRatioTools(wxCommandEvent &event);
        void createAspectBlock();
        void createGrowBlock();
        void initGrowChoices();
        void growChoiceState(bool state, int choice);
        void updateBlock(ict::CollPaneID block);
        
        wxCollapsiblePane *aspectBlock;
        wxTextCtrl *widthRatio;
        wxTextCtrl *heightRatio;
        wxCheckBox *fixRatio;
        wxBoxSizer *aspectSizer;
        wxBoxSizer *widthSizer;
        wxBoxSizer *heightSizer;

        wxCollapsiblePane *growBlock;
        wxCheckBox *growCheck;
        wxRadioBox *growSelector;
        wxSlider *backBlur;
        wxColourPickerCtrl *colorPicker;
        wxBoxSizer *growSizer;
        wxString growChoices[ict::GROW_CHOICE_SIZE];
        int showedGrowChoice;

        wxCollapsiblePane *shapeBlock;
        wxBoxSizer *shapeSizer;

        wxButton *showPreview;
        wxBoxSizer *toolsSizer;

        unsigned int width, height;

};

#endif // TOOLSPANEL_H
