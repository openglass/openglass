#include "OpenGlassClient.h"

#include <boost/shared_ptr.hpp>

#include "common/Command.h"

namespace FC=Fog::Common;
namespace Fog {

    typedef boost::shared_ptr<Fog::Network::Request>  ReqPtr;
    OpenGlassClient::OpenGlassClient(const char* configFileName, const char* logFileName)
        : BaseGlass(configFileName, logFileName)
    {
    }

    bool OpenGlassClient::initialize()
    {
        // Initialize base
        BaseGlass::initialize();
        return true;
    }

    void OpenGlassClient::run()
    {
        std::cout << "sleeping for 5 seconds before trying this to get connection" <<std::endl;
        sleep(5);

        DataSet<TextSerializer> request("REQUEST");
        request.add<std::string>("REQUEST", "METHOD", "testMethod");

        DSptr ds1=rmi_->invokeRemoteMethod(request);
        if (ds1.get())
        {
            int* reqPtr = ds1->get<int>("REQUEST", "REQUESTID");
            std::string* statusPtr= ds1->get<std::string>("REQUEST", "STATUS");
            if (reqPtr  && statusPtr )
                std::cout << "Request id - " << *reqPtr  << "status is " << *statusPtr << std::endl;
            else
            {
                std::cout << "unable to retrieve status "<< std::endl;
                return;
            }
        }
        else
        {
            std::cout << "no dataset found" << "\n";
        }

        DataSet<TextSerializer> request2("REQUEST");
        request2.add<std::string>("REQUEST", "METHOD", "exit");

        std::cout << " calling async exit " << "\n";
        ReqPtr asyncReq=rmi_->invokeRemoteMethodAsync(request2);
        if (asyncReq.get())
        {
            std::cout << " Got async request object " << "\n";
            std::cout << "Lot of work to do before going back to pick request. So sleepig for 5 seconds" << "\n";
            std::cin.get(); // TODO - how about a command thread wait!!
        }
        else
        {
            std::cout << " Something went wrong here" << "\n";
            return;
        }

        DSptr ds2=asyncReq->getReply();
        if (ds2.get())
        {
            int* reqPtr = ds2->get<int>("REQUEST", "REQUESTID");
            std::string* statusPtr= ds2->get<std::string>("REQUEST", "STATUS");
            if (reqPtr  && statusPtr )
                std::cout << "Request id - " << *reqPtr  << "status is " << *statusPtr << std::endl;
            else
            {
                std::cout << "unable to retrieve status second time over"<< std::endl;
                // stopping now
                BaseGlass::stop();
                return;
            }
        }
        else
        {
            std::cout << "no dataset found for request 2 " << "\n";
        }

        // stopping now
        BaseGlass::stop();
    }
}// Fog

