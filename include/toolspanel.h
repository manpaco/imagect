#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if wxUSE_COLOURPICKERCTRL
    #include <wx/clrpicker.h>
#endif
#if wxUSE_FILEPICKERCTRL
    #include <wx/filepicker.h>
#endif
#if wxUSE_COLLPANE
    #include <wx/collpane.h>
#endif

#include "identifiersdef.h"

struct OptionsContainer {
    unsigned int width, height;
    bool fixRatio;
    ict::ShapeChoice shape;
    unsigned int strokeWidth;
    wxColour strokeColour;
    int strokePos;
    bool allowGrow;
    ict::GrowChoice growChoice;
    wxColour backColour;
    wxString backImage;

    //bool operator==(const OptionsContainer &oc) const {
    //    return width == oc.width &&
    //        height == oc.height &&
    //        fixRatio == oc.fixRatio &&
    //        shape == oc.shape &&
    //        strokeWidth == oc.strokeWidth &&
    //        strokeColour == oc.strokeColour &&
    //        strokePos == oc.strokePos &&
    //        allowGrow == oc.allowGrow &&
    //        growChoice == oc.growChoice &&
    //        backColour == oc.backColour &&
    //        backImage == oc.backImage;
    //}
};

class ToolsPanel: public wxScrolledCanvas {
    public:
        ToolsPanel(wxWindow *parent, wxWindowID id);
        unsigned int widthCrop() const;
        unsigned int hegihtCrop() const;
        void widthCrop(unsigned int width);
        void heightCrop(unsigned int height);
        int growChoice() const;
        OptionsContainer currentStatus() const;
        ~ToolsPanel();

    private:
        void setBindings();
        void createTools();
        void overlayTools();
        void updateVirtualSize(wxCollapsiblePaneEvent &event);
        void showPresetRatioTools(wxCommandEvent &event);
        void createAspectBlock();
        void createGrowBlock();
        void createShapeBlock();
        void initGrowChoices();
        void initShapeChoices();
        void growChoiceState(bool state, int choice);
        void growChoice(int choice);
        void growState(bool state);
        void updateGrowBlock();
        
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
        wxFilePickerCtrl *backPicker;
        wxColourPickerCtrl *colorPicker;
        wxBoxSizer *growSizer;
        wxString growChoices[ict::GROW_CHOICE_SIZE];
        int showedGrowChoice;

        wxCollapsiblePane *shapeBlock;
        wxBoxSizer *shapeSizer;
        wxChoice *shapeSelector;
        wxString shapeChoices[ict::SHAPE_CHOICE_SIZE];

        wxButton *apply;
        wxBoxSizer *toolsSizer;

        unsigned int width, height;
        OptionsContainer status;

};

#endif // TOOLSPANEL_H
