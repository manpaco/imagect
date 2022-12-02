#include "toolspanel.h"
#include <string>
#include "wx/valnum.h"

ToolsPanel::ToolsPanel(wxWindow *parent, wxWindowID id) {
    Create(parent, id);
    createTools();
    overlayTools();
    setBindings();
}

ToolsPanel::~ToolsPanel() {

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

void ToolsPanel::widthChange(wxCommandEvent &event) {
    status.cropSize.SetWidth(std::stoi(event.GetString().ToStdString()));
}

void ToolsPanel::heightChange(wxCommandEvent &event) {
    status.cropSize.SetHeight(std::stoi(event.GetString().ToStdString()));
}

void ToolsPanel::fixRatioChange(wxCommandEvent &event) {
    status.fixRatio = event.IsChecked();
}

void ToolsPanel::shapeChange(wxCommandEvent &event) {
    status.shapeChoice = event.GetInt();
}

void ToolsPanel::colourChange(wxColourPickerEvent &event) {
    status.backColour = event.GetColour();
}

void ToolsPanel::imageChange(wxFileDirPickerEvent &event) {
    status.backImage = event.GetPath();
}

void ToolsPanel::blurChange(wxScrollEvent &event) {
    status.backBlur = event.GetPosition();
}

void ToolsPanel::growChoiceChange(wxCommandEvent &event) {
    if(showedGrowChoice == event.GetInt()) return;
    growChoiceState(false, showedGrowChoice);
    growChoiceState(true, event.GetInt());
    status.growChoice = event.GetInt();
    showedGrowChoice = event.GetInt();
    updateGrowBlock();
}

void ToolsPanel::growStateChange(wxCommandEvent &event) {
    growSelector->Enable(event.IsChecked());
    growChoiceState(event.IsChecked(), showedGrowChoice);
    status.allowGrow = event.IsChecked();
    updateGrowBlock();
}

void ToolsPanel::updateVirtualSize(wxCollapsiblePaneEvent &event) {
    FitInside();
    event.Skip();
}

void ToolsPanel::createTools() {
    createAspectBlock();
    createGrowBlock();
    createShapeBlock();
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
    showedGrowChoice = 0;
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
}

void ToolsPanel::heightCrop(unsigned int height) {
    heightCtrl->ChangeValue(wxString(std::to_string(height)));
}

OptionsContainer ToolsPanel::currentStatus() const {
    return status;
}
