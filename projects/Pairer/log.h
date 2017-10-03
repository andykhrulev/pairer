//
// Created by Andrey Khrulev on 16.09.17.
//

#ifndef SAMPLEPROJECT_DEBUG_H
#define SAMPLEPROJECT_DEBUG_H

#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ios>

#ifdef DEBUG_ENABLED

#define ENABLE_DEBUG() \
    std::ostream* Logger::errstream; \
    int Logger::level;

#define INIT_DEBUG(stream) Logger log(stream)

#define LOGGER_IN() Logger log(__FILE__, __FUNCTION__);
#define LOGGER_STR(msg) log.print(__LINE__, msg);
#define LOGGER_STR_ARG1(fmt,arg1) {\
    char buf[301]; buf[300]=0;\
    snprintf(buf,300,fmt,arg1); \
    log.print(__LINE__, buf); }
#define LOGGER_STR_ARG2(fmt,arg1, arg2) {\
    char buf[301]; buf[300]=0;\
    snprintf(buf,300,fmt,arg1, arg2); \
    log.print(__LINE__, buf); }
#define LOGGER_STR_ADDR(str, addr_ptr) {\
    struct sockaddr* sockaddr_ptr = addr_ptr; \
    if (sockaddr_ptr->sa_family == AF_INET) {\
        struct sockaddr_in *addr_in = (struct sockaddr_in *) addr_ptr; \
        char *s = inet_ntoa(addr_in->sin_addr); \
        char buf[301]; buf[300]=0;\
        snprintf(buf,300,"%s %s:%d",str,s,ntohs(addr_in->sin_port)); \
        log.print(__LINE__, buf); }}
#else

#define ENABLE_DEBUG()
#define INIT_DEBUG(stream)
#define LOGGER_IN()
#define LOGGER_STR(msg)
#define LOGGER_STR_ARG1(fmt,arg1)
#define LOGGER_STR_ARG2(fmt,arg1, arg2)
#define LOGGER_STR_ADDR(str, addr_ptr)

#endif

class Logger {
public:
    Logger() = delete;
    Logger(const char* filename, const char* funcname) {
        std::stringstream sstream;
        if(errstream == nullptr)
            errstream = new std::ofstream("report.txt", std::ofstream::out);//&std::cerr;
        this->filename.append(filename);
        this->funcname.append(funcname);

        for(int i=level++; i> 0; --i) sstream << "    ";
        sstream << ">>>> " << /*filename << "::" << */ funcname << "()" << std::endl;
        *errstream << sstream.str();
        errstream->flush();
    }
    Logger(std::ostream* ostream) {
        level = 0;
        errstream = ostream;
        errstream->flush();
    }
    ~Logger() {
        std::stringstream sstream;
        if (level > 0) {
            for(int i=--level; i> 0; --i) sstream << "    ";
            sstream << "<<<< " /*<< filename << "::"*/ << funcname << "()" << std::endl;
            *errstream << sstream.str();
            errstream->flush();
        }
        else {
            errstream->flush();
            delete errstream;
        }
    }

    void print(int line, const char* output){
        std::stringstream sstream;
        for(int i=level; i> 0; --i) sstream << "    ";
        sstream << line << ": " << output << std::endl;
        *errstream << sstream.str();
        errstream->flush();
    }

private:
    static std::ostream *errstream; // single per-application stream
    std::string filename, funcname;
    static int level;
};


#endif //SAMPLEPROJECT_DEBUG_H
