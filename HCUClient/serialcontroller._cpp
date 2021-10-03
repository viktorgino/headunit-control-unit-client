#include "serialcontroller.h"

QString keyToString(Keys key) {
    switch (key) {
    case Key_1:
        return "1";
    case Key_2:
        return "2";
    case Key_3:
        return "3";
    case Key_4:
        return "4";
    case Key_5:
        return "5";
    case Key_6:
        return "6";
    case Key_7:
        return "7";
    case Key_8:
        return "8";
    case Key_9:
        return "9";
    case Key_0:
        return "0";
    case Key_Enter:
        return "Enter";
    case Key_Exit:
        return "Exit";
    case Key_Back:
        return "Back";
    case Key_Menu:
        return "Menu";
    case Key_Power:
        return "Power";
    case Key_Left:
        return "Left";
    case Key_Right:
        return "Right";
    case Key_Up:
        return "Up";
    case Key_Down:
        return "Down";
    case Key_VolumeUp:
        return "VolumeUp";
    case Key_VolumeDown:
        return "VolumeDown";
    case Key_TuneUp:
        return "TuneUp";
    case Key_TuneDown:
        return "TuneDown";
    case Key_Sound:
        return "Sound";
    case Key_AMFM:
        return "AMFM";
    case Key_CD:
        return "CD";
    case Key_Eject:
        return "Eject";
    case Key_Auto:
        return "Auto";
    case Key_Scan:
        return "Scan";
    }
}
SerialController::SerialController(QObject *parent) : QObject(parent), m_connected(false), m_serialInput(""), m_acControlFrame()
{
    updatePorts();
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialController::handleReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialController::handleError);
}

void SerialController::updatePorts() {
    m_ports.clear();
    for(const QSerialPortInfo &port : QSerialPortInfo::availablePorts()){
        if(port.vendorIdentifier() == 0x16C0 && port.productIdentifier() == 0x0483)
            m_ports.append(port.portName());
    }
    emit portsUpdated();
}

void SerialController::connectSerial(QString port, QString baud){
    m_serial.setPortName(port);
    m_serial.setBaudRate(baud.toInt());
    if (!m_serial.open(QIODevice::ReadWrite)) {
        qDebug() << QObject::tr("Failed to open port %1, error: %2")
                    .arg(port, m_serial.errorString())
                 << "\n";
    } else {
        m_connected = true;
        emit connectedUpdated();
        m_serial.write("update\n");
    }
}

void SerialController::disconnectSerial(){
    m_serial.close();
    m_connected = false;

    emit connectedUpdated();
}

void SerialController::handleReadyRead(){
    while(m_serial.canReadLine() > 0){
        QString line = QString(m_serial.readLine()).trimmed();
        if(line.length() > 4){ //Make sure we have msg id and len bytes set
            uint8_t id = line.leftRef(2).toUInt(nullptr, 16);
            uint8_t len = line.midRef(2,2).toUInt(nullptr, 16);
            QString dataBuffer = line.right(line.length() - 4);
            if(dataBuffer.length() != len * 2){
                qWarning() << "Invalid data buffer length";
                qDebug() << dataBuffer;
                continue;
            }
            switch (id) {
            case ButtonInputCommand: {
                Keys key = (Keys)dataBuffer.leftRef(2).toUInt(nullptr, 16);
                QString logLine = QString("%1 button pressed").arg(keyToString(key));
                emit logLineReceived(logLine);
            }
                break;
            case ClimateControlCommand: {
                uint8_t dirByte = dataBuffer.leftRef(2).toUInt(nullptr, 16);

                m_acControlFrame["DirectionFrontDown"] = (dirByte & (1<<0));
                m_acControlFrame["DirectionFrontCenter"] = (dirByte & (1<<1));
                m_acControlFrame["DirectionFrontUp"] = (dirByte & (1<<2));
                m_acControlFrame["DirectionFrontWindscreen"] = (dirByte & (1<<3));
                m_acControlFrame["DirectionRearDown"] = (dirByte & (1<<4));
                m_acControlFrame["DirectionRearCenter"] = (dirByte & (1<<5));
                m_acControlFrame["DirectionRearUp"] = (dirByte & (1<<6));
                m_acControlFrame["DirectionRearWindscreen"] = (dirByte & (1<<7));
                m_acControlFrame["FanFront"] = dataBuffer.midRef(2,2).toUInt(nullptr, 16);
                m_acControlFrame["FanRear"] = dataBuffer.midRef(4,2).toUInt(nullptr, 16);
                m_acControlFrame["TempFrontLeft"] = dataBuffer.midRef(6,2).toUInt(nullptr, 16);
                m_acControlFrame["TempFrontRight"] = dataBuffer.midRef(8,2).toUInt(nullptr, 16);
                m_acControlFrame["TempRearLeft"] = dataBuffer.midRef(10,2).toUInt(nullptr, 16);
                m_acControlFrame["TempRearRight"] = dataBuffer.midRef(12,2).toUInt(nullptr, 16);
                uint8_t extrasByte = dataBuffer.midRef(14,2).toUInt(nullptr, 16);
                m_acControlFrame["ProgAuto"] = (extrasByte & (1<<0));
                m_acControlFrame["ProgAutoFan"] = (extrasByte & (1<<1));
                m_acControlFrame["ProgAutoDir"] = (extrasByte & (1<<2));
                emit ACControlFrameUpdated();
                break;
            }
            case CustomCommand: {
                uint8_t byte1 = dataBuffer.leftRef(2).toUInt(nullptr, 16);
                m_customCommandFrame["Bit1"] = (byte1 & (1<<0));
                m_customCommandFrame["Bit2"] = (byte1 & (1<<1));
                m_customCommandFrame["Bit3"] = (byte1 & (1<<2));
                m_customCommandFrame["Bit4"] = (byte1 & (1<<3));
                m_customCommandFrame["Bit5"] = (byte1 & (1<<4));
                m_customCommandFrame["Bit6"] = (byte1 & (1<<5));
                m_customCommandFrame["Bit7"] = (byte1 & (1<<6));
                m_customCommandFrame["Bit8"] = (byte1 & (1<<7));

                uint8_t byte2 = dataBuffer.midRef(2,2).toUInt(nullptr, 16);
                m_customCommandFrame["Bit9"] = (byte2 & (1<<0));
                m_customCommandFrame["Bit10"] = (byte2 & (1<<1));
                m_customCommandFrame["Bit11"] = (byte2 & (1<<2));
                m_customCommandFrame["Bit12"] = (byte2 & (1<<3));
                m_customCommandFrame["Bit13"] = (byte2 & (1<<4));
                m_customCommandFrame["Bit14"] = (byte2 & (1<<5));
                m_customCommandFrame["Bit15"] = (byte2 & (1<<6));
                m_customCommandFrame["Bit16"] = (byte2 & (1<<7));

                m_customCommandFrame["Byte1"] = dataBuffer.midRef(4,2).toUInt(nullptr, 16);
                m_customCommandFrame["Byte2"] = dataBuffer.midRef(6,2).toUInt(nullptr, 16);
                m_customCommandFrame["Byte3"] = dataBuffer.midRef(8,2).toUInt(nullptr, 16);
                m_customCommandFrame["Byte4"] = dataBuffer.midRef(10,2).toUInt(nullptr, 16);
                m_customCommandFrame["Byte5"] = dataBuffer.midRef(12,2).toUInt(nullptr, 16);
                m_customCommandFrame["Byte6"] = dataBuffer.midRef(14,2).toUInt(nullptr, 16);
                CustomCommandFrameUpdated();
                qDebug() << id << len << dataBuffer.toLocal8Bit().toHex(' ');
                break;
            }
            case WriteBackCommand:{
                emit logLineReceived(dataBuffer);
                break;
            }
            default:
                qDebug() << id << len << dataBuffer.toLocal8Bit().toHex(' ');
                emit logLineReceived(line);
                break;
            }
        } else {
            qDebug() << "Invalid message";
            qDebug() << line;
        }
    }
}

void SerialController::setCustomCommandBit(int bitNumber, bool value){
    switch (bitNumber) {
    case 1:
        m_commandFrame.CustomCommandBit1 = value;
        break;
    case 2:
        m_commandFrame.CustomCommandBit2 = value;
        break;
    case 3:
        m_commandFrame.CustomCommandBit3 = value;
        break;
    case 4:
        m_commandFrame.CustomCommandBit4 = value;
        break;
    case 5:
        m_commandFrame.CustomCommandBit5 = value;
        break;
    case 6:
        m_commandFrame.CustomCommandBit6 = value;
        break;
    case 7:
        m_commandFrame.CustomCommandBit7 = value;
        break;
    case 8:
        m_commandFrame.CustomCommandBit8 = value;
        break;
    case 9:
        m_commandFrame.CustomCommandBit9 = value;
        break;
    case 10:
        m_commandFrame.CustomCommandBit10 = value;
        break;
    case 11:
        m_commandFrame.CustomCommandBit11 = value;
        break;
    case 12:
        m_commandFrame.CustomCommandBit12 = value;
        break;
    case 13:
        m_commandFrame.CustomCommandBit13 = value;
        break;
    case 14:
        m_commandFrame.CustomCommandBit14 = value;
        break;
    case 15:
        m_commandFrame.CustomCommandBit15 = value;
        break;
    case 16:
        m_commandFrame.CustomCommandBit16 = value;
        break;
    }
    sendCustomCommandFrame();
}
void SerialController::setCustomCommandByte(int byteNumber, int value){
    switch (byteNumber) {
    case 1:
        m_commandFrame.CustomCommandByte1 = value;
        break;
    case 2:
        m_commandFrame.CustomCommandByte2 = value;
        break;
    case 3:
        m_commandFrame.CustomCommandByte3 = value;
        break;
    case 4:
        m_commandFrame.CustomCommandByte4 = value;
        break;
    case 5:
        m_commandFrame.CustomCommandByte5 = value;
        break;
    case 6:
        m_commandFrame.CustomCommandByte6 = value;
        break;
    }
    sendCustomCommandFrame();
}

QString SerialController::printHex(uint8_t numByte)
{
    QString ret;
    if (numByte < 16)
    {
        ret.append('0');
    }
    ret.append(QString::number(numByte,16));
    return ret;
}

void SerialController::sendMessage(CommandTypes messageType, uint8_t length, uint8_t *message)
{
    if(m_connected){
        QByteArray cmd;
        cmd.append(messageType);
        cmd.append(length);
        for (int i = 0; i < length; i++)
        {
            cmd.append(message[i]);
        }
        cmd.append("\r\n");
        m_serial.write(cmd);
    }
}

void SerialController::sendCustomCommandFrame(){
    uint8_t CustomCommandBuffer[8];
    CustomCommandBuffer[0] = m_commandFrame.CustomCommandBit1      | m_commandFrame.CustomCommandBit2 << 1 | m_commandFrame.CustomCommandBit3 << 2 |
                             m_commandFrame.CustomCommandBit4 << 3 | m_commandFrame.CustomCommandBit5 << 4 | m_commandFrame.CustomCommandBit6 << 5 |
                             m_commandFrame.CustomCommandBit7 << 6 | m_commandFrame.CustomCommandBit8 << 7;
    CustomCommandBuffer[1] = m_commandFrame.CustomCommandBit9       | m_commandFrame.CustomCommandBit10 << 1 | m_commandFrame.CustomCommandBit11 << 2 |
                             m_commandFrame.CustomCommandBit12 << 3 | m_commandFrame.CustomCommandBit13 << 4 | m_commandFrame.CustomCommandBit14 << 5 |
                             m_commandFrame.CustomCommandBit15 << 6 | m_commandFrame.CustomCommandBit16 << 7;
    CustomCommandBuffer[2] = m_commandFrame.CustomCommandByte1;
    CustomCommandBuffer[3] = m_commandFrame.CustomCommandByte2;
    CustomCommandBuffer[4] = m_commandFrame.CustomCommandByte3;
    CustomCommandBuffer[5] = m_commandFrame.CustomCommandByte4;
    CustomCommandBuffer[6] = m_commandFrame.CustomCommandByte5;
    CustomCommandBuffer[7] = m_commandFrame.CustomCommandByte6;

    sendMessage(CustomCommand, 8, CustomCommandBuffer);

}
void SerialController::handleError(QSerialPort::SerialPortError error){
    if(error > 0){
        disconnectSerial();
        qDebug() << "Error : " << error;
    }
}

