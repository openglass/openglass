#ifndef REQUEST_H
#define REQUEST_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"

typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;

namespace Fog {
namespace Network{

class Request
{
    public:
    Request(int32_t requestId) : reqId_(requestId), isReplySet_(false), timeout_(10000){}

    /**
     * @brief  It will make the thread that needs the reply to wait until the
     * receivedReply_ condition variable is signalled
     *
     * @param theReply - The replay dataset sent as reply by the remote process
     *
     */
    DSptr getReply();

    /**
     * @brief Will set the reply dataset and signal the receivedReply condition
     * variable
     *
     * @param theReply - The replay dataset sent as reply by the remote process
     *
     */
    void setReply(DSptr theReply);
    void setTimeout(uint32_t theTimeout) { timeout_=theTimeout;}
    uint32_t getTimeou() { return timeout_; } 
private:
    DSptr   reply_;
    boost::mutex lock_;
    boost::condition gotReply_;
    int32_t reqId_;
    bool isReplySet_;
    uint32_t timeout_;
};

} // Network
} // Fog

#endif //REQUEST_H
