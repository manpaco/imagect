#ifndef CANVASITEM_H
#define CANVASITEM_H

class CanvasItem {
public:
    CanvasItem();
    CanvasItem(CanvasItem &&);
    CanvasItem(const CanvasItem &);
    CanvasItem &operator=(CanvasItem &&);
    CanvasItem &operator=(const CanvasItem &);
    ~CanvasItem();

private:
    
};

#endif // !CANVASITEM_H
