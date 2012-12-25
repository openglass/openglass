#ifndef LOGGER_H
#define LOGGER_H
/****************************************
File name   :   logger.h
Desc        :   logger Class definition
                Provides logging functions.
                The output is written on to the file openGlass.log
Author      :   Renjini
Version     :   1.0
//TODO - Need to make logger macro take ostream or something so that we do not
//have to make a string everytime to put in a log
*****************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <exception>

//#include "../include/errors.h"
#include "Errors.h"
#include "CommonDefs.h"


class Logger
{
    private:
        //log file handler
        std::ofstream logFile_;
        std::string logFileName_;

        //static instance of the logger class
        static Logger* logInstance_;

        //private constructor
        Logger();
//        Logger(PropertiesPtr prop);
        Logger(const char* fileName);

    public:
        //static function to get a singleton object
        static Logger* get_Logger_Instance();

//        //static function to create a singleton object
  //      static Logger* create(PropertiesPtr prop);
       //static function to create a singleton object
      static Logger* create(const char* fileName );

        //Function to write errors to logfile
        template <typename T>
        void writeErrorLog(std::string, std::string, int, T value);

        //Function to write warnings to logfile
        template <typename T>
        void writeWarningLog(std::string, std::string, int, T string);

        //Function to write debug messages to logfile
        template <typename T>
        void writeDebugLog(std::string, std::string, int, T string);

        ~Logger();

};

template <typename T>
void Logger :: writeErrorLog(std::string file, std::string func, int line, T value)
{
    if( DEBUG_BIT & DEBUG_ERROR )
        if (logFile_.is_open())
            logFile_ << "ERROR: " << file <<  " >> " << func << "()" <<"@ line " << line << " >> " << value << std::endl;
}

template <typename T>
void Logger :: writeWarningLog(std::string file, std::string func, int line, T value)
{
    if( DEBUG_BIT & DEBUG_WARNING )
        if (logFile_.is_open())
            logFile_ << "WARNING: " << file <<  " >> " << func << "()" <<"@ line " << line << " >> " << value << std::endl;
}

template <typename T>
void Logger :: writeDebugLog(std::string file, std::string func, int line, T value)
{
    if( DEBUG_BIT & DEBUG_INFO )
        if (logFile_.is_open())
            logFile_ << "DEBUG: " << file <<  " >> " << func << "()" <<"@ line " << line << " >> " << value << std::endl;
}

#define LOG_ERROR(x) Logger::get_Logger_Instance()->writeErrorLog(__FILE__, __FUNCTION__, __LINE__, (x))

#define LOG_WARNING(x) Logger::get_Logger_Instance()->writeWarningLog(__FILE__, __FUNCTION__, __LINE__, (x))

#define LOG_DEBUG(x) Logger::get_Logger_Instance()->writeDebugLog(__FILE__, __FUNCTION__, __LINE__, (x))

#endif // LOGGER_H
