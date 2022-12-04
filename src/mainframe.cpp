#include "mainframe.h"
#include "Magick++.h"
#include "canvaspanel.h"
#include "cropevent.h"
#include "identifiersdef.h"
#include "previewpanel.h"
#include "toolspanel.h"
#include "wx/splitter.h"
#include "imgtools.h"

using Magick::Quantum;

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Image Croping Tool") {
    createMenuBar();
    allocateMem();
    overlayPanels();
    setBindings();
    initVars();
}

void MainFrame::initVars() {
    currentState = history.begin();
}

MainFrame::~MainFrame() {
    if(sourceImg) delete(sourceImg);
    if(lowResImg) delete(lowResImg);
}

void MainFrame::allocateMem() {
    sourceImg = new Magick::Image("/usr/share/icons/Adwaita/512x512/devices/computer.png");
    lowResImg = new Magick::Image(*sourceImg);
    Magick::Geometry newRes(lowResImg->columns() * 0.3, lowResImg->rows() * 0.3);
    lowResImg->zoom(newRes);
    wxBitmap initBitmap = createBitmap(*lowResImg);
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sideSplitter = new wxSplitterWindow(mainSplitter, ict::SIDE_SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS, initBitmap);
    tools = new ToolsPanel(sideSplitter, ict::TOOLS);
    preview = new PreviewPanel(sideSplitter, ict::PREVIEW);
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
    updatePreview(initBitmap);
}

void MainFrame::createMenuBar() {
    topMenuBar = new wxMenuBar;

    mFile = new wxMenu;
    mFile->Append(wxID_OPEN);
    mFile->AppendSeparator();
    mFile->Append(wxID_CLOSE);
    mFile->Append(wxID_EXIT);

    mEdit = new wxMenu;
    mEdit->Append(wxID_UNDO);
    mEdit->Append(wxID_REDO);

    mHelp = new wxMenu;
    mHelp->Append(wxID_HELP);
    mHelp->AppendSeparator();
    mHelp->Append(wxID_ABOUT);

    topMenuBar->Append(mFile, "File");
    topMenuBar->Append(mEdit, "Edit");
    topMenuBar->Append(mHelp, "More");
    
    SetMenuBar(topMenuBar);
}

wxBitmap MainFrame::createBitmap(Magick::Image &img) {
    return wxBitmap(wxImage(img.columns(), img.rows(), 
            extractRgb(img), extractAlpha(img)));
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(tools, preview);
    mainSplitter->SplitVertically(canvas, sideSplitter);
    mainSplitter->SetMinimumPaneSize(150);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::setBindings() {
    canvas->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
    tools->Bind(wxEVT_BUTTON, &MainFrame::saveState, this, ict::APPLY_BT);
    tools->Bind(wxEVT_CHECKBOX, &MainFrame::onFixRatio, this, ict::FIX_RATIO_CB);
    tools->Bind(wxEVT_CHECKBOX, &MainFrame::onAllowGrow, this, ict::GROW_CHECK_CB);
    tools->Bind(wxEVT_BUTTON, &MainFrame::undo, this, ict::UNDO_BT);
    tools->Bind(wxEVT_BUTTON, &MainFrame::redo, this, ict::REDO_BT);
}

void MainFrame::onFixRatio(wxCommandEvent &event) {
    canvas->fixCrop(event.IsChecked());
    event.Skip();
}

void MainFrame::onAllowGrow(wxCommandEvent &event) {
    canvas->allowGrow(event.IsChecked());
    event.Skip();
}

void MainFrame::undo(wxCommandEvent &event) {
    currentState--;
    updateCropGeometry();
    if(currentState == history.begin()) tools->enableUndo(false);
    tools->enableRedo(true);
    tools->setOpts(std::get<1>(*currentState));
    composePreview();
    std::cout << std::distance(history.begin(), currentState) << std::endl;
}

void MainFrame::updateCropGeometry() {
    wxRect g(std::get<0>(*currentState), std::get<1>(*currentState).cropSize);
    canvas->cropGeometry(g);
}

void MainFrame::redo(wxCommandEvent &event) {
    if(currentState == history.begin()) tools->enableUndo(true);
    currentState++;
    updateCropGeometry();
    if(currentState == --history.end()) tools->enableRedo(false);
    tools->setOpts(std::get<1>(*currentState));
    composePreview();
    std::cout << std::distance(history.begin(), currentState) << std::endl;
}

void MainFrame::updateHistory(State toSave) {
    if(currentState != history.end()) history.erase(++currentState, history.end());
    history.push_back(toSave);
    currentState = --history.end();
    tools->enableRedo(false);
    if(currentState == ++history.begin()) tools->enableUndo(true);
    composePreview();
    std::cout << std::distance(history.begin(), currentState) << std::endl;
}

void MainFrame::onCropChange(CropEvent &event) {
    if(tools->cropSize() != event.getSize()) tools->cropSize(event.getSize());
}

Magick::Image MainFrame::composeUsingState(Magick::Image &img) {
    int w = std::get<1>(*currentState).cropSize.GetWidth();
    int h = std::get<1>(*currentState).cropSize.GetHeight();
    int xo = std::get<0>(*currentState).x;
    int yo = std::get<0>(*currentState).y;
    Magick::Geometry crop(w, h, xo, yo);;
    Magick::Image comp(extractArea(crop, img));
    Magick::Image back("/usr/share/desktop-base/homeworld-theme/grub/grub-4x3.png");
    overlap(comp, back, true, true);
    return back;
}

void MainFrame::composePreview() {
    Magick::Image newImg = composeUsingState(*lowResImg);
    wxBitmap newPreview = createBitmap(newImg);
    updatePreview(newPreview);
}

void MainFrame::saveState(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
    OptionsContainer opts = tools->currentOpts();
    canvas->cropSize(opts.cropSize);
    if(tools->cropSize() != opts.cropSize) tools->cropSize(opts.cropSize);
    State toSave = std::make_tuple(canvas->getCropOffset(), opts);
    if(toSave == *currentState) return;
    updateHistory(toSave);
}

void MainFrame::updatePreview(wxBitmap &bm) {
    preview->updatePreview(bm);
}
