#include "DeviceMultiplayerControl.h"
#include "ui/UICommon.h"
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCoreApplication>

DeviceMultiplayerControl::DeviceMultiplayerControl(Settings *settings, QWidget *parent) : QWidget{parent}
{
    m_settings = settings;

    auto progressMeditationU1 = UICommon::meditationBar();
    auto progressConcentrationU1 = UICommon::concentrationBar();
    auto progressMeditationU2 = UICommon::meditationBar();
    auto progressConcentrationU2 = UICommon::concentrationBar();
    progressMeditationU2->setLayoutDirection(Qt::RightToLeft);
    progressConcentrationU2->setLayoutDirection(Qt::RightToLeft);

    auto progressMeditationMix1 = UICommon::meditationBar();
    auto progressMeditationMix2 = UICommon::meditationBar();
    auto progressConcentrationMix1 = UICommon::concentrationBar();
    auto progressConcentrationMix2 = UICommon::concentrationBar();
    progressConcentrationMix1->setLayoutDirection(Qt::RightToLeft);
    progressMeditationMix1->setLayoutDirection(Qt::RightToLeft);


    auto multipleControlCombo = new QComboBox();
    auto multipleControlComboIndex = m_settings->getMultiplayerControl() == "meditation" ? 0 : 1;
    multipleControlCombo->addItem(QCoreApplication::translate("Generic", "Meditation"));
    multipleControlCombo->addItem(QCoreApplication::translate("Generic", "Concentration"));

    m_meditationBars << progressMeditationU1;
    m_meditationBars << progressMeditationU2;
    m_meditationBars << progressMeditationMix1;
    m_meditationBars << progressMeditationMix2;

    m_concentrationBars << progressConcentrationU1;
    m_concentrationBars << progressConcentrationU2;
    m_concentrationBars << progressConcentrationMix1;
    m_concentrationBars << progressConcentrationMix2;


    auto grid = new QGridLayout();
    grid->addWidget(new QLabel(QCoreApplication::translate("Generic", "User") + "1"), 0, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(new QLabel(QCoreApplication::translate("Generic", "Control")),    0, 1, 1, 2, Qt::AlignCenter);
    grid->addWidget(new QLabel(QCoreApplication::translate("Generic", "User") + "2"), 0, 3, 1, 1, Qt::AlignRight);

    grid->addWidget(progressMeditationU1,     1, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(progressConcentrationU1,  1, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(multipleControlCombo,     1, 1, 1, 2, Qt::AlignCenter);
    grid->addWidget(progressMeditationU2,     1, 3, 1, 1, Qt::AlignLeft);
    grid->addWidget(progressConcentrationU2,  1, 3, 1, 1, Qt::AlignLeft);

    grid->addWidget(progressMeditationMix1,    2, 1, 1, 1, Qt::AlignLeft);
    grid->addWidget(progressMeditationMix2,    2, 2, 1, 1, Qt::AlignLeft);
    grid->addWidget(progressConcentrationMix1, 2, 1, 1, 1, Qt::AlignLeft);
    grid->addWidget(progressConcentrationMix2, 2, 2, 1, 1, Qt::AlignLeft);

    grid->setColumnStretch(0, 100);
    grid->setColumnStretch(1, 100);
    grid->setColumnStretch(2, 100);
    grid->setColumnStretch(3, 100);

    auto func_mode = [=]() {
        m_useMeditation = multipleControlCombo->currentIndex() == 0;
        m_settings->setMultiplayerControl(m_useMeditation ? "meditation" : "concentration");

        foreach (auto p, m_meditationBars) {
            p->setVisible(m_useMeditation);
        }

        foreach (auto p, m_concentrationBars) {
            p->setVisible(!m_useMeditation);
        }
    };

    multipleControlCombo->setCurrentIndex(multipleControlComboIndex);
    connect(multipleControlCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, func_mode);
    func_mode();

    m_labelMix = new QLabel();
    m_labelMix->setStyleSheet("font-weight: bold");

    m_layout = new QVBoxLayout(this);
    m_layout->addLayout(grid, 0);
    m_layout->addWidget(m_labelMix, 0, Qt::AlignCenter);
}

void DeviceMultiplayerControl::setValues(QPair<float, float> meditation, QPair<float, float> concentration)
{
    m_lastMeditation = meditation;
    m_lastConcentration = concentration;

    m_meditationBars[0]->setValue(meditation.first);
    m_meditationBars[1]->setValue(meditation.second);

    m_concentrationBars[0]->setValue(concentration.first);
    m_concentrationBars[1]->setValue(concentration.second);

    // Mix
    m_meditationMix = qBound(-100.f, meditation.first - meditation.second, 100.f);
    m_concentrationMix = qBound(-100.f, concentration.first - concentration.second, 100.f);

    m_meditationBars[2]->setValue(m_meditationMix < 0 ? -m_meditationMix : 0);
    m_meditationBars[3]->setValue(m_meditationMix > 0 ? m_meditationMix : 0);

    m_concentrationBars[2]->setValue(m_concentrationMix < 0 ? -m_concentrationMix : 0);
    m_concentrationBars[3]->setValue(m_concentrationMix > 0 ? m_concentrationMix : 0);

    m_labelMix->setText(QString::number(m_useMeditation ? m_meditationMix : m_concentrationMix, 'f', 0) + "%");

    onSetValues();
}

void DeviceMultiplayerControl::addWidget(QWidget *widget, Qt::Alignment align)
{
    if (m_layout) m_layout->addWidget(widget, 100, align);
}
