#include "recthelper.hpp"

void inflateRect(wxRect2DDouble *r, wxDouble d) {
    r->SetLeft(r->GetLeft() - d);
    r->SetTop(r->GetTop() - d);
    r->SetRight(r->GetRight() + d);
    r->SetBottom(r->GetBottom() + d);
}

void truncRect(wxRect2DDouble *r) {
    r->m_x = floor(r->m_x);
    r->m_y = floor(r->m_y);
    r->m_width = floor(r->m_width);
    r->m_height = floor(r->m_height);
}
