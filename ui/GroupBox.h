#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <QWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

class GroupBox : public QWidget
{
    Q_OBJECT
public:
    explicit GroupBox(QString title, bool linear = false, QWidget *parent = nullptr);

    bool isChecked();
    void setChecked(bool checked);

    void addHeaderWidget(QWidget *w);
    void addWidget(QWidget *w);
    void addHeaderSpacing(int spacing);

signals:
    void toggled(bool on);

protected:
    bool m_linear = false;
    QCheckBox *m_check = nullptr;
    QVBoxLayout *m_contentLayout = nullptr;
    QHBoxLayout *m_headerLayout = nullptr;
    QList<QWidget*> m_widgets;

    void manageWidgetsEnabled();
};

#endif // GROUPBOX_H
