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

#if wxUSE_STATLINE
    #include <wx/statline.h>
#endif

#include "identifiersdef.h"

struct OptionsContainer {
    wxSize cropSize;
    bool fixRatio;
    float ratio;
    int shapeChoice;
    unsigned int strokeWidth;
    wxColour strokeColour;
    int strokeOff;
    bool allowGrow;
    int growChoice;
    wxColour backColour;
    wxString backImage;
    int backBlur;

    OptionsContainer() : 
        cropSize(0, 0),
        fixRatio(false),
        ratio(0.0),
        shapeChoice(0),
        strokeWidth(1),
        strokeColour(*wxBLACK),
        strokeOff(0),
        allowGrow(false),
        growChoice(0),
        backColour(*wxBLACK),
        backImage(""),
        backBlur(0)
    {

    }

    bool operator==(const OptionsContainer &oc) const {
        return cropSize == oc.cropSize &&
            fixRatio == oc.fixRatio &&
            shapeChoice == oc.shapeChoice &&
            strokeWidth == oc.strokeWidth &&
            strokeColour == oc.strokeColour &&
            strokeOff == oc.strokeOff &&
            allowGrow == oc.allowGrow &&
            growChoice == oc.growChoice &&
            backColour == oc.backColour &&
            backImage == oc.backImage &&
            backBlur == oc.backBlur;
    }
    bool operator!=(const OptionsContainer &oc) const {
        return !(*this == oc);
    }
};

class ToolsPanel: public wxScrolledCanvas {
    public:
        ToolsPanel(wxWindow *parent, wxWindowID id);
        unsigned int widthCrop() const;
        unsigned int hegihtCrop() const;
        void widthCrop(unsigned int width);
        void heightCrop(unsigned int height);
        void cropSize(const wxSize &s);
        wxSize cropSize() const;
        OptionsContainer currentOpts() const;
        void setOpts(const OptionsContainer &oc);
        void clear(bool enableOp);
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
        void strokeColorChange(wxColourPickerEvent &event);
        void strokeWidthChange(wxCommandEvent &event);

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
        
        class UnfocusedCheckBox : public wxCheckBox {
            public:
                UnfocusedCheckBox(wxWindow *parent, wxWindowID id, const wxString &label) {
                    Create(parent, id, label);
                }
                bool AcceptsFocus() const {
                    return false;
                }
        };
        
        wxCollapsiblePane *aspectBlock;
        wxTextCtrl *widthCtrl;
        wxTextCtrl *heightCtrl;
        UnfocusedCheckBox *fixRatio;

        wxCollapsiblePane *growBlock;
        UnfocusedCheckBox *growCheck;
        wxChoice *growSelector;
        wxSlider *backBlur;
        wxFilePickerCtrl *imagePicker;
        wxColourPickerCtrl *colorPicker;
        wxString growChoices[ict::GROW_CHOICE_SIZE];
        wxStaticText *blurText;
        wxStaticText *colorText;
        wxStaticText *imageText;

        wxCollapsiblePane *shapeBlock;
        wxChoice *shapeSelector;
        wxTextCtrl *strokeWidthCtrl;
        wxColourPickerCtrl *strokeColorPicker;
        wxString shapeChoices[ict::SHAPE_CHOICE_SIZE];

        OptionsContainer opts;

};

#endif // TOOLSPANEL_H
