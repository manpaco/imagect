#include "previewframe.h"
#include "Magick++/Image.h"
#include "pixeltool.h"
#include <wx/event.h>
#include <wx/gtk/frame.h>

PreviewFrame::PreviewFrame(wxWindow *parent, wxWindowID id, const wxString &title, Magick::Image &img) : wxFrame() {
    Create(parent, id, title);
    createAdapter(img);
    Bind(wxEVT_PAINT, &PreviewFrame::paintImage, this);
}

void PreviewFrame::paintImage(wxPaintEvent &event) {
    wxBitmap bm(adapter);
    wxPaintDC previewDC(this);
    previewDC.DrawBitmap(bm, 0, 0);
}

void PreviewFrame::createAdapter(Magick::Image &img) {
    PixelTool extractor;
    adapter.Create(img.columns(), img.rows(), extractor.extractRgb(img), extractor.extractAlpha(img), true);
}

void PreviewFrame::updatePreview(Magick::Image &img) {
    createAdapter(img);
    Update();
}
