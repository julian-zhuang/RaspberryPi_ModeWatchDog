#include <QCoreApplication>
#include <QUdpSocket>
#include <QThread>
#include <QDateTime>
#include <QHostInfo>
#include <QSettings>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings m_Setting("DogFeed.ini",QSettings::IniFormat);
    m_Setting.beginGroup("Config");
    QString HostAddr = m_Setting.value("Addr").toString();
    unsigned short Port = m_Setting.value("Port").toUInt();
    unsigned int Spac = m_Setting.value("Spac").toUInt();
    if (HostAddr == ""){
        std::cout<< "Start Faild!<-1>" << std::endl;
        return -1;
    }
    if (Port == 0){
        std::cout<< "Start Faild!<-2>" << std::endl;
        return -2;
    }
    if (Spac == 0){
        std::cout << "Start Faild!<-3>" << std::endl;
        return -3;
    }
    std::cout << "Addr:" << HostAddr.toStdString() << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    std::cout << "Port:" << Port << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
    std::cout << "Spac:" << Spac << "    Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;

    QUdpSocket m_UdpSocket;
    while(1){
        char Command = 0x20;
        unsigned int Dtattime = QDateTime::currentDateTime().toTime_t();
        QByteArray WaitSendata;
        WaitSendata.clear();
        WaitSendata.append(Command);
        WaitSendata.append((char *)(&Dtattime) , 4);
        QHostAddress m_HostAddr = QHostInfo::fromName(HostAddr).addresses().first();
        m_UdpSocket.writeDatagram(WaitSendata.data(), 5, m_HostAddr, Port);
        std::cout << "Send:0x20    " << WaitSendata.toHex().toStdString() << "    " << m_HostAddr.toString().toStdString() << "    Time:" << QDateTime::fromTime_t(Dtattime).toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString() << std::endl;
        QThread::sleep(Spac);
    }
    return a.exec();
}
