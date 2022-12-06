#include "toolspanel.h"
#include <string>
#include "wx/valnum.h"

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
    opts = OptionsContainer();
    setOpts(opts);
    shapeBlock->Collapse();
    growBlock->Collapse();
    aspectBlock->Collapse();
}

void ToolsPanel::setBindings() {
    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &ToolsPanel::updateVirtualSize, this);
    Bind(wxEVT_CHECKBOX, &ToolsPanel::growStateChange, this, 
            ict::GROW_CHECK_CB);
    Bind(wxEVT_RADIOBOX, &ToolsPanel::growChoiceChange, this, 
            ict::GROW_SELECTOR_RB);
    Bind(wxEVT_TEXT, &ToolsPanel::widthChange, this, ict::WIDTH_TC);
    Bind(wxEVT_TEXT, &ToolsPanel::heightChange, this, ict::HEIGHT_TC);
    Bind(wxEVT_CHECKBOX, &ToolsPanel::fixRatioChange, this, ict::FIX_RATIO_CB);
    Bind(wxEVT_CHOICE, &ToolsPanel::shapeChange, this, ict::SHAPE_CH);
    Bind(wxEVT_COLOURPICKER_CHANGED, &ToolsPanel::colourChange, this, ict::PICK_COLOUR_BT);
    Bind(wxEVT_FILEPICKER_CHANGED, &ToolsPanel::imageChange, this, ict::PICK_IMG_FP);
    Bind(wxEVT_SCROLL_THUMBRELEASE, &ToolsPanel::blurChange, this, ict::BACK_BLUR_SL);
}

void ToolsPanel::setOpts(const OptionsContainer &oc) {
    wxCommandEvent h(wxEVT_RADIOBOX, growSelector->GetId());
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
    updateGrowBlock();
    event.Skip();
}

void ToolsPanel::growStateChange(wxCommandEvent &event) {
    growSelector->Enable(event.IsChecked());
    growChoiceState(event.IsChecked(), opts.growChoice);
    opts.allowGrow = event.IsChecked();
    updateGrowBlock();
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
    createButtons();
}

void ToolsPanel::createButtons() {
    apply = new wxButton(this, ict::APPLY_BT, "Apply");
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
    fixRatio = new wxCheckBox(winAspect, ict::FIX_RATIO_CB, "Fix ratio");

    widthSizer = new wxBoxSizer(wxHORIZONTAL);
    widthSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Width:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    widthSizer->AddSpacer(10);
    widthSizer->Add(widthCtrl);

    heightSizer = new wxBoxSizer(wxHORIZONTAL);
    heightSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Height:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    heightSizer->AddSpacer(10);
    heightSizer->Add(heightCtrl);

    aspectSizer = new wxBoxSizer(wxVERTICAL);
    aspectSizer->AddSpacer(5);
    aspectSizer->Add(widthSizer);
    aspectSizer->AddSpacer(5);
    aspectSizer->Add(heightSizer);
    aspectSizer->AddSpacer(5);
    aspectSizer->Add(fixRatio);
    winAspect->SetSizerAndFit(aspectSizer);
}

void ToolsPanel::createGrowBlock() {
    growBlock = new wxCollapsiblePane(this, ict::GROW, "Grow", 
            wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
    wxWindow *winGrow = growBlock->GetPane();

    initGrowChoices();
    growCheck = new wxCheckBox(winGrow, ict::GROW_CHECK_CB, "Allow growing");
    growSelector = new wxRadioBox(winGrow, ict::GROW_SELECTOR_RB, 
            "Options", wxDefaultPosition, wxDefaultSize, 
            wxArrayString(ict::GROW_CHOICE_SIZE, growChoices));
    growSelector->Enable(false);
    colorPicker = new wxColourPickerCtrl(winGrow, ict::PICK_COLOUR_BT, 
            *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_USE_TEXTCTRL);
    colorPicker->Show(false);
    imagePicker = new wxFilePickerCtrl(winGrow, ict::PICK_IMG_FP);
    imagePicker->Show(false);
    backBlur = new wxSlider(winGrow, ict::BACK_BLUR_SL, 0, 0, 100);
    backBlur->Show(false);

    growSizer = new wxBoxSizer(wxVERTICAL);
    growSizer->Add(growCheck);
    growSizer->Add(growSelector);
    growSizer->Add(colorPicker);
    growSizer->Add(imagePicker);
    growSizer->Add(backBlur);
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
    
    shapeSizer = new wxBoxSizer(wxVERTICAL);
    shapeSizer->Add(shapeSelector);
    winShape->SetSizerAndFit(shapeSizer);
}

void ToolsPanel::updateGrowBlock() {
    if(growBlock->IsCollapsed()) return;
    growBlock->Collapse();
    growBlock->Expand();
}

void ToolsPanel::growChoiceState(bool state, int choice) {
    switch (choice) {
    case ict::COLOR:
        colorPicker->Show(state);
        break;
    case ict::IMAGE:
        imagePicker->Show(state);
        backBlur->Show(state);
        break;
    }
}

void ToolsPanel::initGrowChoices() {
    growChoices[ict::COLOR] = wxString("Color");
    growChoices[ict::IMAGE] = wxString("Image");
    opts.growChoice = 0;
}

void ToolsPanel::initShapeChoices() {
    shapeChoices[ict::SQUARE] = wxString("Square");
    shapeChoices[ict::CIRCLE] = wxString("Circle");
    shapeChoices[ict::TRIANGLE] = wxString("Triangle");
}

void ToolsPanel::overlayTools() {
    toolsSizer = new wxBoxSizer(wxVERTICAL);
    toolsSizer->Add(aspectBlock, 0, wxGROW | wxALL, 5);
    toolsSizer->Add(shapeBlock, 0, wxGROW | wxALL, 5);
    toolsSizer->Add(growBlock, 0, wxGROW | wxALL, 5);
    toolsSizer->AddSpacer(10);
    toolsSizer->Add(apply, 0, wxALIGN_CENTER);
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
