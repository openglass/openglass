#include "Command.h"
#include <string.h>

/**
 * @brief Accepts registration of commands from the client and with instance of
 * this class any object within this process space will be able to call this
 * class.
 * 
 */

namespace Fog {
namespace Common{

CommandRouter* CommandRouter::cmdRouter_ = NULL;

CommandRouter*
CommandRouter::getInstance()
{
    if(cmdRouter_== NULL)
    {
        cmdRouter_=new CommandRouter();;
        return cmdRouter_;
    }
    return cmdRouter_;
}

Command::Command( const char* name, 
        size_t nameSize, 
        const char* helpMsg,
        size_t helpMsgSize, 
        Func f ) : f_(f)
    {
        memset(name_, 0, sizeof(name_));
        memset(helpMsg_, 0, sizeof(helpMsg_));

        memcpy( (void*)name_, (void*)name, 
                nameSize > sizeof(name_) ? sizeof(name_)-1:nameSize);
        memcpy( (void*)helpMsg_, (void*)helpMsg, 
                helpMsgSize > sizeof(helpMsg_) ? sizeof(helpMsg_)-1:helpMsgSize );
    }

void Command::execute(DataSet<TextSerializer>& ds) 
{
    if (f_)
        f_(ds);
}

void Command::printHelp(std::ostream& os)
{
    os << "Method - " << name_ << " - " << helpMsg_ << std::endl;
}

bool CommandRouter::registerCmd(  const char* name, 
                                size_t nameSize,
                                const char* helpMsg,
                                size_t helpMsgSize,
                                Func f) // any function object
{
    // find command in command registry
    if (!name)
        return false;

    CommandMap::iterator iter;
    iter=commandMap_.find(std::string(name));
    if (iter == commandMap_.end())
    {
        // create an empty vector commands
        // this is first command associated with this key
        boost::shared_ptr<Command> cmd(new Command(name, nameSize, helpMsg, helpMsgSize, f));
        commandMap_.insert(std::pair<std::string, boost::shared_ptr<Command> > (std::string(name), cmd));
        return true;
    }
    else
    {
        std::ostringstream tmp;
        tmp << "Command " << name 
            << " already exists " << std::endl ;
        LOG_DEBUG(tmp.str());
        return false;
    }
}

bool CommandRouter::deregisterCmd(const char* key, Func& f)
{
    CommandMap::iterator iter;
    iter=commandMap_.find(std::string(key));
    if (iter == commandMap_.end())
    {
        std::ostringstream tmp;
            tmp << "Command " << key
            << " does not exist" << std::endl ;
        LOG_DEBUG(tmp.str());
        return true;
    }
    else
    {
        commandMap_.erase(iter);
        return true;
    }
}

void CommandRouter::invokeMethod(const char* key, DataSet<TextSerializer>& arg)
{
    // if command is "help" then call help on each command
    //
    int ret = memcmp(key, "help", 4); 
    if (ret ==0)
    {
        CommandMap::iterator iter=commandMap_.begin();
        while (iter != commandMap_.end())
        {
            std::ostringstream tmp;
            iter->second->printHelp(tmp);
            LOG_DEBUG(tmp.str());
            ++iter;
        }

        const char *request="REQUEST";
        const char *msg="STATMESSAGE";
        const char* stat="STATUS";
        arg.add<std::string>(request, stat, "true");
        arg.add<std::string>(request, msg, "help printed out ");
        return;
    }

    CommandMap::iterator iter=commandMap_.find(std::string(key));
    if (iter == commandMap_.end())
    {
        std::ostringstream tmp;
            tmp << "Command " << key
                << " does not exist" << std::endl ;
        LOG_DEBUG(tmp.str());
        // return map with status false
        const char *request="REQUEST";
        const char *msg="STATMESSAGE";
        const char* stat="STATUS";
        arg.add<std::string>(request, stat, "false");
        arg.add<std::string>(request, msg, "Requested Command not found");
       return;
    }
    else
    {
        iter->second->execute(arg);
    }
}

}
}

