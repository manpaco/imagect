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
    tools->Bind(wxEVT_BUTTON, &MainFrame::saveOpts, this, ict::APPLY_BT);
    canvas->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
}

void MainFrame::undo() {
    currentState--;
    if(currentState == history.begin()) {}
    composePreview();
}

void MainFrame::redo() {
    currentState++;
    std::list<State>::iterator verify(currentState);
    verify++;
    if(verify == history.end()) {}
    composePreview();
}

void MainFrame::saveState(State toSave) {
    std::list<State>::iterator deleteFrom(currentState);
    if(deleteFrom != history.end()) deleteFrom++;
    history.erase(deleteFrom, history.end());
    history.push_back(toSave);
    currentState = history.end();
    currentState--;
    composePreview();
}

void MainFrame::onCropChange(CropEvent &event) {
    // update preview
    tools->cropSize(event.getSize());
    OptionsContainer opts = tools->currentOpts();
    saveState(std::make_tuple(event.getOffset(), opts));
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

void MainFrame::saveOpts(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
    OptionsContainer opts = tools->currentOpts();
    canvas->cropSize(opts.cropSize);
    if(tools->cropSize() != opts.cropSize) tools->cropSize(opts.cropSize);
    if(opts == std::get<1>(*currentState)) return;
    wxPoint off(canvas->getCropOffset());
    saveState(std::make_tuple(off, opts));
}

void MainFrame::updatePreview(wxBitmap &bm) {
    preview->updatePreview(bm);
}
