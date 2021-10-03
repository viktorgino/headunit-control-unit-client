#include "hcucontroller.h"

HCUController::HCUController(QObject *parent) : QObject(parent) , m_hudSerial(), m_connected(false)
{
    m_hudSerial.setCallbacks(this);

    for(int i = 0; i < 16; i++){
        m_customCommandFrameBits.append(false);
    }
    for(int i = 0; i < 6; i++){
        m_customCommandFrameBytes.append(0);
    }
    updatePorts();
    connect(&m_serial, &QSerialPort::readyRead, this, &HCUController::handleReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &HCUController::handleError);
    ClimateControlCallback(m_ccCommandFrame);
}

void HCUController::handleError(QSerialPort::SerialPortError error){
    if(error > 0){
        disconnectSerial();
        qDebug() << "Error : " << error;
    }
}

void HCUController::handleReadyRead(){
    uint8_t buffer;
    while(m_serial.read((char*)&buffer, 1)){
        uint8_t rec = static_cast<uint8_t>(buffer);
        m_hudSerial.receiveByte(rec);
    }

}
void HCUController::updatePorts() {
    m_ports.clear();
    for(const QSerialPortInfo &port : QSerialPortInfo::availablePorts()){
        //        if(port.vendorIdentifier() == 0x16C0 && port.productIdentifier() == 0x0483)
        m_ports.append(port.portName());
    }
    emit portsUpdated();
}

void HCUController::connectSerial(QString port, QString baud){
    m_serial.setPortName(port);
    m_serial.setBaudRate(baud.toInt());
    if (!m_serial.open(QIODevice::ReadWrite)) {
        qDebug() << QObject::tr("Failed to open port %1, error: %2")
                    .arg(port, m_serial.errorString())
                 << "\n";
    } else {
        qDebug() << "Connected to serial : " << port << baud;
        m_connected = true;
        m_hudSerial.sendUpdateRequest();
        emit connectedUpdated();
    }
}
void HCUController::disconnectSerial(){
    if(m_serial.isOpen()){
        m_serial.close();
    }
    m_connected = false;
    emit connectedUpdated();
}

void HCUController::CustomCommandCallback(const CustomCommandFrame &commandFrame){
    for(int i = 0; i < 16; i++){
        m_customCommandFrameBits[i] = commandFrame.Bits[i];
    }
    for(int i = 0; i < 6; i++){
        m_customCommandFrameBytes[i] = commandFrame.Bytes[i];
    }
    emit CustomCommandFrameUpdated();
}

void HCUController::setParameter(QString parameter, bool value){
    if(parameter == "TempSelectLeft"){
        m_ccCommandFrame.TempSelectLeft = value;
    } else if(parameter == "TempSelectRight"){
        m_ccCommandFrame.TempSelectRight = value;
    } else if(parameter == "FanSelectLeft"){
        m_ccCommandFrame.FanSelectLeft = value;
    } else if(parameter == "FanSelectRight"){
        m_ccCommandFrame.FanSelectRight = value;
    } else if(parameter == "ProgAuto"){
        m_ccCommandFrame.ProgAuto = value;
    } else if(parameter == "ProgAutoFanFront"){
        m_ccCommandFrame.ProgAutoFanFront = value;
    } else if(parameter == "ProgAutoFanRear"){
        m_ccCommandFrame.ProgAutoFanRear = value;
    } else if(parameter == "ProgWindscreen"){
        m_ccCommandFrame.ProgWindscreen = value;
    } else if(parameter == "Recirculate"){
        m_ccCommandFrame.Recirculate = value;
    } else if(parameter == "RearDefrost"){
        m_ccCommandFrame.RearDefrost = value;
    } else if(parameter == "AC"){
        m_ccCommandFrame.AC = value;
    }
    m_hudSerial.sendClimateControlCommand(m_ccCommandFrame);
}

void HCUController::setZoneParameter(QString zone, QString parameter, QVariant value) {
    ClimateControlZoneControl *zoneControl;

    if(zone == "FrontLeft"){
        zoneControl = &m_ccCommandFrame.Front.Left;
    } else if(zone == "FrontRight"){
        zoneControl = &m_ccCommandFrame.Front.Right;
    } else if(zone == "RearLeft"){
        zoneControl = &m_ccCommandFrame.Rear.Left;
    } else if(zone == "RearRight"){
        zoneControl = &m_ccCommandFrame.Rear.Right;
    } else {
        return;
    }

    if(parameter == "Fan"){
        zoneControl->Fan = value.toUInt();
    } else if(parameter == "Temperature"){
        zoneControl->Temperature = value.toUInt();
    }  else if(parameter == "SeatHeating"){
        zoneControl->SeatHeating = value.toUInt();
    } else if(parameter == "DirUp"){
        zoneControl->Direction.Up = value.toBool();
    } else if(parameter == "DirCenter"){
        zoneControl->Direction.Center = value.toBool();
    } else if(parameter == "DirDown"){
        zoneControl->Direction.Down = value.toBool();
    } else if(parameter == "DirAuto"){
        zoneControl->Direction.Auto = value.toBool();
    }
    m_hudSerial.sendClimateControlCommand(m_ccCommandFrame);
}


void HCUController::ClimateControlCallback(const ClimateControlCommandFrame &commandFrame){
    QVariantMap hvacControlArray;
    QVariantMap controlZoneFrontLeft;
    QVariantMap controlZoneFrontRight;
    QVariantMap controlZoneRearLeft;
    QVariantMap controlZoneRearRight;

    controlZoneFrontLeft.insert("DirUp",        commandFrame.Front.Left.Direction.Up);
    controlZoneFrontLeft.insert("DirCenter",    commandFrame.Front.Left.Direction.Center);
    controlZoneFrontLeft.insert("DirDown",      commandFrame.Front.Left.Direction.Down);
    controlZoneFrontLeft.insert("DirAuto",    commandFrame.Front.Left.Direction.Auto);
    controlZoneFrontLeft.insert("Fan",          commandFrame.Front.Left.Fan);
    controlZoneFrontLeft.insert("Temperature",  commandFrame.Front.Left.Temperature);
    controlZoneFrontLeft.insert("SeatHeating",  commandFrame.Front.Left.SeatHeating);

    controlZoneFrontRight.insert("DirUp",        commandFrame.Front.Right.Direction.Up);
    controlZoneFrontRight.insert("DirCenter",    commandFrame.Front.Right.Direction.Center);
    controlZoneFrontRight.insert("DirDown",      commandFrame.Front.Right.Direction.Down);
    controlZoneFrontRight.insert("DirAuto",    commandFrame.Front.Right.Direction.Auto);
    controlZoneFrontRight.insert("Fan",          commandFrame.Front.Right.Fan);
    controlZoneFrontRight.insert("Temperature",  commandFrame.Front.Right.Temperature);
    controlZoneFrontRight.insert("SeatHeating",  commandFrame.Front.Right.SeatHeating);

    controlZoneRearLeft.insert("DirUp",        commandFrame.Rear.Left.Direction.Up);
    controlZoneRearLeft.insert("DirCenter",    commandFrame.Rear.Left.Direction.Center);
    controlZoneRearLeft.insert("DirDown",      commandFrame.Rear.Left.Direction.Down);
    controlZoneRearLeft.insert("DirAuto",    commandFrame.Rear.Left.Direction.Auto);
    controlZoneRearLeft.insert("Fan",          commandFrame.Rear.Left.Fan);
    controlZoneRearLeft.insert("Temperature",  commandFrame.Rear.Left.Temperature);
    controlZoneRearLeft.insert("SeatHeating",  commandFrame.Rear.Left.SeatHeating);

    controlZoneRearRight.insert("DirUp",        commandFrame.Rear.Right.Direction.Up);
    controlZoneRearRight.insert("DirCenter",    commandFrame.Rear.Right.Direction.Center);
    controlZoneRearRight.insert("DirDown",      commandFrame.Rear.Right.Direction.Down);
    controlZoneRearRight.insert("DirAuto",    commandFrame.Rear.Right.Direction.Auto);
    controlZoneRearRight.insert("Fan",          commandFrame.Rear.Right.Fan);
    controlZoneRearRight.insert("Temperature",  commandFrame.Rear.Right.Temperature);
    controlZoneRearRight.insert("SeatHeating",  commandFrame.Rear.Right.SeatHeating);

    m_climateControlFrame.insert("FrontLeft", controlZoneFrontLeft);
    m_climateControlFrame.insert("FrontRight", controlZoneFrontRight);
    m_climateControlFrame.insert("RearLeft", controlZoneRearLeft);
    m_climateControlFrame.insert("RearRight", controlZoneRearRight);

    m_climateControlFrame.insert("TempSelectLeft", commandFrame.TempSelectLeft);
    m_climateControlFrame.insert("TempSelectRight", commandFrame.TempSelectRight);
    m_climateControlFrame.insert("FanSelectLeft", commandFrame.FanSelectLeft);
    m_climateControlFrame.insert("FanSelectRight", commandFrame.FanSelectRight);

    m_climateControlFrame.insert("ProgAuto", commandFrame.ProgAuto);
    m_climateControlFrame.insert("ProgAutoFanFront", commandFrame.ProgAutoFanFront);
    m_climateControlFrame.insert("ProgAutoFanRear", commandFrame.ProgAutoFanRear);
    m_climateControlFrame.insert("ProgWindscreen", commandFrame.ProgWindscreen);
    m_climateControlFrame.insert("Recirculate", commandFrame.Recirculate);
    m_climateControlFrame.insert("RearDefrost", commandFrame.RearDefrost);
    m_climateControlFrame.insert("AC", commandFrame.AC);

    emit ClimateControlFrameUpdated();
}

QString buttonToString(Keys key) {
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
    default:
        return "";
    }
}
void HCUController::ButtonInputCommandCallback(Keys key){
    qDebug() << "ButtonInputCommandCallback";
    QString logLine = QString("%1 button pressed").arg(buttonToString(key));
    emit logLineReceived(logLine);
}
void HCUController::SendMessageCallback(uint8_t receivedByte){
    if(m_serial.isOpen()){
        m_serial.write((char*)&receivedByte, 1);
        m_serial.flush();
    }
}

void HCUController::setCustomCommandBit(int bitNumber, bool value){
    if(bitNumber > 15 || bitNumber < 0){
        qWarning() << "Invalid custom bit number";
        return;
    }
    m_commandFrame.Bits[bitNumber] = value;
    m_hudSerial.sendCustomCommand(m_commandFrame);
}

void HCUController::setCustomCommandByte(int byteNumber, int value){
    if(byteNumber > 5 || byteNumber < 0){
        qWarning() << "Invalid custom byte number";
        return;
    }
    m_commandFrame.Bytes[byteNumber] = value;
    m_hudSerial.sendCustomCommand(m_commandFrame);
}

void HCUController::setBodyControlParameter(QString parameter, QVariant value){
    if(parameter == "IndicatorLeft") {
        m_bccFrame.IndicatorLeft = value.toBool();
    } else if(parameter == "IndicatorRight") {
        m_bccFrame.IndicatorRight = value.toBool();
    } else if(parameter == "Braking") {
        m_bccFrame.Braking = value.toBool();
    } else if(parameter == "Reversing") {
        m_bccFrame.Reversing = value.toBool();
    } else if(parameter == "HandBrake") {
        m_bccFrame.HandBrake = value.toBool();
    } else if(parameter == "SeatBelt") {
        m_bccFrame.SeatBelt = value.toBool();
    } else if(parameter == "PassengerSeatOccupied") {
        m_bccFrame.PassengerSeatOccupied = value.toBool();
    } else if(parameter == "RearLeftOccupied") {
        m_bccFrame.RearLeftOccupied = value.toBool();
    } else if(parameter == "RearMiddleOccupied") {
        m_bccFrame.RearMiddleOccupied = value.toBool();
    } else if(parameter == "RearRightOccupied") {
        m_bccFrame.RearRightOccupied = value.toBool();
    } else if(parameter == "PassengerSeatBelt") {
        m_bccFrame.PassengerSeatBelt = value.toBool();
    } else if(parameter == "RearLeftSeatBelt") {
        m_bccFrame.RearLeftSeatBelt = value.toBool();
    } else if(parameter == "RearMiddleSeatBelt") {
        m_bccFrame.RearMiddleSeatBelt = value.toBool();
    } else if(parameter == "RearRightSeatBelt") {
        m_bccFrame.RearRightSeatBelt = value.toBool();
    } else if(parameter == "DashBrightness") {
        m_bccFrame.DashBrightness = value.toUInt();
    } else if(parameter == "NightLight") {
        m_bccFrame.NightLight = value.toBool();
    }
    qDebug() << parameter << value;
    m_hudSerial.sendBodyControlCommand(m_bccFrame);
}
void HCUController::setDriveTrainParameter(QString parameter, QVariant value) {
    if(parameter == "speed") {
        m_dtcFrame.speed = value.toUInt();
    } else if(parameter == "engineRpm") {
        m_dtcFrame.engineRpm = value.toUInt();
    } else if(parameter == "frontLeftWheelSpeed") {
        m_dtcFrame.frontLeftWheelSpeed = value.toUInt();
    } else if(parameter == "frontRightWheelSpeed") {
        m_dtcFrame.frontRightWheelSpeed = value.toUInt();
    } else if(parameter == "rearLeftWheelSpeed") {
        m_dtcFrame.rearLeftWheelSpeed = value.toUInt();
    } else if(parameter == "rearRightWheelSpeed") {
        m_dtcFrame.rearRightWheelSpeed = value.toUInt();
    }
    qDebug() << parameter << value;
    m_hudSerial.sendDriveTrainControlCommand(m_dtcFrame);
}
void HCUController::PrintString(char * message, int len) {
    emit logLineReceived(QByteArray(message,len).data());
}


void HCUController::BodyControlCommandCallback(const BodyControlCommandFrame& controlFrame){
    m_bodyControlCommandFrame["IndicatorLeft"] = controlFrame.IndicatorLeft;
    m_bodyControlCommandFrame["IndicatorRight"] = controlFrame.IndicatorRight;
    m_bodyControlCommandFrame["Braking"] = controlFrame.Braking;
    m_bodyControlCommandFrame["Reversing"] = controlFrame.Reversing;
    m_bodyControlCommandFrame["HandBrake"] = controlFrame.HandBrake;
    m_bodyControlCommandFrame["SeatBelt"] = controlFrame.SeatBelt;
    m_bodyControlCommandFrame["PassengerSeatOccupied"] = controlFrame.PassengerSeatOccupied;
    m_bodyControlCommandFrame["RearLeftOccupied"] = controlFrame.RearLeftOccupied;
    m_bodyControlCommandFrame["RearMiddleOccupied"] = controlFrame.RearMiddleOccupied;
    m_bodyControlCommandFrame["RearRightOccupied"] = controlFrame.RearRightOccupied;
    m_bodyControlCommandFrame["PassengerSeatBelt"] = controlFrame.PassengerSeatBelt;
    m_bodyControlCommandFrame["RearLeftSeatBelt"] = controlFrame.RearLeftSeatBelt;
    m_bodyControlCommandFrame["RearMiddleSeatBelt"] = controlFrame.RearMiddleSeatBelt;
    m_bodyControlCommandFrame["RearRightSeatBelt"] = controlFrame.RearRightSeatBelt;
    m_bodyControlCommandFrame["DashBrightness"] = controlFrame.DashBrightness;
    m_bodyControlCommandFrame["NightLight"] = controlFrame.NightLight;
    emit BodyControlCommandFrameUpdated();
}

void HCUController::DriveTrainControlCommandCallback(const DriveTrainControlCommandFrame& controlFrame){
    m_driveTrainControlCommandFrame["speed"] = controlFrame.speed;
    m_driveTrainControlCommandFrame["engineRpm"] = controlFrame.engineRpm;
    m_driveTrainControlCommandFrame["frontLeftWheelSpeed"] = controlFrame.frontLeftWheelSpeed;
    m_driveTrainControlCommandFrame["frontRightWheelSpeed"] = controlFrame.frontRightWheelSpeed;
    m_driveTrainControlCommandFrame["rearLeftWheelSpeed"] = controlFrame.rearLeftWheelSpeed;
    m_driveTrainControlCommandFrame["rearRightWheelSpeed"] = controlFrame.rearRightWheelSpeed;
    emit DriveTrainControlCommandFrameUpdated();
}
