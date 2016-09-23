#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QStyleOptionSlider>
#include <QApplication>
#include <QStylePainter>
#include <QStyle>
#include <QToolTip>

#include "MultiSlider.h"
#include "MultiSlider_p.h"

namespace
{
QRect AdjustRectForMac(const QRect& rect)
{
    return rect.adjusted(3, 2, -3, -2);
}
}

MultiSliderPrivate::MultiSliderPrivate(MultiSlider& object)
  :q_ptr(&object)
  , m_subclassClickOffset(0)
  , m_subclassPosition(0)
  , m_subclassWidth(0.0)
  , m_count(0)
  , m_maxCount(0)
  , m_minimumRange(0)
{
    
}

void MultiSliderPrivate::init()
{
    Q_Q(MultiSlider);
    q->refreshMaxCount();
    q->connect(q, &MultiSlider::rangeChanged, q, &MultiSlider::onRangeChanged);
    q->connect(q, &MultiSlider::rangeChanged, q, &MultiSlider::refreshMaxCount);
    q->connect(q, &MultiSlider::countChanged, q, &MultiSlider::refreshMaxCount);
}

int MultiSliderPrivate::handleAtPos(const QPoint& pos, QRect &handleRect) const
{
    Q_Q(const MultiSlider);

    QStyleOptionSlider option;
    q->initStyleOption( &option );

    // The functinos hitTestComplexControl only know about 1 handle. As we have
    // d->m_count, we change the position of the handle and test if the pos correspond to
    // any of the 2 positions.

    for(int i = m_count - 1;  i >= 0;  --i)
    {
        option.sliderPosition = this->m_positions.at(i);
        option.sliderValue = this->m_values.at(i);

        if(q->style()->hitTestComplexControl(QStyle::CC_Slider, &option, pos, q)
                == QStyle::SC_SliderHandle)
        {
            handleRect = q->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);
            return i;
        }
    }
    return -1;
}

int MultiSliderPrivate::posBetweenHandles(QPoint pos) const
{
    Q_Q(const MultiSlider);
    int mepos = q->orientation() == Qt::Horizontal ? pos.x() : pos.y();

    QStyleOptionSlider option;
    q->initStyleOption( &option );
    for(int i = 0;  i < m_count - 1;  ++i)
    {
        option.sliderPosition = m_positions.at(i);
        option.sliderValue = m_values.at(i);
        QRect handleRect = q->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);
        option.sliderPosition = m_positions.at(i + 1);
        option.sliderValue = m_values.at(i + 1);
        QRect nextHandleRect = q->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, q);
        if(q->style()->objectName() == "macintosh")
        {
            handleRect = AdjustRectForMac(handleRect);
            nextHandleRect = AdjustRectForMac(nextHandleRect);
        }
        int minCenter = (q->orientation() == Qt::Horizontal ?
            handleRect.center().x() : nextHandleRect.center().y());
        int maxCenter = (q->orientation() == Qt::Horizontal ?
            nextHandleRect.center().x() : handleRect.center().y());
        if(mepos > minCenter && mepos < maxCenter)
        {
            return i;
        }
    }
    return -1;

}

// --------------------------------------------------------------------------
// Copied verbatim from QSliderPrivate::pixelPosToRangeValue. See QSlider.cpp
//
int MultiSliderPrivate::pixelPosToRangeValue( int pos ) const
{
  Q_Q(const MultiSlider);
  QStyleOptionSlider option;
  q->initStyleOption( &option );

  QRect gr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderGroove,
                                            q );
  QRect sr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            q );
    int sliderMin, sliderMax, sliderLength;
    if (option.orientation == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition( q->minimum(),
                                            q->maximum(),
                                            pos - sliderMin,
                                            sliderMax - sliderMin,
                                            option.upsideDown );
}

int MultiSliderPrivate::pixelPosFromRangeValue( int val ) const
{
    Q_Q(const MultiSlider);
    QStyleOptionSlider option;
    q->initStyleOption( &option );

    QRect gr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderGroove,
                                            q );
    QRect sr = q->style()->subControlRect( QStyle::CC_Slider,
                                            &option,
                                            QStyle::SC_SliderHandle,
                                            q );
    int sliderMin, sliderMax, sliderLength;
    if (option.orientation == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }

    return QStyle::sliderPositionFromValue( q->minimum(),
                                            q->maximum(),
                                            val,
                                            sliderMax - sliderMin,
                                            option.upsideDown ) + sliderMin;
}

// Draw slider at the bottom end of the range
void MultiSliderPrivate::drawHandle(int num, QStylePainter* painter ) const
{
    Q_Q(const MultiSlider);
    QStyleOptionSlider option;
    q->initSliderStyleOption(num, &option );

    option.subControls = QStyle::SC_SliderHandle;
    option.sliderValue = m_values.at(num);
    option.sliderPosition = m_positions.at(num);
    if (m_selectedHandles.contains(num))
    {
        option.activeSubControls = QStyle::SC_SliderHandle;
        option.state |= QStyle::State_Sunken;
    }
    if(q->style()->objectName() == "macintosh")
    {
        // On mac style, drawing just the handle actually draws also the groove.
        QRect clip = q->style()->subControlRect(QStyle::CC_Slider, &option,
                                                QStyle::SC_SliderHandle, q);
        clip = AdjustRectForMac(clip);
        QString path = QString(":/Icons/knob") + (m_selectedHandles.contains(num) ? "_selected" : "") + ".png";
        painter->drawPixmap(clip, QPixmap(path));
    }
    else
    {
        painter->drawComplexControl(QStyle::CC_Slider, option);
    }
}

MultiSlider::MultiSlider(QWidget* _parent)
    : QSlider(_parent)
    , d_ptr(new MultiSliderPrivate(*this))
{
    Q_D(MultiSlider);
    d->init();
}

QColor MultiSlider::color(int index, double bright)
{
    return QColor::fromHslF((index % 7) * 1.0 / 7, 1, bright);
}

MultiSlider::MultiSlider( Qt::Orientation o,
                                  QWidget* parentObject )
    :QSlider(o, parentObject)
    , d_ptr(new MultiSliderPrivate(*this))
{
    Q_D(MultiSlider);
    d->init();
}

MultiSlider::MultiSlider(MultiSliderPrivate* impl, QWidget* _parent)
    : QSlider(_parent)
    , d_ptr(impl)
{
    Q_D(MultiSlider);
    d->init();
}

MultiSlider::MultiSlider( MultiSliderPrivate* impl, Qt::Orientation o,
                                QWidget* parentObject )
    :QSlider(o, parentObject)
    , d_ptr(impl)
{
    Q_D(MultiSlider);
    d->init();
}

bool MultiSlider::isHandleDown(int index) const
{
    Q_D(const MultiSlider);
    return d->m_selectedHandles.contains(index);
}

int MultiSlider::count() const
{
    Q_D(const MultiSlider);
    return d->m_count;
}

int MultiSlider::minimumRange() const
{
    Q_D(const MultiSlider);
    return d->m_minimumRange;
}

int MultiSlider::maxCount() const
{
    Q_D(const MultiSlider);
    return d->m_maxCount;
}

void MultiSlider::setCount(int arg)
{
    Q_D(MultiSlider);
    Q_ASSERT(arg >= 0);
    Q_ASSERT(arg < maxCount());
    if (d->m_count == arg)
    {
        return;
    }
    if(arg > d->m_count)
    {
        addToRight(arg - d->m_count);
    }
    else
    {
        removeFromRight(d->m_count - arg);
    }
}

void MultiSlider::setMinimumRange(int arg)
{
    Q_D(MultiSlider);
    if (d->m_minimumRange == arg)
    {
        return;
    }

    d->m_minimumRange = arg;
    refreshMaxCount();
    normalize(true);
    emit minimumRangeChanged(arg);
    update();
}

void MultiSlider::refreshMaxCount()
{
    Q_D(MultiSlider);
    int newMaxCount = d->m_minimumRange ? ((maximum() - minimum()) / d->m_minimumRange - 1) : std::numeric_limits<int>::max(); //first position can be zero and we set margin as d->m_minimumRange
    if(d->m_maxCount != newMaxCount)
    {
        d->m_maxCount = newMaxCount;
        if(d->m_count > d->m_maxCount)
        {
            setCount(d->m_maxCount);
        }
        emit maxCountChanged(d->m_maxCount);
    }
}

void MultiSlider::addOneToleft()
{
    addToLeft(1);
}

void MultiSlider::addToLeft(int arg)
{
    Q_D(MultiSlider);
    if(d->m_count == maxCount())
    {
        return;
    }
    arg = qMin(arg, maxCount() - d->m_count);
    int minDist = d->m_minimumRange;
    if(0 == minDist)
    {
        if(d->m_positions.isEmpty())
        {
            minDist = (maximum() - minimum()) / 5;
        }
        else
        {
            minDist = (d->m_positions.first() - minimum()) / 5;
        }
    }
    if(d->m_count && d->m_positions.first() - minimum() < minDist * 2) //margin is min range
    {
        movePosition(0, arg * minDist);
    }
    for(int i = 0; i < arg; ++i)
    {
        int position = i * minDist + minimum() + minDist;
        d->m_count++;
        d->m_positions.insert(i, position);
        d->m_values.insert(i, position);
    }
    emit countChanged(d->m_count);
    normalize(true);
    update();
}

void MultiSlider::addOneToRight()
{
    addToRight(1);
}

void MultiSlider::addToRight(int arg)
{
    Q_D(MultiSlider);
    if(d->m_count == maxCount())
    {
        return;
    }
    arg = qMin(arg, maxCount() - d->m_count);
    int minDist = d->m_minimumRange;
    if(0 == minDist)
    {
        if(d->m_positions.isEmpty())
        {
            minDist = (maximum() - minimum()) / 5;
        }
        else
        {
            minDist = (maximum() - d->m_positions.last()) / 5;
        }
    }
    if(d->m_count && maximum() - d->m_positions.last() < minDist * 2) //margin is min range
    {
        movePosition(d->m_count - 1, -1 * arg * minDist);
    }
    int startPos = d->m_count != 0 ? (d->m_positions.last() + minDist) : minimum();
    for(int i = 0; i < arg; ++i)
    {
        int position = i * minDist + startPos;
        d->m_positions.insert(d->m_count, position);
        d->m_values.insert(d->m_count, position);
        d->m_count++;
    }
    emit countChanged(d->m_count);
    normalize(true);
    update();
}

void MultiSlider::removeOneFromLeft()
{
    removeFromLeft(1);
}

void MultiSlider::removeFromLeft(int count)
{
    Q_D(MultiSlider);
    count = qMin(d->m_count, count);
    for(int i = 0; i < count; ++i)
    {
        d->m_count--;
        d->m_positions.removeFirst();
        d->m_values.removeFirst();
    }
    emit countChanged(d->m_count);
    normalize(true);
    update();
}

void MultiSlider::removeOneFromRight()
{
    removeFromRight(1);
}

void MultiSlider::removeFromRight(int count)
{
    Q_D(MultiSlider);
    count = qMin(d->m_count, count);
    for(int i = 0; i < count; ++i)
    {
        d->m_count--;
        d->m_positions.removeLast();
        d->m_values.removeLast();
    }
    emit countChanged(d->m_count);
    normalize(true);
    update();
}

QVector<int> MultiSlider::values() const
{
    Q_D(const MultiSlider);
    return d->m_values;
}

QVector<int> MultiSlider::positions() const
{
    Q_D(const MultiSlider);
    return d->m_positions;
}

int MultiSlider::position(int index) const
{
    Q_D(const MultiSlider);
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < d->m_count);
    return d->m_positions.at(index);
}

int MultiSlider::value(int index) const
{
    Q_D(const MultiSlider);
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < d->m_count);
    return d->m_values.at(index);
}

void MultiSlider::setPosition(int index, int arg)
{
    Q_D(MultiSlider);
    Q_ASSERT(index >= 0);
    Q_ASSERT(index <= d->m_count);
    if (d->m_positions.at(index) != arg)
    {
        movePosition(index, arg - d->m_positions.at(index));
        emit positionsChanged(d->m_positions);
        if (hasTracking())
        {
            d->m_values = d->m_positions;
            emit valuesChanged(d->m_values);
        }
        update();
    }
}

void MultiSlider::setValue(int index, int arg)
{
    Q_D(MultiSlider);
    Q_ASSERT(index >= 0);
    Q_ASSERT(index <= d->m_count);
    if (d->m_values.at(index) != arg)
    {
        setPosition(index, arg);
        d->m_values = d->m_positions;
        emit valuesChanged(d->m_values);
        update();
    }
}

void MultiSlider::setValues(QVector<int> values)
{
    Q_D(MultiSlider);
    Q_ASSERT(d->m_values.size() == this->count());
    if (d->m_values != values)
    {
        d->m_positions = values;
        d->m_values = values;
        normalize(false);
        emit positionsChanged(d->m_positions);
        emit valuesChanged(d->m_values);
        update();
    }
}

void MultiSlider::movePosition(int index, int arg)
{
    Q_D(MultiSlider);
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < d->m_count);
    int position = d->m_positions.at(index);
    if(index == (arg < 0 ? 0 : (d->m_count - 1)))
    {
        d->m_positions.replace(index, qBound(minimum() + minimumRange(), position + arg, maximum() - minimumRange()));
    }
    else
    {
        int nextStep = arg < 0 ? -1 : 1;
        int minimumRange = d->m_minimumRange * nextStep;

        int nextPosition = d->m_positions.at(index + nextStep);
        if(qAbs(arg) < qAbs(position - nextPosition + minimumRange) )
        {
            d->m_positions.replace(index, position + arg);
        }
        else
        {
            movePosition(index + nextStep, arg - (nextPosition - position) + minimumRange);
            nextPosition = d->m_positions.at(index + nextStep);
            d->m_positions.replace(index, nextPosition - minimumRange);
        }
    }
    update();
}

void MultiSlider::normalize(bool emitIfChanged)
{
    Q_D(MultiSlider);
    if(d->m_count == 0)
    {
        return;
    }
    QVector<int> oldPositions = d->m_positions;
    for(int i = -1; i < d->m_count - 1; ++i) //do not check last element
    {
        int diff =  i == -1 ? d->m_positions.first() - minimum() : d->m_positions.at(i + 1) - d->m_positions.at(i);
        if(diff < d->m_minimumRange)
        {
            movePosition(i + 1, d->m_minimumRange - diff);
        }
    }
    for(int i = d->m_count;  i > 0; --i)
    {
        int diff = i == d->m_count ? maximum() - d->m_positions.last() : d->m_positions.at(i) - d->m_positions.at(i - 1);
        if(diff < d->m_minimumRange)
        {
            movePosition(i - 1, d->m_minimumRange - diff);
        }
    }
    if (emitIfChanged && d->m_positions != oldPositions)
    {
        emit positionsChanged(d->m_positions);
        if(hasTracking())
        {
            d->m_values = d->m_positions;
            emit valuesChanged(d->m_values);
        }
    }
}

// --------------------------------------------------------------------------
void MultiSlider::onRangeChanged(int /*_minimum*/, int /*_maximum*/)
{
    normalize(true);
}

// --------------------------------------------------------------------------
// Render
void MultiSlider::drawColoredRect(int pos, int nextPos, QStylePainter &painter, QColor highlight)
{
    QStyleOptionSlider option;
    this->initStyleOption(&option);
    option.sliderPosition = pos;
    const QRect lr = style()->subControlRect( QStyle::CC_Slider,
                                        &option,
                                        QStyle::SC_SliderHandle,
                                        this);

    option.sliderPosition = nextPos;

    const QRect ur = style()->subControlRect( QStyle::CC_Slider,
                                        &option,
                                        QStyle::SC_SliderHandle,
                                        this);

    QRect groove = style()->subControlRect( QStyle::CC_Slider,
                                        &option,
                                        QStyle::SC_SliderGroove,
                                        this);
    if (option.orientation == Qt::Horizontal)
    {
        const int padding = (style()->objectName() == "macintosh") ? 5 : 2;
        groove = QRect(
                        QPoint(qMin( lr.left(), ur.center().x()) + (pos != 0 ? 10 : 0), groove.center().y() - 2),
                        QPoint(qMax( lr.left(), ur.right() - padding) , groove.center().y() + 1));
    }
    else
    {
        groove = QRect(
                        QPoint(groove.center().x() - 2, qMin( lr.center().y(), ur.center().y() )),
                        QPoint(groove.center().x() + 1, qMax( lr.center().y(), ur.top())));
    }

    painter.setPen(QPen(highlight.darker(150), 0));
    painter.setBrush(highlight);
    painter.drawRect( groove );
}

void MultiSlider::paintEvent( QPaintEvent* )
{
    Q_D(MultiSlider);
    QStyleOptionSlider option;
    this->initStyleOption(&option);
    QStylePainter painter(this);
    option.subControls = QStyle::SC_SliderGroove;
    if(this->tickPosition() != NoTicks)
    {
        option.subControls |= QStyle::SC_SliderTickmarks;
    }
    // Move to minimum to not highlight the SliderGroove.
    // On mac style, drawing just the slider groove also draws the handles,
    // therefore we give a negative (outside of view) position.
    option.sliderValue = this->minimum() - this->maximum();
    option.sliderPosition = this->minimum() - this->maximum();
    painter.drawComplexControl(QStyle::CC_Slider, option);
    for(int i = 0;  i < d->m_count; ++i)
    {
        //d->drawHandle(i, &painter);
    }
    int pos = minimum();
    int nextPos = d->m_count ? d->m_positions.at(0) : maximum();
    drawColoredRect(pos, nextPos, painter, color(0, 0.5));
    if(d->m_count == 0)
    {
        return;
    }
    for(int i = 0;  i < d->m_count - 1; ++i)
    {
        pos = d->m_positions.at(i);
        nextPos = d->m_positions.at(i + 1);
        drawColoredRect(pos, nextPos, painter, color(i + 1, 0.5));
    }
    pos = d->m_positions.last();
    nextPos = maximum();
    drawColoredRect(pos, nextPos, painter, color(d->m_count + 1, 0.5));

    for(int i = 0;  i < d->m_count; ++i)
    {
        d->drawHandle(i, &painter);
    }
}

// --------------------------------------------------------------------------
// Standard Qt UI events
void MultiSlider::mousePressEvent(QMouseEvent* mouseEvent)
{
    Q_D(MultiSlider);
    if (minimum() == maximum() || (mouseEvent->buttons() ^ mouseEvent->button()))
    {
        mouseEvent->ignore();
        return;
    }
    int mepos = this->orientation() == Qt::Horizontal ? mouseEvent->pos().x() : mouseEvent->pos().y();

    QStyleOptionSlider option;
    this->initStyleOption( &option );

    QRect handleRect;
    int handle = d->handleAtPos(mouseEvent->pos(), handleRect);

    if (handle != -1)
    {
        d->m_subclassPosition = d->m_positions.at(handle);

        // save the position of the mouse inside the handle for later
        d->m_subclassClickOffset = mepos - (this->orientation() == Qt::Horizontal ?
            handleRect.left() : handleRect.top());

        this->setSliderDown(true);
        selectHandle(handle);

        // Accept the mouseEvent
        mouseEvent->accept();
        return;
    }

  // if we are here, no handles have been pressed
  // Check if we pressed on the groove between the 2 handles
  
    QStyle::SubControl control = this->style()->hitTestComplexControl(QStyle::CC_Slider, &option, mouseEvent->pos(), this);

    int index = d->posBetweenHandles(mouseEvent->pos());
    if (control == QStyle::SC_SliderGroove && -1 != index)
    {
        // warning lost of precision it might be fatal
        d->m_subclassPosition = (d->m_positions.at(index) + d->m_positions.at(index + 1)) / 2.;
        d->m_subclassClickOffset = mepos - d->pixelPosFromRangeValue(d->m_subclassPosition);
        d->m_subclassWidth = (d->m_positions.at(index + 1) - d->m_positions.at(index)) / 2.;
        this->setSliderDown(true);
        if (!this->isHandleDown(index) || !this->isHandleDown(index + 1))
        {
            d->m_selectedHandles.clear();
            selectTwoHandles(index, index + 1);
        }
        mouseEvent->accept();
        return;
    }
    mouseEvent->ignore();
}

int MultiSlider::selectedHandle() const
{
    Q_D(const MultiSlider);
    return d->m_selectedHandles.isEmpty() ? -1 : d->m_selectedHandles.first();
}

void MultiSlider::selectHandle(int handle)
{
    Q_D(MultiSlider);
    Q_ASSERT(handle >= 0);
    Q_ASSERT(handle < d->m_count);
    if (!d->m_selectedHandles.contains(handle))
    {
        d->m_selectedHandles.clear();
        d->m_selectedHandles.push_back(handle);
        emit selectedHandleChanged(handle);
        update();
    }
}

void MultiSlider::selectTwoHandles(int firstHandle, int secondHandle)
{
    Q_D(MultiSlider);
    Q_ASSERT(firstHandle >= 0 && secondHandle >= 1);
    Q_ASSERT(firstHandle < d->m_count - 1);
    Q_ASSERT(secondHandle < d->m_count);
    if (!d->m_selectedHandles.contains(firstHandle) || !d->m_selectedHandles.contains(secondHandle))
    {
        d->m_selectedHandles.clear();
        d->m_selectedHandles.push_back(firstHandle);
        d->m_selectedHandles.push_back(secondHandle);
        emit selectedHandleChanged(firstHandle);
        update();
    }
}

// --------------------------------------------------------------------------
// Standard Qt UI events
void MultiSlider::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    Q_D(MultiSlider);
    if (d->m_selectedHandles.isEmpty())
    {
        mouseEvent->ignore();
        return;
    }
    int mepos = this->orientation() == Qt::Horizontal ?
        mouseEvent->pos().x() : mouseEvent->pos().y();

    QStyleOptionSlider option;
    this->initStyleOption(&option);

    int newPosition = d->pixelPosToRangeValue(mepos - d->m_subclassClickOffset);

    switch (d->m_selectedHandles.size())
    {
    case 1:
        setPosition(d->m_selectedHandles.first(), newPosition);
        break;
    case 2:
        setPosition(d->m_selectedHandles.at(0), newPosition - static_cast<int>(d->m_subclassWidth));
        setPosition(d->m_selectedHandles.at(1), newPosition + static_cast<int>(d->m_subclassWidth + .5));
        break;
    default:
        Q_ASSERT(!"error selected handles count");
        break;
    }
    mouseEvent->accept();
}

// --------------------------------------------------------------------------
// Standard Qt UI mouseEvents
void MultiSlider::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  Q_D(MultiSlider);
  this->QSlider::mouseReleaseEvent(mouseEvent);

  setSliderDown(false);
  d->m_selectedHandles.clear();
  if(d->m_values != d->m_positions)
  {
      d->m_values = d->m_positions;
      emit valuesChanged(d->m_values);
  }
  update();
}

// --------------------------------------------------------------------------
void MultiSlider::initSliderStyleOption(int num, QStyleOptionSlider* option) const
{
    Q_UNUSED(num);
    this->initStyleOption(option);
}

// --------------------------------------------------------------------------
QString MultiSlider::handleToolTip()const
{
    Q_D(const MultiSlider);
    return d->m_handleToolTip;
}

// --------------------------------------------------------------------------
void MultiSlider::setHandleToolTip(const QString& _toolTip)
{
    Q_D(MultiSlider);
    d->m_handleToolTip = _toolTip;
}

// --------------------------------------------------------------------------
bool MultiSlider::event(QEvent* _event)
{
    Q_D(MultiSlider);
    switch(_event->type())
    {
    case QEvent::ToolTip:
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(_event);
        QRect rect;
        int handle = d->handleAtPos(helpEvent->pos(), rect);
        if(handle != -1)
        {
            QToolTip::showText(helpEvent->globalPos(), d->m_handleToolTip.arg(d->m_positions.at(handle)));
            _event->accept();
        }
    }
    default:
        break;
    }
    return this->Superclass::event(_event);
}
