#include "NetWorkWatchDog.h"
#include <QDateTime>
#include <QDebug>
#include <wiringPi.h>
#include <QSettings>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QThread>

NetWorkWatchDog::NetWorkWatchDog()
{
    m_Port = 0;
    m_TimeOut = 600 * 1000; //10Min
    m_UdpSocket = nullptr;
    connect(&m_Dog,SIGNAL(timeout()),this,SLOT(Slot_DogBarking()));
}

NetWorkWatchDog::~NetWorkWatchDog()
{

}

void NetWorkWatchDog::SetPort(unsigned short Port)
{
    m_Port = Port;
    std::cout << "Set Port:" << m_Port << "    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
}

void NetWorkWatchDog::SetTimeOut(unsigned int Sec)
{
    m_TimeOut = Sec * 1000;
    std::cout << "Set TimeOut:" << m_TimeOut << "ms" << "    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
}

void NetWorkWatchDog::SetSrcIP(QHostAddress SrcAddr)
{
    m_SrcAddr = SrcAddr;
    std::cout << "Set SrcIP:" << SrcAddr.toString().toStdString() << "    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
}

void NetWorkWatchDog::SetSwitch_01(unsigned short Io)
{
    Switch_01 = Io;
}

void NetWorkWatchDog::SetSwitch_02(unsigned short Io)
{
    Switch_02 = Io;
}

void NetWorkWatchDog::SetSwitch_03(unsigned short Io)
{
    Switch_03 = Io;
}

void NetWorkWatchDog::SetSwitch_04(unsigned short Io)
{
    Switch_04 = Io;
}

void NetWorkWatchDog::SetBeeIoPort(unsigned short Io)
{
    m_Bee = Io;
}

void NetWorkWatchDog::Init()
{
    wiringPiSetup();
    std::cout << "wiringPiSetup()" << "    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    pinMode(m_Bee,OUTPUT);
    digitalWrite(m_Bee, false);
    QThread::msleep(600);
    digitalWrite(m_Bee,true);

    if (Switch_01 != 0)    {
        pinMode(Switch_01,OUTPUT);
        digitalWrite(Switch_01,true);
        std::cout <<"IO:"<< Switch_01 << " Set Out<" << true << ">    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    }
    if (Switch_02 != 0)    {
        pinMode(Switch_02,OUTPUT);
        digitalWrite(Switch_02,true);
        std::cout << "IO:"<<Switch_02 << " Set Out<" << true << ">    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    }
    if (Switch_03 != 0)    {
        pinMode(Switch_03,OUTPUT);
        digitalWrite(Switch_03,true);
        std::cout << "IO:" << Switch_03 << " Set Out<" << true << ">    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    }
    if (Switch_04 != 0)    {
        pinMode(Switch_04,OUTPUT);
        digitalWrite(Switch_04,true);
        std::cout << "IO:" << Switch_04 << " Set Out<" << true << ">    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    }
}

int NetWorkWatchDog::Start()
{
    if (m_Port == 0){
        return -1;
    }
    m_UdpSocket = new QUdpSocket(this);
    m_UdpSocket->bind(QHostAddress::Any,m_Port);
    connect(m_UdpSocket,SIGNAL(readyRead()),this,SLOT(Slot_SocketReadyRead()));
    m_Dog.start(m_TimeOut);
    std::cout << "NetWorkWatchDog Start" << "    " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
}

void NetWorkWatchDog::Stop()
{
    m_Dog.stop();
    m_UdpSocket->close();
    delete m_UdpSocket;
    m_UdpSocket = nullptr;
}

void NetWorkWatchDog::Slot_SocketReadyRead()
{
    QHostAddress t_HostAddr;
    quint16 t_Port;
    QByteArray RecvData;
    RecvData.resize(m_UdpSocket->pendingDatagramSize());
    //读取数据
    m_UdpSocket->readDatagram(RecvData.data(),RecvData.size(),&t_HostAddr,&t_Port);
    if (RecvData.length() == 0){
        return;
    }
    digitalWrite(7,false);
    QThread::msleep(100);
    digitalWrite(7,true);
    char Command = RecvData.at(0);
    if (Command == 0x10){
        std::cout << "Recv:0x10"
                  << "    From:" << t_HostAddr.toString().toStdString()
                  << "    Port:" << t_Port
                  << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString()
                  << std::endl;
        m_Dog.stop();
        std::cout << "Stop Timer!";
        return;
    }
    if (Command == 0x20){
        std::cout << "Recv:0x20"
                  << "    From:" << t_HostAddr.toString().toStdString()
                  << "    Port:" << t_Port
                  << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString()
                  << std::endl;

        unsigned int Data = *((unsigned int *)RecvData.mid(1,4).data());
        int Cheack = Data - QDateTime::currentDateTime().toTime_t();
        if (abs(Cheack) < 5){
            m_Dog.start(m_TimeOut);
            std::cout << "Restart Timer!"
                      << "    " << Data
                      << std::endl;
        }
        return;
    }
    if (Command == 0x30){
        std::cout << "Recv:0x30"
                  << "    From:" << t_HostAddr.toString().toStdString()
                  << "    Port:" << t_Port
                  << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString()
                  << std::endl;

        char Switch_Number = RecvData.at(1);
        char Switch_State = RecvData.at(2);

        if (Switch_State > 0){
            Switch_State = true;
        }else{
            Switch_State = false;
        }

        if (Switch_Number == 1){
            digitalWrite(Switch_02,Switch_State);
        }
        if (Switch_Number == 2){
            digitalWrite(Switch_02,Switch_State);
        }
        if (Switch_Number == 3){
            digitalWrite(Switch_02,Switch_State);
        }
        if (Switch_Number == 4){
            digitalWrite(Switch_02,Switch_State);
        }
        return;
    }
}

void NetWorkWatchDog::Slot_DogBarking()
{
    std::cout << "TimeOut!"<< "    " <<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    m_Dog.stop();
    digitalWrite(7,false);
    QThread::msleep(1000);
    digitalWrite(7,true);
    emit Sig_DogBarking();

    digitalWrite(Switch_01,false);
    std::cout << "Switch_01  OFF"<<std::endl;
    digitalWrite(Switch_02,false);
    std::cout << "Switch_02  OFF"<<std::endl;
    digitalWrite(Switch_03,false);
    std::cout << "Switch_03  OFF"<<std::endl;
    digitalWrite(Switch_04,false);
    std::cout << "Switch_04  OFF"<<std::endl;
    QThread::msleep(5000);
    digitalWrite(Switch_01,true);
    std::cout << "Switch_01  NO"<<std::endl;
    digitalWrite(Switch_02,true);
    std::cout << "Switch_02  NO"<<std::endl;
    digitalWrite(Switch_03,true);
    std::cout << "Switch_03  NO"<<std::endl;
    digitalWrite(Switch_04,true);
    std::cout << "Switch_04  NO"<<std::endl;
    m_Dog.start(m_TimeOut);
}
