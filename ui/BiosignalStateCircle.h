#ifndef BIOSIGNALSTATECIRCLE_H
#define BIOSIGNALSTATECIRCLE_H

#include <QWidget>

class BiosignalStateCircle : public QWidget
{
    Q_OBJECT
public:
    explicit BiosignalStateCircle(int state, bool filled = false, QSize size = QSize(24,24), QWidget *parent = nullptr);

    void setState(int state);
    void setFilled(bool filled);
    void setStateAndFilled(int state, bool filled);

protected:
    int m_state = -1;
    bool m_filled = false;
    QString m_stateStr = "";

    void paintEvent(QPaintEvent *) override;
};

#endif // BIOSIGNALSTATECIRCLE_H
