#include <QCoreApplication>
#include <NetWorkWatchDog.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QThread>
#include <wiringPi.h>
#include <QSettings>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings m_Settings("GPIOConfig.ini",QSettings::IniFormat);
    m_Settings.beginGroup("SwitchList");
    unsigned short Switch_01 = m_Settings.value("Switch_01").toUInt();
    unsigned short Switch_02 = m_Settings.value("Switch_02").toUInt();
    unsigned short Switch_03 = m_Settings.value("Switch_03").toUInt();
    unsigned short Switch_04 = m_Settings.value("Switch_04").toUInt();
    unsigned short Bee_IO = m_Settings.value("Bee").toUInt();
    m_Settings.endGroup();
    m_Settings.beginGroup("TimeOut");
    unsigned int TimeOut = m_Settings.value("Value").toUInt();
    std::cout << TimeOut;
    if (TimeOut != 600){
        return 0;
    }

    m_Settings.endGroup();
    m_Settings.beginGroup("Port");
    unsigned int Net_Port = m_Settings.value("Value").toUInt();
    m_Settings.endGroup();

    NetWorkWatchDog * m_NetWorkWatchDog = new NetWorkWatchDog;
    m_NetWorkWatchDog->SetSwitch_01(Switch_01);
    m_NetWorkWatchDog->SetSwitch_02(Switch_02);
    m_NetWorkWatchDog->SetSwitch_03(Switch_03);
    m_NetWorkWatchDog->SetSwitch_04(Switch_04);
    m_NetWorkWatchDog->SetBeeIoPort(Bee_IO);
    m_NetWorkWatchDog->SetPort(Net_Port);
    m_NetWorkWatchDog->SetTimeOut(TimeOut);
    m_NetWorkWatchDog->Init();
    m_NetWorkWatchDog->Start();
    a.exec();
    delete m_NetWorkWatchDog;
    return 0;
}
