#ifndef OGCLIENT_H
#define OGCLIENT_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include "qobject.h"
#include "ProtocolTableModel.h"
#include "ui_OgAdminWindow.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "network/Packetizer.h"
#include "common/DataSet.h"
#include "common/TextSerializer.h"


class OgClient : public /*QObject,*/ QThread
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
    typedef boost::shared_ptr<Fog::Network::TCPClient> TCPClient;
    typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;

    OgClient(/*DataSet<TextSerializer> &dataSet,*/ QThread* parent = 0);
    bool initialize(DataSet<TextSerializer>& config_);
    void run();
    void closeConnection();
    void setRequest(DataSet <TextSerializer> *req);
    bool sendRequest();
    DataSet <TextSerializer> *getRequest();

    static void sleep(unsigned long secs)
    {
        QThread::sleep(secs);
    }

//public Q_SLOTS:
//    void handleDataReceived();

//Q_SIGNALS:
//    void dataReceived();

private:
    void operator()(TCPConnectionPtr conn);
    void onPacket(char* data, size_t size);

    int event;
    TCPClient tcpClient_;
    CmdPacketizer cmdPacketizer_;
    TCPConnectionPtr conn_;
    DataSet <TextSerializer> *request_; //Stores the request send to the server

    boost::condition dataReceived_;
    boost::mutex waitForDataMutex_;

    static int firstRun;
};


#endif // OGCLIENT_H
