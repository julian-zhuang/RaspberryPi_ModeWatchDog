#ifndef NETWORKWATCHDOG_H
#define NETWORKWATCHDOG_H
#include <QUdpSocket>
#include <QTimer>
#include <QObject>
#include <QHostAddress>

class NetWorkWatchDog : public QObject
{
    Q_OBJECT
signals:
    void Sig_DogBarking();
public:
    NetWorkWatchDog();
    ~NetWorkWatchDog();
    void SetPort(unsigned short Port);
    void SetTimeOut(unsigned int Sec);
    void SetSrcIP(QHostAddress SrcAddr);
    void SetSwitch_01(unsigned short Io);
    void SetSwitch_02(unsigned short Io);
    void SetSwitch_03(unsigned short Io);
    void SetSwitch_04(unsigned short Io);
    void SetBeeIoPort(unsigned short Io);
    void Init();
    int Start();
    void Stop();

private slots:
    void Slot_SocketReadyRead();
    void Slot_DogBarking();
private:
    NetWorkWatchDog *m_NetWorkWatchDog;
    QUdpSocket *m_UdpSocket;
    QHostAddress m_SrcAddr;
    unsigned short m_Port;
    QTimer m_Dog;
    unsigned int m_TimeOut;
    unsigned short m_Bee;
    unsigned short Switch_01;
    unsigned short Switch_02;
    unsigned short Switch_03;
    unsigned short Switch_04;
};

#endif // NETWORKWATCHDOG_H
