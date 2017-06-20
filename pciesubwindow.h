#ifndef PCIESUBWINDOW_H
#define PCIESUBWINDOW_H

#include <QMdiSubWindow>

class PCIeSubWindow : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit PCIeSubWindow(QMdiSubWindow *parent = 0);

    enum SubWindowType {
        swNoneType = 0,
        swHighSpeed,
        swMediumSpeed,
        swSingleWire,
        swTripleWire,
    };
    /// setter
    void setSubWindowType(SubWindowType windowType) { subWindowType = windowType; }
    /// getter
    SubWindowType getSubWindowType() const { return subWindowType; }

signals:

public slots:

private:
    SubWindowType subWindowType;
};

#endif // PCIESUBWINDOW_H
