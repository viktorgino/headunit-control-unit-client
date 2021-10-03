#ifndef HCUCONTROLLER_H
#define HCUCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QQmlPropertyMap>
#include "HCUcrl/HUDSerial/HUDSerial.h"


class HCUController : public QObject, PlatformCallbacks
{
    Q_OBJECT
    Q_PROPERTY(QStringList ports READ getPorts NOTIFY portsUpdated)
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedUpdated)
    Q_PROPERTY(QVariantMap ClimateControlFrame READ getClimateControlFrame NOTIFY ClimateControlFrameUpdated)
    Q_PROPERTY(QVariantList CustomCommandBits READ getCustomCommandBits NOTIFY CustomCommandFrameUpdated)
    Q_PROPERTY(QVariantList CustomCommandBytes READ getCustomCommandBytes NOTIFY CustomCommandFrameUpdated)
    Q_PROPERTY(QVariantMap BodyControlCommandFrame READ getBodyControlCommandFrame NOTIFY BodyControlCommandFrameUpdated)
    Q_PROPERTY(QVariantMap DriveTrainControlCommandFrame READ getDriveTrainControlCommandFrame NOTIFY DriveTrainControlCommandFrameUpdated)
public:
    explicit HCUController(QObject *parent = nullptr);

    void ClimateControlCallback(const ClimateControlCommandFrame &) override;
    void CustomCommandCallback(const CustomCommandFrame&) override;
    void ButtonInputCommandCallback(Keys) override;
    void SendMessageCallback(uint8_t) override;
    void PrintString(char * message, int len) override;
    void BodyControlCommandCallback(const BodyControlCommandFrame&) override;
    void DriveTrainControlCommandCallback(const DriveTrainControlCommandFrame&) override;

signals:
    void portsUpdated();
    void connectedUpdated();
    void ClimateControlFrameUpdated();
    void CustomCommandFrameUpdated();
    void DriveTrainControlCommandFrameUpdated();
    void BodyControlCommandFrameUpdated();
    void logLineReceived(QString logLine);

public slots:
    void setParameter(QString parameter, bool value);
    void setZoneParameter(QString zone, QString parameter, QVariant value);
    void connectSerial(QString port, QString baud);
    void disconnectSerial();
    void setCustomCommandBit(int bitNumber, bool value);
    void setCustomCommandByte(int byteNumber, int value);
    void setBodyControlParameter(QString parameter, QVariant value);
    void setDriveTrainParameter(QString parameter, QVariant value);

    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial;
    QStringList m_ports;
    bool m_connected;
    void updatePorts();
    HUDSerial::HUDSerial m_hudSerial;
    CustomCommandFrame m_commandFrame;
    ClimateControlCommandFrame m_ccCommandFrame;

    BodyControlCommandFrame m_bccFrame;
    DriveTrainControlCommandFrame m_dtcFrame;

    QVariantMap m_climateControlFrame;
    QVariantList m_customCommandFrameBits;
    QVariantList m_customCommandFrameBytes;

    QVariantMap m_driveTrainControlCommandFrame;
    QVariantMap m_bodyControlCommandFrame;

    QStringList getPorts() {
        return m_ports;
    }
    bool getConnected(){
        return m_connected;
    }
    QVariantMap getClimateControlFrame(){
        return m_climateControlFrame;
    }
    QVariantList getCustomCommandBits(){
        return m_customCommandFrameBits;
    }
    QVariantList getCustomCommandBytes(){
        return m_customCommandFrameBytes;
    }
    QVariantMap getDriveTrainControlCommandFrame(){
        return m_driveTrainControlCommandFrame;
    }
    QVariantMap getBodyControlCommandFrame(){
        return m_bodyControlCommandFrame;
    }
};

#endif // HCUCONTROLLER_H
