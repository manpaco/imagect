#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

class wxColourPickerEvent;
class wxFileDirPickerEvent;
class wxChoice;
class wxTextCtrl;
class wxSlider;
class wxFilePickerCtrl;
class wxColourPickerCtrl;
class wxStaticText;

#include "wx/scrolwin.h"
#include "wx/checkbox.h"
#include "wx/valnum.h"

#if wxUSE_COLLPANE
    #include <wx/collpane.h>
#endif

#include "optscontainer.h"
#include "defs.h"

/**
 * Implementation of a tools panel.
 *
 * Offer options for differents topics:
 * Aspect: Visual aspect of the crop area.
 * Shape: Border and crop shape.
 * Growing: Background of the overflow and/or transparent area.
 * Validation of option values.
 */
class ToolsPanel: public wxScrolledCanvas {
    public:
        /**
         * Ctor.
         */
        ToolsPanel(wxWindow *parent, wxWindowID id);

        /**
         * Get the crop width.
         */
        unsigned int widthCrop() const;

        /**
         * Get the crop height.
         */
        unsigned int hegihtCrop() const;

        void widthCrop(unsigned int width);
        void heightCrop(unsigned int height);
        void cropSize(const wxSize &s);
        void cropGeometry(const wxRect &r);

        /**
         * Get the crop width and height contained in a wxSize.
         */
        wxSize cropSize() const;

        /**
         * If invalid options are found, then change them to default or minimum
         * values.
         *
         * @return true if the background image can be opened, else false.
         */
        bool valid();

        /**
         * Set the stroke width to be drawn.
         */
        void strokeWidth(unsigned int sw);

        /**
         * Get the current options in a container.
         */
        OptionsContainer currentOpts() const;

        /**
         * Set all options with the given container.
         */
        void setOpts(const OptionsContainer &oc);

        /**
         * Reset the options to default values.
         * Events are emited.
         *
         * @param enableOp Enable or disable the panel.
         */
        void clear(bool enableOp);

        /**
         * Collapse all blocks.
         */
        void collapseBlocks();

        /**
         * Dtor.
         */
        ~ToolsPanel();

    private:
        // ------------------------ EVENT HANDLERS ----------------------------
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
        // ---------------------- END EVENT HANDLERS --------------------------

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
        
        /**
         * Generic CheckBox but with unfocused behaviour.
         */
        class UnfocusedCheckBox : public wxCheckBox {
            public:
                UnfocusedCheckBox(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString &label) {
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
        wxIntegerValidator<unsigned int> wVal;
        wxIntegerValidator<unsigned int> hVal;
        wxIntegerValidator<unsigned int> swVal;

};

#endif // TOOLSPANEL_H
