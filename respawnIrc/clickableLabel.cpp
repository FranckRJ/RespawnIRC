#include "clickableLabel.hpp"

clickableLabelClass::clickableLabelClass(int newLabelID, QWidget* parent) : QLabel(parent)
{
    labelID = newLabelID;
}

void clickableLabelClass::mousePressEvent(QMouseEvent* event)
{
    emit clicked(event->button(), labelID);
}
