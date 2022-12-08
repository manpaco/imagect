#include "toolspanel.h"
#include <string>
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
    backBlur->SetValue(opts.backBlur);
    imagePicker->SetPath(opts.backImage);
    colorPicker->SetColour(opts.backColour);
    growSelector->SetSelection(opts.growChoice);
    growCheck->SetValue(opts.allowGrow);
}

void ToolsPanel::cropSize(const wxSize &s) {
    widthCrop(s.GetWidth());
    heightCrop(s.GetHeight());
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
            "Aspect ratio", wxDefaultPosition, wxDefaultSize, 
            wxCP_NO_TLW_RESIZE);
    wxWindow *winAspect = aspectBlock->GetPane();

    widthCtrl = new wxTextCtrl(winAspect, ict::WIDTH_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0,
            wxIntegerValidator<unsigned int>());
    heightCtrl = new wxTextCtrl(winAspect, ict::HEIGHT_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0,
            wxIntegerValidator<unsigned int>());
    fixRatio = new UnfocusedCheckBox(winAspect, ict::FIX_RATIO_CB, "Fix ratio");

    wxBoxSizer *widthSizer = new wxBoxSizer(wxHORIZONTAL);
    widthSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Width:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    widthSizer->AddSpacer(bestSpace * 2);
    widthSizer->Add(widthCtrl);

    wxBoxSizer *heightSizer = new wxBoxSizer(wxHORIZONTAL);
    heightSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Height:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    heightSizer->AddSpacer(bestSpace * 2);
    heightSizer->Add(heightCtrl);

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
    growCheck = new UnfocusedCheckBox(winGrow, ict::GROW_CHECK_CB, "Allow growing");
    growSelector = new wxChoice(winGrow, ict::GROW_SELECTOR_CH, 
            wxDefaultPosition, wxDefaultSize, 
            wxArrayString(ict::GROW_CHOICE_SIZE, growChoices));
    growSelector->Enable(false);
    colorPicker = new wxColourPickerCtrl(winGrow, ict::PICK_COLOUR_BT, 
            *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
    colorPicker->Enable(false);
    imagePicker = new wxFilePickerCtrl(winGrow, ict::PICK_IMG_FP);
    imagePicker->Enable(false);
    backBlur = new wxSlider(winGrow, ict::BACK_BLUR_SL, 0, 0, 100);
    backBlur->Enable(false);

    wxBoxSizer *growSizer = new wxBoxSizer(wxVERTICAL);
    growSizer->AddSpacer(bestSpace);
    growSizer->Add(growCheck);
    growSizer->AddSpacer(bestSpace);
    growSizer->Add(growSelector, 0, wxALIGN_CENTER);
    growSizer->AddSpacer(bestSpace);
    wxStaticLine *fl = new wxStaticLine(winGrow);
    growSizer->Add(fl, 0, wxEXPAND);
    growSizer->AddSpacer(bestSpace);
    colorText = new wxStaticText(winGrow, wxID_ANY, "Color :");
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
    imageText = new wxStaticText(winGrow, wxID_ANY, "Image :");
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
    
    wxBoxSizer *shapeSizer = new wxBoxSizer(wxVERTICAL);
    shapeSizer->AddSpacer(bestSpace);
    shapeSizer->Add(shapeSelector, 0, wxALIGN_CENTER);
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
