#ifndef __MULTISLIDER_P_H__
#define __MULTISLIDER_P_H__

#include <QObject>

class MultiSliderPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MultiSlider)
protected:
    MultiSlider* const q_ptr;

public:
    MultiSliderPrivate(MultiSlider& object);
    void init();

    /// \brief function return first handle at given pos.
    /// \param[in]  pos given position
    /// \param[out] if handle was found param is equal to founded handle rect. Otherwise return empty rect
    /// \return the handle at the given pos. Otherwise return -1
    int handleAtPos(const QPoint& pos, QRect& handleRect) const;

    ///
    /// \brief posBetweenHandles checks that point is between two handles and
    /// \param point pos
    /// \return index of left handle, if not returns -1
    ///
    int posBetweenHandles(QPoint pos) const;

    /// Copied verbatim from QSliderPrivate class (see QSlider.cpp)
    int pixelPosToRangeValue(int pos) const;
    int pixelPosFromRangeValue(int val) const;

    /// \brief draw slider handle
    /// \param[in]  num handle number from left to right
    /// \param[in]  painter painter to draw handle
    void drawHandle(int num, QStylePainter* painter) const;

    /// Values on slider
    QVector<int> m_values;

    /// Positions on slider.
    QVector<int> m_positions;

    /// See QSliderPrivate::clickOffset.
    /// Overrides this var
    int m_subclassClickOffset;

    /// See QSliderPrivate::position
    /// Overrides this var.
    int m_subclassPosition;

    /// Original width between the 2 bounds before any moves
    float m_subclassWidth;

    /// vector of selected handles numbers
    QVector<int> m_selectedHandles;

    /// tooltip to be displayed on handle
    QString m_handleToolTip;

    /// count of slider handles
    int m_count;
    /// maximum available count of handles
    int m_maxCount;
    /// minimum range between handles
    int m_minimumRange;

private:
    Q_DISABLE_COPY(MultiSliderPrivate)
};

#endif //__MULTISLIDER_H__
