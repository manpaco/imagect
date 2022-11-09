#include "mainframe.h"
#include "Magick++.h"
#include "canvaspanel.h"
#include "defs.h"
#include "previewpanel.h"
#include "wx/splitter.h"
#include "imgtools.h"

using Magick::Quantum;

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Image Croping Tool") {
    allocateMem();
    overlayPanels();
    setBindings();
}

void MainFrame::allocateMem() {
    imgTest = nullptr;
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sideSplitter = new wxSplitterWindow(mainSplitter, ict::SIDE_SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS, createBitmap(imgTest));
    tools = new ToolsPanel(sideSplitter, ict::TOOLS);
    preview = new PreviewPanel(sideSplitter, ict::PREVIEW);
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
    //updatePreview();
}

wxBitmap MainFrame::createBitmap(Magick::Image *img) {
    if(!img) return wxBitmap();
    return wxBitmap(wxImage(img->columns(), img->rows(), 
            extractRgb(*img), extractAlpha(*img)));
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(tools, preview);
    mainSplitter->SplitVertically(canvas, sideSplitter);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::setBindings() {
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::onExit, this);
    tools->Bind(wxEVT_BUTTON, &MainFrame::applyChanges, this, ict::APPLY_BT);
}

void MainFrame::applyChanges(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
}

void MainFrame::onExit(wxCloseEvent &event) {
    if(preview) preview->Destroy();
    if(tools) tools->Destroy();
    if(canvas) canvas->Destroy();
    if(sideSplitter) sideSplitter->Destroy();
    if(mainSplitter) mainSplitter->Destroy();
    if(imgTest) delete(imgTest);
    Destroy();
}

void MainFrame::updatePreview() {
    preview->updatePreview(*imgTest);
}
