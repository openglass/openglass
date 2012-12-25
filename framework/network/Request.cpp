#include "Request.h"
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Fog {
namespace Network{

/**
 * @brief  It will make the thread that needs the reply to wait until the
 * receivedReply_ condition variable is signalled
 *
 * @param theReply - The replay dataset sent as reply by the remote process
 *
 */
DSptr Request::getReply()
{
    // check if reply is alreyd available, if yes return the reply
    boost::mutex::scoped_lock lock(lock_);
    if (isReplySet_)
        return reply_;

    // calculat abs time + 5 seconds (max wait time)
    boost::system_time const timeout = boost::get_system_time() +
                                       boost::posix_time::milliseconds(timeout_);
    if(!gotReply_.timed_wait(lock, timeout) )
    {
        std::ostringstream tmp;
        tmp << " No reply for Request ID" << reqId_ << 
               "Reply wait timed out after - " << timeout_ << "miliseconds" << "\n";
        LOG_ERROR (tmp.str());
    }
    return reply_; // Client needs to check if returned DSptr is set or not.
}

/**
 * @brief Will set the reply dataset and signal the receivedReply condition
 * variable
 *
 * @param theReply - The replay dataset sent as reply by the remote process
 *
 */
void Request::setReply(DSptr theReply)
{
    boost::mutex::scoped_lock lock(lock_);
    reply_=theReply;
    isReplySet_=true;
    gotReply_.notify_one(); // only one thread should be waiting on each request
}

} // Network
} // Fog

