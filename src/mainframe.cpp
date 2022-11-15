#include "mainframe.h"
#include "Magick++.h"
#include "canvaspanel.h"
#include "cropevent.h"
#include "identifiersdef.h"
#include "previewpanel.h"
#include "wx/splitter.h"
#include "imgtools.h"

using Magick::Quantum;

MainFrame::MainFrame(): wxFrame(NULL, wxID_ANY, "Image Croping Tool") {
    allocateMem();
    overlayPanels();
    setBindings();
}

MainFrame::~MainFrame() {
    if(preview) preview->Destroy();
    if(tools) tools->Destroy();
    if(canvas) canvas->Destroy();
    if(sideSplitter) sideSplitter->Destroy();
    if(mainSplitter) mainSplitter->Destroy();
    if(highResImg) delete(highResImg);
    if(lowResImg) delete(lowResImg);
    if(canvasBitmap) delete(canvasBitmap);
    if(previewBitmap) delete(previewBitmap);
}

void MainFrame::allocateMem() {
    highResImg = new Magick::Image("/usr/share/icons/Adwaita/512x512/devices/computer.png");
    lowResImg = new Magick::Image(*highResImg);
    Magick::Geometry newRes(lowResImg->columns() * 0.3, lowResImg->rows() * 0.3);
    lowResImg->zoom(newRes);
    canvasBitmap = createBitmap(lowResImg);
    previewBitmap = new wxBitmap(*canvasBitmap);
    mainSplitter = new wxSplitterWindow(this, ict::MAIN_SPLITTER);
    sideSplitter = new wxSplitterWindow(mainSplitter, ict::SIDE_SPLITTER);
    canvas = new CanvasPanel(mainSplitter, ict::CANVAS, canvasBitmap);
    tools = new ToolsPanel(sideSplitter, ict::TOOLS);
    preview = new PreviewPanel(sideSplitter, ict::PREVIEW);
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
    updatePreview();
}

wxBitmap * MainFrame::createBitmap(Magick::Image *img) {
    if(!img) return nullptr;
    return new wxBitmap(wxImage(img->columns(), img->rows(), 
            extractRgb(*img), extractAlpha(*img)));
}

void MainFrame::overlayPanels() {
    sideSplitter->SplitHorizontally(tools, preview);
    mainSplitter->SplitVertically(canvas, sideSplitter);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizerAndFit(mainSizer);
}

void MainFrame::setBindings() {
    tools->Bind(wxEVT_BUTTON, &MainFrame::applyChanges, this, ict::APPLY_BT);
    canvas->Bind(EVT_CROP_CHANGE, &MainFrame::onCropChange, this);
}

void MainFrame::onCropChange(CropEvent &event) {
    // update preview
}

void MainFrame::applyChanges(wxCommandEvent &event) {
    // update preview and/or update crop rectangle
}

void MainFrame::updatePreview() {
    preview->updatePreview(previewBitmap);
}
