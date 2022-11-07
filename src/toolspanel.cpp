#include "toolspanel.h"
#include "defs.h"
#include <string>
#include "wx/valnum.h"

ToolsPanel::ToolsPanel(wxWindow *parent, wxWindowID id) : wxScrolledWindow() {
    Create(parent, id);
    createTools();
    overlayTools();
    setBindings();
}

ToolsPanel::~ToolsPanel() {

}

void ToolsPanel::setBindings() {
    Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &ToolsPanel::updateVirtualSize, this);
}

void ToolsPanel::updateVirtualSize(wxCollapsiblePaneEvent &event) {
    FitInside();
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

    widthRatio = new wxTextCtrl(winAspect, ict::WIDTH_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0,
            wxIntegerValidator<unsigned int>(&width));
    heightRatio = new wxTextCtrl(winAspect, ict::HEIGHT_TC, wxEmptyString, 
            wxDefaultPosition, wxDefaultSize, 0,
            wxIntegerValidator<unsigned int>(&height));
    fixRatio = new wxCheckBox(winAspect, ict::FIX_RATIO_CB, "Fix ratio");

    widthSizer = new wxBoxSizer(wxHORIZONTAL);
    widthSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Width:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    widthSizer->AddSpacer(10);
    widthSizer->Add(widthRatio);

    heightSizer = new wxBoxSizer(wxHORIZONTAL);
    heightSizer->Add(new wxStaticText(winAspect, wxID_ANY, "Height:"), 0, 
            wxALIGN_CENTER_VERTICAL);
    heightSizer->AddSpacer(10);
    heightSizer->Add(heightRatio);

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
    colorPicker = new wxColourPickerCtrl(winGrow, ict::COLOR_PICKER_BT, 
            *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_USE_TEXTCTRL);
    colorPicker->Show(false);
    backPicker = new wxFilePickerCtrl(winGrow, ict::PICK_BACK_FP);
    backPicker->Show(false);
    backBlur = new wxSlider(winGrow, ict::BACK_BLUR_SL, 0, 0, 100);
    backBlur->Show(false);

    growSizer = new wxBoxSizer(wxVERTICAL);
    growSizer->Add(growCheck);
    growSizer->Add(growSelector);
    growSizer->Add(colorPicker);
    growSizer->Add(backPicker);
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
    
    shapeSizer = new wxBoxSizer(wxVERTICAL);
    shapeSizer->Add(shapeSelector);
    winShape->SetSizerAndFit(shapeSizer);
}

void ToolsPanel::growState(bool state) {
    if(growCheck->IsChecked() != state) return;
    growSelector->Enable(state);
    growChoiceState(state, showedGrowChoice);
    updateBlock(ict::GROW);
}

void ToolsPanel::growChoice(int choice) {
    if(showedGrowChoice == choice) return;
    growChoiceState(false, showedGrowChoice);
    growChoiceState(true, choice);
    showedGrowChoice = choice;
    updateBlock(ict::GROW);
}

void ToolsPanel::updateBlock(ict::CollPaneID block) {
    switch (block) {
    case ict::GROW:
        growBlock->Collapse();
        growBlock->Expand();
        break;
    case ict::ASPECT_RATIO:
        aspectBlock->Collapse();
        aspectBlock->Expand();
        break;
    case ict::SHAPE:
        shapeBlock->Collapse();
        shapeBlock->Expand();
        break;
    }
}

void ToolsPanel::growChoiceState(bool state, int choice) {
    switch (choice) {
    case ict::COLOR:
        colorPicker->Show(state);
        break;
    case ict::IMAGE:
        backPicker->Show(state);
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
    toolsSizer->Add(apply);
    toolsSizer->AddSpacer(10);
    SetSizerAndFit(toolsSizer);
    SetScrollRate(5, 5);
}

unsigned int ToolsPanel::widthCrop() const {
    return width;
}

unsigned int ToolsPanel::hegihtCrop() const {
    return height;
}

void ToolsPanel::widthCrop(unsigned int width) {
    widthRatio->ChangeValue(wxString(std::to_string(width)));
}

void ToolsPanel::heightCrop(unsigned int hegight) {
    widthRatio->ChangeValue(wxString(std::to_string(height)));
}

int ToolsPanel::growChoice() const {
    return showedGrowChoice;
}
