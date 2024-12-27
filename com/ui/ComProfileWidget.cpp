#include "ComProfileWidget.h"
#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>
#include "classes/Common.h"
#include <QLineEdit>
#include "com/ui/ComDeviceStatusWidget.h"
#include <QSpinBox>

ComProfileWidget::ComProfileWidget(ComDevice *com, QWidget *parent) : QWidget(parent)
{
    if (!com) return;

    m_profile = com->profile();
    if (!m_profile) return;

    // Port selector
    auto comboPorts = new QComboBox();

    auto func_setPort =[=]() {
        auto port = comboPorts->currentData().toString();
        m_profile->setPort(port);
    };

    connect(comboPorts, &QComboBox::currentIndexChanged, func_setPort);

    auto func_fillPorts = [=]() {
        comboPorts->blockSignals(true);
        comboPorts->clear();
        QString lastPort = m_profile->port();
        auto infos = QSerialPortInfo::availablePorts();
        foreach (auto p, infos) {
            auto name = p.portName();
            comboPorts->addItem(p.portName() + " [" + p.description() + "]", name);
            if (name == lastPort) {
                comboPorts->setCurrentIndex(comboPorts->count() - 1);
            }
        }
        if (!comboPorts->currentText().isEmpty()) {
            func_setPort();
        }
        comboPorts->blockSignals(false);
    };

    auto btnRefresh = new QPushButton(QCoreApplication::translate("Generic", "Refresh"));
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
    comboBaudRate->setCurrentText(QString::number(m_profile->baudRate()));
    connect(comboBaudRate, &QComboBox::currentTextChanged, [=]() {
        m_profile->setBaudRate(comboBaudRate->currentText().toInt());
    });


    // Parity
    auto comboParity = new QComboBox();
    comboParity->setMaximumWidth(200);
    auto parities = Common::EnumIntValues<QSerialPort::Parity>();
    auto selectedParity = m_profile->parity();
    foreach (auto parity, parities) {
        comboParity->addItem(Common::EnumToString<QSerialPort::Parity>((QSerialPort::Parity)parity), parity);
        if (selectedParity == parity) {
            comboParity->setCurrentIndex(comboParity->count() - 1);
        }
    }
    connect(comboParity, &QComboBox::currentTextChanged, [=]() {
        m_profile->setParity((QSerialPort::Parity)comboParity->currentData().toInt());
    });

    // DataBits
    auto comboDataBits = new QComboBox();
    comboDataBits->setMaximumWidth(200);
    auto dataBits = Common::EnumIntValues<QSerialPort::DataBits>();

    foreach (auto bits, dataBits) {
        comboDataBits->addItem(QString::number(bits), bits);
    }
    comboDataBits->setCurrentText(QString::number(m_profile->dataBits()));
    connect(comboDataBits, &QComboBox::currentTextChanged, [=]() {
        m_profile->setDataBits(comboDataBits->currentText().toInt());
    });

    // Timeout
    auto spinTimeout = new QSpinBox();
    spinTimeout->setRange(0, 10000);
    spinTimeout->setSuffix(" " + QCoreApplication::translate("Generic", "ms"));
    spinTimeout->setMaximumWidth(200);
    spinTimeout->setValue(m_profile->timeoutMs());
    connect(spinTimeout, &QSpinBox::valueChanged, [=]() {
        m_profile->setTimeoutMs(spinTimeout->value());
    });

    // Speed format
    auto speedFormatter = new QWidget();
    speedFormatter->setContentsMargins(0,0,0,0);
    auto laySpeedFormat = new QHBoxLayout(speedFormatter);
    laySpeedFormat->setContentsMargins(0,0,0,0);
    auto txtPrefix = new QLineEdit(m_profile->speedPrefix());
    txtPrefix->setMinimumWidth(32);
    txtPrefix->setAlignment(Qt::AlignRight);
    connect(txtPrefix, &QLineEdit::editingFinished, [=]() {
        m_profile->setSpeedPrefix(txtPrefix->text());
    });
    auto txtSuffix = new QLineEdit(m_profile->speedSuffix());
    txtSuffix->setMinimumWidth(32);
    connect(txtSuffix, &QLineEdit::editingFinished, [=]() {
        m_profile->setSpeedPrefix(txtSuffix->text());
    });
    auto labelN = new QLabel("-100...100");
    labelN->setStyleSheet("QLabel { color: #159 }");
    laySpeedFormat->addWidget(txtPrefix, 0);
    laySpeedFormat->addWidget(labelN, 0);
    laySpeedFormat->addWidget(txtSuffix, 0);
    laySpeedFormat->addStretch(1);

    auto grid = new QGridLayout();
    auto func_addRow = [=](QString title, QWidget *editor = nullptr) {
        int row = grid->rowCount();
        auto label = new QLabel(title);
        grid->addWidget(label, row, 0);
        if (editor) grid->addWidget(editor, row, 1);
        else label->setStyleSheet("font-weight: bold");
    };

    auto func_commandRow = [=](QString commandKey, QString commandTitle) {
        auto txt = new QLineEdit(m_profile->command(commandKey));
        txt->setPlaceholderText(QCoreApplication::translate("Generic", "Enter command"));
        connect(txt, &QLineEdit::editingFinished, [=]() {
            m_profile->setCommand(commandKey, txt->text());
        });
        func_addRow(commandTitle, txt);
        return txt;
    };

    func_addRow(QCoreApplication::translate("Generic", "Port settings"));

    func_addRow(QCoreApplication::translate("Generic", "Port"), portWidget);
    func_addRow(QCoreApplication::translate("Generic", "BaudRate"), comboBaudRate);
    func_addRow(QCoreApplication::translate("Generic", "Parity"), comboParity);
    func_addRow(QCoreApplication::translate("Generic", "DataBits"), comboDataBits);
    func_addRow(QCoreApplication::translate("Generic", "Timeout"), spinTimeout);

    grid->addWidget(new ComDeviceStatusWidget(com), 2, 1, 5, 1, Qt::AlignRight|Qt::AlignBottom);

    func_addRow("");
    func_addRow(QCoreApplication::translate("Generic", "Commands"));
    func_addRow(QCoreApplication::translate("Generic", "Speed format"), speedFormatter);

    foreach (auto command, ComDevice::Commands()) {
        func_commandRow(command.key, (command.symbol.isEmpty() ? "" : "[" + command.symbol + "] ") + command.title);
    }
    grid->setColumnStretch(1, 100);

    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(10, 10, 10, 10);
    vbox->addLayout(grid, 0);
    vbox->addStretch(100);
}
