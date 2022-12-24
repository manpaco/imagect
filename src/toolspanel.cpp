#include "toolspanel.h"
#include <string>
#include "filext.h"
#include "imgtools.h"

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

#include "wx/valnum.h"

extern const int bestSpace;

ToolsPanel::ToolsPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    createTools();
    overlayTools();
    setBindings();
    clear(false);
}

ToolsPanel::~ToolsPanel() {

}

void ToolsPanel::clear(bool enableOp) {
    Enable(enableOp);
    setOpts(OptionsContainer());
    collapseBlocks();
}

void ToolsPanel::collapseBlocks() {
    shapeBlock->Collapse();
    growBlock->Collapse();
    aspectBlock->Collapse();
}

void ToolsPanel::setBindings() {
    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &ToolsPanel::updateVirtualSize, this);
    Bind(wxEVT_CHECKBOX, &ToolsPanel::growStateChange, this, 
            ict::GROW_CHECK_CB);
    Bind(wxEVT_CHOICE, &ToolsPanel::growChoiceChange, this, 
            ict::GROW_SELECTOR_CH);
    Bind(wxEVT_TEXT, &ToolsPanel::widthChange, this, ict::WIDTH_TC);
    Bind(wxEVT_TEXT, &ToolsPanel::heightChange, this, ict::HEIGHT_TC);
    Bind(wxEVT_CHECKBOX, &ToolsPanel::fixRatioChange, this, ict::FIX_RATIO_CB);
    Bind(wxEVT_CHOICE, &ToolsPanel::shapeChange, this, ict::SHAPE_CH);
    Bind(wxEVT_COLOURPICKER_CHANGED, &ToolsPanel::colourChange, this, ict::PICK_COLOUR_BT);
    Bind(wxEVT_FILEPICKER_CHANGED, &ToolsPanel::imageChange, this, ict::PICK_IMG_FP);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &ToolsPanel::blurChange, this, ict::BACK_BLUR_SL);
    Bind(wxEVT_COLOURPICKER_CHANGED, &ToolsPanel::strokeColorChange, this, ict::PICK_STROKE_COLOUR_BT);
    Bind(wxEVT_TEXT, &ToolsPanel::strokeWidthChange, this, ict::STROKE_WIDTH_TC);
}

void ToolsPanel::setOpts(const OptionsContainer &oc) {
    wxCommandEvent h(wxEVT_CHOICE, growSelector->GetId());
    h.SetEventObject(growSelector);
    h.SetInt(oc.growChoice);
    ProcessWindowEvent(h);
    wxCommandEvent f(wxEVT_CHECKBOX, growCheck->GetId());
    f.SetEventObject(growCheck);
    f.SetInt(oc.allowGrow);
    ProcessWindowEvent(f);
    opts = oc;
    cropSize(opts.cropSize);
    fixRatio->SetValue(opts.fixRatio);
    wxCommandEvent e(wxEVT_CHECKBOX, fixRatio->GetId());
    e.SetEventObject(fixRatio);
    e.SetInt(opts.fixRatio);
    ProcessWindowEvent(e);
    shapeSelector->SetSelection(opts.shapeChoice);
    strokeWidthCtrl->SetValue(std::to_string(opts.strokeWidth));
    strokeColorPicker->SetColour(opts.strokeColour);
    backBlur->SetValue(opts.backBlur);
    imagePicker->SetPath(opts.backImage);
    colorPicker->SetColour(opts.backColour);
    growSelector->SetSelection(opts.growChoice);
    growCheck->SetValue(opts.allowGrow);
}

void ToolsPanel::cropGeometry(const wxRect &r) {
    cropSize(r.GetSize());
    opts.cropOff = r.GetPosition();
}

void ToolsPanel::strokeWidthChange(wxCommandEvent &event) {
    std::string text = event.GetString().ToStdString();
    if(text.empty()) return;
    opts.strokeWidth = std::stoi(text);
}

void ToolsPanel::strokeWidth(unsigned int sw) {
    strokeWidthCtrl->SetValue(std::to_string(sw));
    opts.strokeWidth = sw;
}

void ToolsPanel::strokeColorChange(wxColourPickerEvent &event) {
    opts.strokeColour = event.GetColour();
}

void ToolsPanel::cropSize(const wxSize &s) {
    widthCrop(s.GetWidth());
    heightCrop(s.GetHeight());
}

bool ToolsPanel::checkValues() {
    std::string checking = widthCtrl->GetValue().ToStdString();
    if(checking.empty()) widthCrop(1);
    checking = heightCtrl->GetValue().ToStdString();
    if(checking.empty()) heightCrop(1);
    checking = strokeWidthCtrl->GetValue().ToStdString();
    if(checking.empty()) strokeWidth(0);
    if(opts.growChoice == ict::IMAGE) {
        return tryOpen(opts.backImage, "Background image");
    }
    return true;
}

wxSize ToolsPanel::cropSize() const {
    return opts.cropSize;
}

void ToolsPanel::widthChange(wxCommandEvent &event) {
    std::string text = event.GetString().ToStdString();
    if(text.empty()) return;
    opts.cropSize.SetWidth(std::stoi(text));
    if(opts.fixRatio) heightCrop((float)opts.cropSize.GetWidth() / opts.ratio);
}

void ToolsPanel::heightChange(wxCommandEvent &event) {
    std::string text = event.GetString().ToStdString();
    if(text.empty()) return;
    opts.cropSize.SetHeight(std::stoi(text));
    if(opts.fixRatio) widthCrop((float)opts.cropSize.GetHeight() * opts.ratio);
}

void ToolsPanel::fixRatioChange(wxCommandEvent &event) {
    opts.fixRatio = event.IsChecked();
    opts.ratio = (float)opts.cropSize.GetWidth() / (float)opts.cropSize.GetHeight();
}

void ToolsPanel::shapeChange(wxCommandEvent &event) {
    opts.shapeChoice = event.GetInt();
}

void ToolsPanel::colourChange(wxColourPickerEvent &event) {
    opts.backColour = event.GetColour();
}

void ToolsPanel::imageChange(wxFileDirPickerEvent &event) {
    opts.backImage = event.GetPath();
}

void ToolsPanel::blurChange(wxScrollEvent &event) {
    opts.backBlur = event.GetPosition();
}

void ToolsPanel::growChoiceChange(wxCommandEvent &event) {
    if(opts.growChoice == event.GetInt()) return;
    growChoiceState(false, opts.growChoice);
    growChoiceState(true, event.GetInt());
    opts.growChoice = event.GetInt();
    event.Skip();
}

void ToolsPanel::growStateChange(wxCommandEvent &event) {
    growSelector->Enable(event.IsChecked());
    growChoiceState(event.IsChecked(), opts.growChoice);
    opts.allowGrow = event.IsChecked();
    event.Skip();
}

void ToolsPanel::updateVirtualSize(wxCollapsiblePaneEvent &event) {
    FitInside();
    event.Skip();
}

void ToolsPanel::createTools() {
    createAspectBlock();
    createGrowBlock();
    createShapeBlock();
}

void ToolsPanel::createAspectBlock() {
    aspectBlock = new wxCollapsiblePane(this, ict::ASPECT_RATIO, 
            "Aspect", wxDefaultPosition, wxDefaultSize, 
            wxCP_NO_TLW_RESIZE);
    wxWindow *winAspect = aspectBlock->GetPane();

    wxIntegerValidator<unsigned int> validator;
    validator.SetMin(1);

    widthCtrl = new wxTextCtrl(winAspect, ict::WIDTH_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0, validator);
    heightCtrl = new wxTextCtrl(winAspect, ict::HEIGHT_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0, validator);
    fixRatio = new UnfocusedCheckBox(winAspect, ict::FIX_RATIO_CB, "Fix ratio");

    wxBoxSizer *widthSizer = new wxBoxSizer(wxHORIZONTAL);
    widthSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Width:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    widthSizer->AddSpacer(bestSpace);
    widthSizer->Add(widthCtrl);
    widthSizer->AddSpacer(bestSpace);
    widthSizer->Add(new wxStaticText(winAspect, wxID_ANY, "px"), 0, wxALIGN_CENTER);

    wxBoxSizer *heightSizer = new wxBoxSizer(wxHORIZONTAL);
    heightSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Height:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    heightSizer->AddSpacer(bestSpace);
    heightSizer->Add(heightCtrl);
    heightSizer->AddSpacer(bestSpace);
    heightSizer->Add(new wxStaticText(winAspect, wxID_ANY, "px"), 0, wxALIGN_CENTER);

    wxBoxSizer *aspectSizer = new wxBoxSizer(wxVERTICAL);
    aspectSizer->AddSpacer(bestSpace);
    aspectSizer->Add(fixRatio);
    wxStaticLine *fl = new wxStaticLine(winAspect);
    aspectSizer->AddSpacer(bestSpace);
    aspectSizer->Add(fl, 0, wxEXPAND);
    aspectSizer->AddSpacer(bestSpace);
    aspectSizer->Add(widthSizer, 0, wxALIGN_CENTER);
    aspectSizer->AddSpacer(bestSpace);
    aspectSizer->Add(heightSizer, 0, wxALIGN_CENTER);
    aspectSizer->AddSpacer(bestSpace);
    winAspect->SetSizerAndFit(aspectSizer);
}

void ToolsPanel::createGrowBlock() {
    growBlock = new wxCollapsiblePane(this, ict::GROW, "Grow", 
            wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
    wxWindow *winGrow = growBlock->GetPane();

    initGrowChoices();
    growCheck = new UnfocusedCheckBox(winGrow, ict::GROW_CHECK_CB, "Allow growing with:");
    growSelector = new wxChoice(winGrow, ict::GROW_SELECTOR_CH, 
            wxDefaultPosition, wxDefaultSize, 
            wxArrayString(ict::GROW_CHOICE_SIZE, growChoices));
    growSelector->Enable(false);
    colorPicker = new wxColourPickerCtrl(winGrow, ict::PICK_COLOUR_BT, 
            *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
    colorPicker->Enable(false);
    imagePicker = new wxFilePickerCtrl(winGrow, ict::PICK_IMG_FP, 
            wxEmptyString, "Background image", importWc);
    imagePicker->Enable(false);
    backBlur = new wxSlider(winGrow, ict::BACK_BLUR_SL, 0, 0, 100);
    backBlur->Enable(false);

    wxBoxSizer *growSizer = new wxBoxSizer(wxVERTICAL);
    growSizer->AddSpacer(bestSpace);
    wxBoxSizer *hGrowSelecSizer = new wxBoxSizer(wxHORIZONTAL);
    hGrowSelecSizer->Add(growCheck, 0, wxALIGN_CENTER);
    hGrowSelecSizer->AddSpacer(bestSpace);
    hGrowSelecSizer->Add(growSelector, 0, wxALIGN_CENTER);
    growSizer->Add(hGrowSelecSizer);
    wxStaticLine *fl = new wxStaticLine(winGrow);
    growSizer->AddSpacer(bestSpace);
    growSizer->Add(fl, 0, wxEXPAND);
    growSizer->AddSpacer(bestSpace);
    colorText = new wxStaticText(winGrow, wxID_ANY, "Color:");
    colorText->Enable(false);
    wxBoxSizer *hColorSizer = new wxBoxSizer(wxHORIZONTAL);
    hColorSizer->Add(colorText, 0, wxALIGN_CENTER);
    hColorSizer->AddSpacer(bestSpace);
    hColorSizer->Add(colorPicker, 0, wxALIGN_CENTER);
    growSizer->Add(hColorSizer, 0, wxALIGN_CENTER);
    growSizer->AddSpacer(bestSpace);
    wxStaticLine *sl = new wxStaticLine(winGrow);
    growSizer->Add(sl, 0, wxEXPAND);
    growSizer->AddSpacer(bestSpace);
    imageText = new wxStaticText(winGrow, wxID_ANY, "Image:");
    imageText->Enable(false);
    wxBoxSizer *hImageSizer = new wxBoxSizer(wxHORIZONTAL);
    hImageSizer->Add(imageText, 0, wxALIGN_CENTER);
    hImageSizer->AddSpacer(bestSpace);
    hImageSizer->Add(imagePicker);
    growSizer->Add(hImageSizer, 0, wxALIGN_CENTER);
    blurText = new wxStaticText(winGrow, wxID_ANY, "Blur");
    blurText->Enable(false);
    growSizer->AddSpacer(bestSpace);
    growSizer->Add(blurText, 0, wxALIGN_CENTER);
    growSizer->Add(backBlur, 0, wxEXPAND);
    growSizer->AddSpacer(bestSpace);

    winGrow->SetSizerAndFit(growSizer);
}

void ToolsPanel::createShapeBlock() {
    shapeBlock = new wxCollapsiblePane(this, ict::SHAPE, "Shape",
            wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
    wxWindow *winShape = shapeBlock->GetPane();
    initShapeChoices();
    shapeSelector = new wxChoice(winShape, ict::SHAPE_CH, wxDefaultPosition, 
            wxDefaultSize, 
            wxArrayString(ict::SHAPE_CHOICE_SIZE, shapeChoices));
    shapeSelector->SetSelection(0);
    strokeWidthCtrl = new wxTextCtrl(winShape, ict::STROKE_WIDTH_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0,
            wxIntegerValidator<unsigned int>());
    strokeColorPicker = new wxColourPickerCtrl(winShape, ict::PICK_STROKE_COLOUR_BT, 
            *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
    
    wxBoxSizer *shapeSizer = new wxBoxSizer(wxVERTICAL);
    shapeSizer->AddSpacer(bestSpace);
    shapeSizer->Add(shapeSelector, 0, wxALIGN_CENTER);
    wxStaticLine *fl = new wxStaticLine(winShape);
    shapeSizer->AddSpacer(bestSpace);
    shapeSizer->Add(fl, 0, wxEXPAND);
    wxBoxSizer *hStrokeWidthSizer = new wxBoxSizer(wxHORIZONTAL);
    hStrokeWidthSizer->Add(new wxStaticText(winShape, wxID_ANY, "Stroke width:"), 0, wxALIGN_CENTER);
    hStrokeWidthSizer->AddSpacer(bestSpace);
    hStrokeWidthSizer->Add(strokeWidthCtrl);
    hStrokeWidthSizer->AddSpacer(bestSpace);
    hStrokeWidthSizer->Add(new wxStaticText(winShape, wxID_ANY, "px"), 0, wxALIGN_CENTER);
    shapeSizer->AddSpacer(bestSpace);
    shapeSizer->Add(hStrokeWidthSizer, 0, wxALIGN_CENTER);
    wxBoxSizer *hStrokeColorSizer = new wxBoxSizer(wxHORIZONTAL);
    hStrokeColorSizer->Add(new wxStaticText(winShape, wxID_ANY, "Stroke color:"), 0, wxALIGN_CENTER);
    hStrokeColorSizer->AddSpacer(bestSpace);
    hStrokeColorSizer->Add(strokeColorPicker);
    shapeSizer->AddSpacer(bestSpace);
    shapeSizer->Add(hStrokeColorSizer, 0, wxALIGN_CENTER);
    shapeSizer->AddSpacer(bestSpace);
    winShape->SetSizerAndFit(shapeSizer);
}

void ToolsPanel::growChoiceState(bool state, int choice) {
    switch (choice) {
    case ict::COLOR:
        colorText->Enable(state);
        colorPicker->Enable(state);
        break;
    case ict::IMAGE:
        imageText->Enable(state);
        imagePicker->Enable(state);
        blurText->Enable(state);
        backBlur->Enable(state);
        break;
    }
}

void ToolsPanel::initGrowChoices() {
    growChoices[ict::COLOR] = wxString("Color");
    growChoices[ict::IMAGE] = wxString("Image");
    growChoices[ict::VOID] = wxString("Void");
}

void ToolsPanel::initShapeChoices() {
    shapeChoices[ict::SQUARE] = wxString("Square");
    shapeChoices[ict::CIRCLE] = wxString("Circle");
    shapeChoices[ict::TRIANGLE] = wxString("Triangle");
}

void ToolsPanel::overlayTools() {
    wxBoxSizer *toolsSizer = new wxBoxSizer(wxVERTICAL);
    toolsSizer->Add(aspectBlock, 0, wxGROW | wxALL, bestSpace);
    toolsSizer->Add(shapeBlock, 0, wxGROW | wxALL, bestSpace);
    toolsSizer->Add(growBlock, 0, wxGROW | wxALL, bestSpace);
    SetSizerAndFit(toolsSizer);
    SetScrollRate(5, 5);
}

void ToolsPanel::widthCrop(unsigned int width) {
    widthCtrl->ChangeValue(wxString(std::to_string(width)));
    opts.cropSize.SetWidth(width);
}

void ToolsPanel::heightCrop(unsigned int height) {
    heightCtrl->ChangeValue(wxString(std::to_string(height)));
    opts.cropSize.SetHeight(height);
}

OptionsContainer ToolsPanel::currentOpts() const {
    return opts;
}
