#ifndef __MULTISLIDER_H__
#define __MULTISLIDER_H__

#include <QSlider>

class QStylePainter;
class MultiSlider;

class MultiSliderPrivate;

class  MultiSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(QVector<int> values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(QVector<int> positions READ positions NOTIFY positionsChanged)
    Q_PROPERTY(int minimumRange READ minimumRange WRITE setMinimumRange NOTIFY minimumRangeChanged)
    Q_PROPERTY(QString handleToolTip READ handleToolTip WRITE setHandleToolTip)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int maxCount READ maxCount NOTIFY maxCountChanged)
    Q_PROPERTY(int selectedHandle READ selectedHandle WRITE selectHandle NOTIFY selectedHandleChanged)

public:
    typedef QSlider Superclass;
    static QColor color(int index, double bright);

    /// \brief Constructor, builds a MultiSlider with properties set the QSlider default properties.
    explicit MultiSlider( Qt::Orientation o, QWidget* parent = nullptr );

    /// \brief Constructor, builds a MultiSlider with properties set the QSlider default properties.
    explicit MultiSlider( QWidget* par = nullptr );

    ~MultiSlider() = default;
    /// \brief check that handle at given index is down
    /// \param index    index of handle from left to right
    /// \return handle selected state
    bool isHandleDown(int index) const;

    /// \brief holds the slider's current values.
    /// \return slider current values for all handles.
    QVector<int> values() const;

    /// \brief holds the current slider positions.
    /// \return slider current positions for all handles.
    QVector<int> positions() const;

    /// \brief position at given index.
    int position(int index) const;

    /// \brief value at given index.
    int value(int index) const;

    /// \brief return first selected handle
    /// \return if handle was selected by mouse or by focus - return first of selected handles from left to right. Otherwise return -1.
    int selectedHandle() const;

    /// \brief set absolute position at given index
    /// \note this function can change other positions
    /// \note if tracking is enabled (the default), this is identical to setValue
    /// \param index    number of handle from left to right
    /// \param arg  absolute value of new position
    void setPosition(int index, int arg);

    /// \brief set absolute value at given index
    /// \note this function can change other values
    /// \note this function also will set position at given index
    /// \param index    number of handle from left to right
    /// \param arg  absolute value of new value
    void setValue(int index, int arg);

    /// \brief set absolute values
    /// \note this function will override
    /// \note this function also will change all positions
    /// \note if values count must be equeal to slider count
    /// \param values  absolute values
    void setValues(QVector<int> values);

    /// \brief returns current tooltip
    QString handleToolTip() const;
    ///
    /// \brief Controls the text to display for the handle tooltip. It is in addition to the widget tooltip.
    /// \note "%1" is replaced by the current value of the slider.
    /// \note Empty string (by default) means no tooltip.
    void setHandleToolTip(const QString& toolTip);

    /// \brief This property holds the current sliders count.
    int count() const;

    /// \brief this property holds minimum range between two slider handles
    int minimumRange() const;

    /// \brief this property holds maximum count of sliders
    int maxCount() const;

Q_SIGNALS:
    ///
    /// \brief This signal is emitted when the slider values has changed.
    /// \param new slider values.
    void valuesChanged(QVector<int> arg);

    ///
    /// \brief This signal is emitted when the slider moves.
    /// \param new slider positions.
    /// This signal is emitted even when tracking is turned off.
    void positionsChanged(QVector<int> arg);

    ///
    /// \brief this signal is emitted when the sliders count changed
    /// \param The argument is the new sliders count.
    ///
    void countChanged(int arg);

    ///
    /// \brief this signal is emitted whem minimum range between handles changed
    /// \param The argument is the new value of minimum range between two handles.
    ///
    void minimumRangeChanged(uint arg);

    ///
    /// \brief this signal is emitted when maximum count of handles on slider changed
    /// \param The argument is the new maximum count of slider handles
    ///
    void maxCountChanged(int arg);

    ///
    /// \brief this signal is emitted when new handle selected
    /// \param The argument is the new maximum count of slider handles
    ///
    void selectedHandleChanged(int arg);

public Q_SLOTS:
    /// \brief This property holds the slider's count.
    /// \param argument is count to set.
    /// \note arg wil be cutted to maximumCount value
    void setCount(int arg);

    /// \brief add handles to the left.
    /// \param count    count of handles to be added
    /// \note total count can not be more than maximumCount
    void addToLeft(int count);

    /// \brief add one handle to the left.
    /// \note total count can not be more than maximumCount
    void addOneToleft();

    /// \brief add handles to the right.
    /// \param count    count of handles to be added
    /// \note total count can not be more than maximumCount
    void addToRight(int count);

    /// \brief add one handle to the right.
    /// \note total count can not be more than maximumCount
    void addOneToRight();

    /// \brief remove handles from the left.
    /// \param count    count of handles to be removed
    void removeFromLeft(int count);

    /// \brief remove one handle from the left.
    void removeOneFromLeft();

    /// \brief remove handles from the right.
    /// \param count    count of handles to be removed
    void removeFromRight(int count);

    /// \brief remove one handle from the right.
    void removeOneFromRight();

    /// \brief this function will hold minimum range between two closest handles
    /// \param arg  number of positions between thwo closest handles
    void setMinimumRange(int arg);

    /// \brief clear selection and mark given handle as selected
    void selectHandle(int handle);

    /// \brief clear selection and mark given handle as selected
    void selectTwoHandles(int firstHandle, int secondHandle);

protected Q_SLOTS:
    /// \brief recalculate maximum count and cure extra handles from right
    void refreshMaxCount();

    /// \brief check that old positions not outside range
    void onRangeChanged(int min, int max);

protected:
    MultiSlider( MultiSliderPrivate* impl, Qt::Orientation o, QWidget* par = 0 );
    MultiSlider( MultiSliderPrivate* impl, QWidget* par = 0 );

    /// \brief normalize positions
    void normalize(bool emitIfChanged);

    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseMoveEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev) override;

    virtual void paintEvent(QPaintEvent* ev) override;
    virtual void initSliderStyleOption(int num, QStyleOptionSlider* option) const;

    virtual bool event(QEvent* event) override;

protected:
    QScopedPointer<QObject> d_ptr;

private:
    void movePosition(int index, int arg);
    void drawColoredRect(int pos, int nextPos, QStylePainter &painter, QColor highlight);

    Q_DECLARE_PRIVATE(MultiSlider)
    Q_DISABLE_COPY(MultiSlider)
};

#endif //__MULTISLIDER_H__

