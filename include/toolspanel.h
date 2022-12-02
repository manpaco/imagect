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
    wxRect cropArea;
    bool fixRatio;
    int shapeChoice;
    unsigned int strokeWidth;
    wxColour strokeColour;
    int strokeOff;
    bool allowGrow;
    int growChoice;
    wxColour backColour;
    wxString backImage;
    int backBlur;

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
        OptionsContainer currentStatus() const;
        ~ToolsPanel();

    private:
        void widthChange(wxCommandEvent &event);
        void heightChange(wxCommandEvent &event);
        void fixRatioChange(wxCommandEvent &event);
        void shapeChange(wxCommandEvent &event);
        void colourChange(wxColourPickerEvent &event);
        void imageChange(wxFileDirPickerEvent &event);
        void blurChange(wxScrollEvent &event);
        void growChoiceChange(wxCommandEvent &event);
        void growStateChange(wxCommandEvent &event);
        void updateVirtualSize(wxCollapsiblePaneEvent &event);

        void setBindings();
        void createTools();
        void overlayTools();
        void createAspectBlock();
        void createGrowBlock();
        void createShapeBlock();
        void initGrowChoices();
        void initShapeChoices();
        void growChoiceState(bool state, int choice);
        void updateGrowBlock();
        
        wxCollapsiblePane *aspectBlock;
        wxTextCtrl *widthCtrl;
        wxTextCtrl *heightCtrl;
        wxCheckBox *fixRatio;
        wxBoxSizer *aspectSizer;
        wxBoxSizer *widthSizer;
        wxBoxSizer *heightSizer;

        wxCollapsiblePane *growBlock;
        wxCheckBox *growCheck;
        wxRadioBox *growSelector;
        wxSlider *backBlur;
        wxFilePickerCtrl *imagePicker;
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
        OptionsContainer status;

};

#endif // TOOLSPANEL_H
