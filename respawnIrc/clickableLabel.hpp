#ifndef CLICKABLELABEL_HPP
#define CLICKABLELABEL_HPP

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class clickableLabelClass : public QLabel
{
    Q_OBJECT
public:
    explicit clickableLabelClass(int newLabelID, QWidget* parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent* event) override;
signals:
    void clicked(Qt::MouseButton buttonPressed, int labelID);
private:
    int labelID = -1;
};

#endif
