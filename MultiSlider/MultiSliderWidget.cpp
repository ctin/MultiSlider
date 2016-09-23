#include "MultiSliderWidget.h"
#include "MultiSlider.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStyleOptionSpinBox>
#include <QSpinBox>
#include <QPushButton>

class MultiSliderWidget::SpinBox : public QSpinBox
{
public:
    SpinBox(int index, QWidget* parent = 0);
    int index() const;

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    int m_index;
};

MultiSliderWidget::SpinBox::SpinBox(int index_, QWidget* parent)
    : QSpinBox(parent)
    , m_index(index_)
{
}

int MultiSliderWidget::SpinBox::index() const
{
    return m_index;
}

void MultiSliderWidget::SpinBox::focusInEvent(QFocusEvent* event)
{
    selectAll();
    setStyleSheet("background-color: " + MultiSlider::color(m_index, 0.93).name() + ";");
    QSpinBox::focusInEvent(event);
}

void MultiSliderWidget::SpinBox::focusOutEvent(QFocusEvent* event)
{
    setStyleSheet("background-color: white;");
    QSpinBox::focusOutEvent(event);
}

MultiSliderWidget::MultiSliderWidget(QWidget* parent)
    : QFrame(parent)
{
    initMultiSlider();
    labelsLayout = new QHBoxLayout();
    labelsLayout->setSpacing(10);
    createWidget();

    setLayout(widgetLayout);
    
    setLabelsUnder(true);
    onSliderCountChanged(multiSlider->count());
    onSelectedHandleChanged(multiSlider->selectedHandle());
    onSliderPositionsChanged(multiSlider->positions());
    installEventFilter(this);

    onLabelsUnderChanged();
}

MultiSlider* MultiSliderWidget::GetMultiSlider()
{
    return multiSlider;
}

QPushButton* MultiSliderWidget::GetButtonAddToLeft()
{
    return addToLeftButton;
}

QPushButton* MultiSliderWidget::GetButtonAddToRight()
{
    return addToRightButton;
}

QPushButton* MultiSliderWidget::GetButtonRemoveFromLeft()
{
    return removeFromLeftButton;
}

QPushButton* MultiSliderWidget::GetButtonRemoveFromRight()
{
    return removeFromRightButton;
}

bool MultiSliderWidget::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        SpinBox* spinBox = dynamic_cast<SpinBox*>(obj);
        int handle = -1;
        if(nullptr != spinBox)
        {
            handle = spinBox->index();
            if (handle >= multiSlider->count())
            {
                handle = multiSlider->count() - 1;
            }
        }
        else
        {
            handle = multiSlider->selectedHandle();
        }
        switch(keyEvent->key())
        {
            default:
                return false;
            case Qt::Key_Tab:
                selectNextSpinBox();
                break;
            case Qt::Key_Left:
                if (handle >= 0)
                {
                    multiSlider->setPosition(handle, multiSlider->position(handle) - 1);
                }
                break;
            case Qt::Key_Right:
                if (handle >= 0)
                {
                    multiSlider->setPosition(handle, multiSlider->position(handle) + 1);
                }
                break;
        }
        return true;
    }
    return false;
}

void MultiSliderWidget::createWidget()
{
    widgetLayout = new QHBoxLayout();
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);

    auto CreateFlatButton = [](QString iconAddr) -> QPushButton* {
        QPushButton* button = new QPushButton(QIcon(iconAddr), "");
        button->setFixedWidth(button->iconSize().width());
        button->setFlat(true);
        return button;
    };

    addToLeftButton = CreateFlatButton(":/Icons/add.png");
    connect(addToLeftButton, &QPushButton::clicked, multiSlider, &MultiSlider::addOneToleft);
    vLayout->addWidget(addToLeftButton);
    removeFromLeftButton = CreateFlatButton(":/Icons/remove.png");
    vLayout->addWidget(removeFromLeftButton);
    connect(removeFromLeftButton, &QPushButton::clicked, multiSlider, &MultiSlider::removeOneFromLeft);
    vLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    widgetLayout->addLayout(vLayout);

    sliderLayout = new QVBoxLayout();
    sliderLayout->setSpacing(0);
    widgetLayout->addLayout(sliderLayout);
    
    vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);

    addToRightButton = CreateFlatButton(":/Icons/add.png");
    connect(addToRightButton, &QPushButton::clicked, multiSlider, &MultiSlider::addOneToRight);

    vLayout->addWidget(addToRightButton);
    removeFromRightButton = CreateFlatButton(":/Icons/remove.png");
    connect(removeFromRightButton, &QPushButton::clicked, multiSlider, &MultiSlider::removeOneFromRight);

    vLayout->addWidget(removeFromRightButton);
    vLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    widgetLayout->addLayout(vLayout);

}

MultiSliderWidget::SpinBox* MultiSliderWidget::createSpinBox(int index)
{
    SpinBox* spinBox = new SpinBox(index, this);
    spinBox->setMinimum(multiSlider->minimum());
    spinBox->setMaximum(multiSlider->maximum());
    spinBox->installEventFilter(this);
    spinBox->setKeyboardTracking(false);
    spinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spinBox->setAlignment(Qt::AlignCenter);
    return spinBox;
}

void MultiSliderWidget::initMultiSlider()
{
    multiSlider = new MultiSlider(Qt::Horizontal, this);
    //i like three. Three is a good number:)
    //just a joke. Three used because it enough to show functional and not too high to overload ui
    multiSlider->setCount(3);
    multiSlider->installEventFilter(this);
    connect(multiSlider, &MultiSlider::selectedHandleChanged, this, &MultiSliderWidget::onSelectedHandleChanged);
    connect(multiSlider, &MultiSlider::positionsChanged, this, &MultiSliderWidget::onSliderPositionsChanged);
    connect(multiSlider, &MultiSlider::countChanged, this, &MultiSliderWidget::updateButtonsEnable);
    connect(multiSlider, &MultiSlider::maxCountChanged, this, &MultiSliderWidget::updateButtonsEnable);
    connect(multiSlider, &MultiSlider::countChanged, this, &MultiSliderWidget::onSliderCountChanged);
    connect(multiSlider, &MultiSlider::rangeChanged, this, &MultiSliderWidget::onSliderRangeChanged);
}

void MultiSliderWidget::updateButtonsEnable()
{
    int count = multiSlider->count();
    int maxcount = multiSlider->maxCount();
    addToRightButton->setEnabled(count < maxcount);
    addToLeftButton->setEnabled(count < maxcount);
    removeFromRightButton->setEnabled(count > 1);
    removeFromLeftButton->setEnabled(count > 1);
}

void MultiSliderWidget::onSliderCountChanged(int count)
{
    int spinBoxesCount = count;
    if(showDifferences() && count > 0)
    {
        spinBoxesCount++; //add extra spinbox for distance between maximum and last value
    }
    while (spinBoxes.size() > spinBoxesCount) //remove extra spinbox
    {
        delete spinBoxes.takeLast();
    }
    while (spinBoxes.size() < spinBoxesCount)
    {
        SpinBox* spinBox = createSpinBox(spinBoxes.size());

        connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MultiSliderWidget::onSpinBoxValueChanged);

        spinBoxes.append(spinBox);
        labelsLayout->insertWidget(labelsLayout->count(), spinBox);
    }
    onSliderPositionsChanged(multiSlider->positions());
}

void MultiSliderWidget::onSelectedHandleChanged(int handle)
{
    if(handle == -1)
    {
        return;
    }
    spinBoxes.at(handle)->setFocus();
}

void MultiSliderWidget::onSliderPositionsChanged(QVector<int> values)
{
    if(multiSlider->count() == 0)
    {
        return;
    }
    for (auto spinBox : spinBoxes)
    {
        spinBox->blockSignals(true);
    }
    for(int i = 0;  i < values.size();  i++)
    {
        updateSpinBoxValue(i);
    }
    if(showDifferences())
    {
        spinBoxes.last()->setValue(multiSlider->maximum() - (multiSlider->count() ? multiSlider->positions().last() : multiSlider->minimum()));
    }
    for (auto spinBox : spinBoxes)
    {
        spinBox->blockSignals(false);
    }
}

void MultiSliderWidget::onSliderRangeChanged(int min, int max)
{
    for (auto spinBox : spinBoxes)
    {
        spinBox->blockSignals(true);
        spinBox->setMinimum(min);
        spinBox->setMaximum(max);
        spinBox->blockSignals(false);
    }
}

void MultiSliderWidget::onSpinBoxValueChanged(int value)
{
    Q_ASSERT(dynamic_cast<SpinBox*>(sender()) != nullptr);
    SpinBox* spinBox = static_cast<SpinBox*>(sender());
    Q_ASSERT(spinBox != nullptr);
    int index = spinBox->index();
    if (showDifferences())
    {
        if (index == multiSlider->count())
        {
            multiSlider->setValue(multiSlider->count() - 1, multiSlider->maximum() - value);
        }
        else
        {
            int sum = 0;
            for (int i = 0; i <= index; i++)
            {
                sum += spinBoxes.at(i)->value();
            }
            multiSlider->setValue(index, sum);
        }
    }
    else
    {
        multiSlider->setValue(index, value);
    }
}

void MultiSliderWidget::updateSpinBoxValue(int i)
{
    const QVector<int> &positions = multiSlider->positions();
    if(showDifferences())
    {
        spinBoxes.at(i)->setValue(positions.at(i) - (i ? positions.at(i - 1) : multiSlider->minimum()));
    }
    else
    {
        spinBoxes.at(i)->setValue(positions.at(i));
    }
}

void MultiSliderWidget::selectNextSpinBox()
{
    for(int i = 0; i < spinBoxes.size(); ++i)
    {
        if(spinBoxes.at(i)->hasFocus())
        {
            int nextIndex = (i == spinBoxes.size() - 1) ? 0 : i + 1;
            spinBoxes.at(nextIndex)->setFocus();
            return;
        }
    }
    int selectedHandle = multiSlider->selectedHandle();
    if(multiSlider->count() == 0)
    {
        return;
    }
    int nextIndex = (selectedHandle == spinBoxes.size() - 1) ? 0 : selectedHandle + 1;
    spinBoxes.at(nextIndex)->setFocus();
}

void MultiSliderWidget::onLabelsUnderChanged()
{
    sliderLayout->takeAt(0);
    sliderLayout->takeAt(0);
    if (m_labelsUnder)
    {
        sliderLayout->addLayout(labelsLayout);
    }
    sliderLayout->addWidget(multiSlider);
    if (!m_labelsUnder)
    {
        sliderLayout->addLayout(labelsLayout);
    }
    setFixedHeight(this->minimumSizeHint().height());
}

bool MultiSliderWidget::labelsUnder() const
{
    return m_labelsUnder;
}

bool MultiSliderWidget::showPositions() const
{
    return !m_showDifferences;
}

bool MultiSliderWidget::showDifferences() const
{
    return m_showDifferences;
}

void MultiSliderWidget::setLabelsUnder(bool arg)
{
    if (m_labelsUnder == arg)
        return;
    m_labelsUnder = arg;
    onLabelsUnderChanged();
    emit labelsUnderChanged(arg);
}

void MultiSliderWidget::setShowPositions(bool arg)
{
    if (!m_showDifferences == arg)
        return;
    
    m_showDifferences = !arg;
    onSliderCountChanged(multiSlider->count());
    emit showPositionsChanged(arg);
    emit showDifferencesChanged(!arg);
}

void MultiSliderWidget::setShowDifferences(bool arg)
{
    if (m_showDifferences == arg)
        return;
    
    m_showDifferences = arg;
    onSliderCountChanged(multiSlider->count());
    onSliderPositionsChanged(multiSlider->positions());
    emit showDifferencesChanged(arg);
    emit showPositionsChanged(!arg);
}

