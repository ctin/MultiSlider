#ifndef MULTISLIDERWIDGET_H
#define MULTISLIDERWIDGET_H

#include <QFrame>

class MultiSlider;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QEvemt;
class QSignalMapper;

class MultiSliderWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool labelsUnder READ labelsUnder WRITE setLabelsUnder NOTIFY labelsUnderChanged)
    Q_PROPERTY(bool showPositions READ showPositions WRITE setShowPositions NOTIFY showPositionsChanged)
    Q_PROPERTY(bool showDifferences READ showDifferences WRITE setShowDifferences NOTIFY showDifferencesChanged)

public:
    MultiSliderWidget(QWidget *parent = 0);
    MultiSlider* GetMultiSlider();
    QPushButton* GetButtonAddToLeft();
    QPushButton* GetButtonAddToRight();
    QPushButton* GetButtonRemoveFromLeft();
    QPushButton* GetButtonRemoveFromRight();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    class SpinBox;
    SpinBox* createSpinBox(int index);
    void initMultiSlider();
    void createWidget();
    void updateSpinBoxValue(int i);
    void selectNextSpinBox();
    void onLabelsUnderChanged();

private slots:
    void updateButtonsEnable();
    void onSliderCountChanged(int count);
    void onSelectedHandleChanged(int handle);
    void onSliderPositionsChanged(QVector<int> values);
    void onSliderRangeChanged(int min, int max);
    void onSpinBoxValueChanged(int value);

private:
    QHBoxLayout *labelsLayout;
    QVBoxLayout *sliderLayout;
    QHBoxLayout *widgetLayout;
    MultiSlider *multiSlider;
    QList<SpinBox*> spinBoxes;
    QPushButton *addToLeftButton;
    QPushButton *addToRightButton;
    QPushButton *removeFromLeftButton;
    QPushButton *removeFromRightButton;

    //properties section
public:
    bool labelsUnder() const;
    bool showPositions() const;
    bool showDifferences() const;

public slots:
    void setLabelsUnder(bool arg);
    void setShowPositions(bool arg);
    void setShowDifferences(bool arg);
    
signals:
    void labelsUnderChanged(bool arg);
    void showPositionsChanged(bool arg);
    void showDifferencesChanged(bool arg);
    
private:
    bool m_labelsUnder = false;
    bool m_showDifferences = true;
};

#endif // MULTISLIDERWIDGET_H
