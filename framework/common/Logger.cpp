#include "Logger.h"
#include <stdexcept>

Logger* Logger::logInstance_ = NULL;

Logger :: Logger()
{
	logFile_.open("openGlass.log", std::ios_base::out);
    logFileName_="openGlass.log";
}

Logger :: Logger(const char* fileName)
{
    if(fileName)
    {
	    logFile_.open(fileName, std::ios_base::out);
        if (!logFile_.is_open())
            throw std::runtime_error("logger fileName not valid");
    }
    else 
        throw std::runtime_error("Logger fileName is null");

    logFileName_=fileName;
}


#if 0
Logger :: Logger(PropertiesPtr properties)
{
    //logFile_.open("../logs/openGlass.log", std::ios_base::out);
    if (properties)
    {
      Properties::iterator ret;
      ret=properties->find(std::string("logFile"));
      if (ret == properties->end())
      {
          std::cout << "Unable to create logger" << std::endl;
          return;
      }
      else
      {
          std::string* fileName=boost::any_cast<std::string>(&(ret->second));
          if (fileName == NULL)
          {
            std::cout << "Unable to create logger" << std::endl;
            return;
          }

          logFileName_=*fileName;
      }
	    logFile_.open(logFileName_.c_str(), std::ios_base::out);
    }
}

#endif 

//TODO make it safer.. and default filestream should be cout
Logger* Logger :: create(const char* fileName )
{
    if( NULL == logInstance_ )
        logInstance_ = new Logger(fileName);
    return logInstance_;
}

Logger* Logger :: get_Logger_Instance()
{
    if( NULL == logInstance_ )
        return Logger::create("openglass.log");
    return logInstance_;
}

Logger :: ~Logger()
{
    logFile_.close();

    if(logInstance_)
        delete logInstance_;
}
