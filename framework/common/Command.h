#ifndef COMMAND_H
#define COMMAND_H

#include <string.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

#include "DataSet.h"
#include "TextSerializer.h"

/**
 * @brief Accepts registration of commands from the client and with instance of
 * this class any object within this process space will be able to call this
 * class.
 * 
 */


namespace Fog {
namespace Common{

class Command;
typedef boost::function<void (DataSet<TextSerializer>&)> Func;
typedef std::map<std::string, boost::shared_ptr<Command> > CommandMap; 

class Command
{
    Func f_;
    char helpMsg_[20];
    char name_[20]; 
public:
    Command( const char* name, 
             size_t nameSize, 
             const char* helpMsg,
             size_t helpMsgSize, 
             Func f );

    void execute(DataSet<TextSerializer>& ds);
    void printHelp(std::ostream& os);
};

class CommandRouter
{
public:
    static CommandRouter* getInstance();
    bool registerCmd(const char* name, 
                size_t nameSize,
                const char* helpMsg,
                size_t helpMsgSize,

                Func f);

    bool deregisterCmd(const char* key, Func& f);

    void invokeMethod(const char* key, DataSet<TextSerializer>& arg);

private:
    CommandRouter() {};
    static CommandRouter* cmdRouter_;
    CommandMap commandMap_;
};

} // Common
} // Fog
#endif // COMMAND_H
