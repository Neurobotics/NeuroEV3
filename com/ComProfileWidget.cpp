#include "ComProfileWidget.h"
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include "classes/Common.h"

ComProfileWidget::ComProfileWidget(ComProfile *profile, QWidget *parent) : QWidget(parent)
{
    if (!profile) return;

    m_profile = profile;

    // Port selector
    auto comboPorts = new QComboBox();
    connect(comboPorts, &QComboBox::currentIndexChanged, [=]() {
        auto port = comboPorts->currentData().toString();
        profile->setPort(port);
    });

    auto func_fillPorts = [=]() {
        comboPorts->blockSignals(true);
        comboPorts->clear();
        QString lastPort = profile->getPort();
        auto infos = QSerialPortInfo::availablePorts();
        foreach (auto p, infos) {
            auto name = p.portName();
            comboPorts->addItem(p.portName() + " [" + p.description() + "]", name);
            if (name == lastPort) {
                comboPorts->setCurrentIndex(comboPorts->count() - 1);
            }
        }
        comboPorts->blockSignals(false);
    };

    auto btnRefresh = new QPushButton(tr("Refresh"));
    connect(btnRefresh, &QPushButton::clicked, this, func_fillPorts);

    auto portWidget = new QWidget();
    portWidget->setContentsMargins(0,0,0,0);
    auto portWidgetLayout = new QHBoxLayout(portWidget);
    portWidgetLayout->setContentsMargins(0,0,0,0);
    portWidgetLayout->addWidget(comboPorts, 100);
    portWidgetLayout->addWidget(btnRefresh);

    func_fillPorts();

    // Baud rate
    auto comboBaudRate = new QComboBox();
    comboBaudRate->setMaximumWidth(200);
    auto baudRates = Common::EnumIntValues<QSerialPort::BaudRate>();
    baudRates.insert(baudRates.count() - 1, 100000);
    baudRates << (QSerialPort::Baud115200 * 2);
    baudRates << 250000;
    baudRates << (QSerialPort::Baud115200 * 4);
    baudRates << 500000;
    foreach (auto rate, baudRates) {
        comboBaudRate->addItem(QString::number(rate), rate);
    }
    comboBaudRate->setCurrentText(QString::number(profile->baudRate()));
    connect(comboBaudRate, &QComboBox::currentTextChanged, [=]() {
        profile->setBaudRate(comboBaudRate->currentText().toInt());
    });


    // Parity
    auto comboParity = new QComboBox();
    comboParity->setMaximumWidth(200);
    auto parities = Common::EnumIntValues<QSerialPort::Parity>();
    auto selectedParity = profile->parity();
    foreach (auto parity, parities) {
        comboParity->addItem(Common::EnumToString<QSerialPort::Parity>((QSerialPort::Parity)parity), parity);
        if (selectedParity == parity) {
            comboParity->setCurrentIndex(comboParity->count() - 1);
        }
    }
    connect(comboParity, &QComboBox::currentTextChanged, [=]() {
        profile->setParity((QSerialPort::Parity)comboParity->currentData().toInt());
    });

    // DataBits
    auto comboDataBits = new QComboBox();
    comboDataBits->setMaximumWidth(200);
    auto dataBits = Common::EnumIntValues<QSerialPort::DataBits>();

    foreach (auto bits, dataBits) {
        comboDataBits->addItem(QString::number(bits), bits);
    }
    comboDataBits->setCurrentText(QString::number(profile->dataBits()));
    connect(comboDataBits, &QComboBox::currentTextChanged, [=]() {
        profile->setDataBits(comboDataBits->currentText().toInt());
    });

    auto func_commandRow = [=](QString commandKey, QString commandTitle) {

    };

    auto grid = new QGridLayout();
    int row = 0;
    auto func_addRow = [=](QString title, QWidget *editor, int &r) {
        grid->addWidget(new QLabel(title), r, 0);
        grid->addWidget(editor, r, 1);
        r++;
    };
    func_addRow(QCoreApplication::translate("Generic", "Port"), portWidget, row);
    func_addRow(QCoreApplication::translate("Generic", "BaudRate"), comboBaudRate, row);
    func_addRow(QCoreApplication::translate("Generic", "Parity"), comboParity, row);
    func_addRow(QCoreApplication::translate("Generic", "DataBits"), comboDataBits, row);

    grid->setColumnStretch(1, 100);

    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(10, 10, 10, 10);
    vbox->addLayout(grid, 0);
    vbox->addStretch(100);
}
