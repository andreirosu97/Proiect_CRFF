#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>
#include "Globals.h"
#include "File.h"

using namespace omnetpp;

/**
 * In this step we keep track of how many messages we send and received,
 * and display it above the icon.
 */
class Host : public cSimpleModule
{
private:
    cModule *server;
    cModule *requestModule;
    simtime_t transferTime;

    simtime_t requestInterval;
    cMessage* fileReqEv;
    cMessage* fileReqTimeoutEv;
    cMessage* getFileEv;
    FileRequest *lastFileReq;

    // speed of light in m/s
    const double propagationSpeed = 299792458.0;
    simtime_t radioDelay;
    double txRate;
    cPar *pkLenBits;
    double x;
    double y;

    enum  {IDLE, REQUESTING,SELECTING, RECEIVING,OFFERING, TRANSMITING }state;

public:
    Host();
    virtual ~Host();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    simtime_t getNextTransmissionTime();
    cMessage* generateMessage(int msgType);
    File* findFile(const char* fileName);
    void fixFileName();
};

Define_Module(Host);

void Host::initialize()
{
    server = getModuleByPath("server");
    requestInterval = par("requestInterval");
    state = IDLE;

    txRate = par("txRate");
    fixFileName();

    x = par("x").doubleValue();
    y = par("y").doubleValue();

    double serverX = server->par("x").doubleValue();
    double serverY = server->par("y").doubleValue();
    double dist = std::sqrt((x-serverX) * (x-serverX) + (y-serverY) * (y-serverY));
    radioDelay = dist / propagationSpeed;

    if(getParentModule()->getIndex() == 0)
        scheduleAt(getNextTransmissionTime(),generateMessage(SEND_FILE_REQUEST_EVENT));
}

void Host::handleMessage(cMessage *msg)
{
    char msgname[20];
    char fileName[20];
    switch(msg->getKind())
    {
    case SEND_FILE_REQUEST_EVENT:
        {
            ASSERT(state == IDLE);
            state = REQUESTING;
            if (lastFileReq == nullptr)
            {
                FileRequest* fileReqMessage = (FileRequest *)generateMessage(SEND_FILE_REQUEST);
                lastFileReq = fileReqMessage->dup();
                sendDirect(fileReqMessage, radioDelay, 0, server->gate("in"));
                scheduleAt(simTime() + 10, generateMessage(SEND_FILE_REQUEST_TIME_OUT));
            }
            else
            {
                EV << "Last request did not get a response yet.\n";
            }
            break;
        }
    case SEND_FILE_REQUEST:
        {
            if (msg->isSelfMessage() && state == REQUESTING)
            {
                sendDirect(lastFileReq, radioDelay, 0, server->gate("in"));
                scheduleAt(simTime() + 10, generateMessage(SEND_FILE_REQUEST_TIME_OUT));
            }
            else if (state == IDLE)
            {
                FileRequest *message = (FileRequest *)msg->dup();
                const char* fileName = message->getFileName();
                File* file = findFile(fileName);
                if (file != nullptr) // if file was found
                {
                    EV << "Changing state of host["<< getParentModule()->getIndex() <<"] to OFFERING\n";
                    state = OFFERING;
                    message->setTxRate(txRate);
                    message->setFileSize(file->getSize());
                    message->setDestAddress(getParentModule()->getIndex());
                    message->setKind(HAVE_FILE);
                    cModule* sourceModule = getParentModule()->getParentModule()->getSubmodule("host",message->getSourceAddress())->getSubmodule("host");
                    double srcX = sourceModule->par("x").doubleValue();
                    double scrY = sourceModule->par("y").doubleValue();
                    double dist = std::sqrt((x-srcX) * (x-srcX) + (y-scrY) * (y-scrY));
                    radioDelay = dist / propagationSpeed;
                    sendDirect(message, radioDelay, 0, sourceModule->gate("in"));
                }
            }
            break;
        }
    case SEND_FILE_REQUEST_AKG:
        {
            if (state == REQUESTING)
            {
                EV << "Changing state of host["<< getParentModule()->getIndex() <<"] to RECEIVING\n";
                cancelEvent(fileReqTimeoutEv);
                state = SELECTING;
                lastFileReq = nullptr;
                //TODO Add request after 60 sec if no file was received, no one in the network has the file, move state to REQUESTING
            }
            break;
        }
    case SEND_FILE_REQUEST_TIME_OUT:
        {
            ASSERT(state == REQUESTING);
            EV << "Server did not receive my request, resending it.\n";
            scheduleAt(getNextTransmissionTime(),lastFileReq);
            break;
        }

    case HAVE_FILE:
        {
            if (state == SELECTING)
            {
                FileRequest *message = (FileRequest *)msg->dup();
                simtime_t duration = message->getFileSize() / message->getTxRate();
                cModule* destModule = getParentModule()->getParentModule()->getSubmodule("host",message->getDestAddress())->getSubmodule("host");
                double dstX = destModule->par("x").doubleValue();
                double dstY = destModule->par("y").doubleValue();
                double dist = std::sqrt((x-dstX) * (x-dstX) + (y-dstY) * (y-dstY));
                radioDelay = dist / propagationSpeed;
                simtime_t totalTime = radioDelay + duration;
                EV << "Nice gucci comming from : host " << message->getDestAddress() << " " << dist << " total time = " << totalTime << "\n";
                if (requestModule == nullptr)
                {
                    requestModule = destModule;
                    transferTime = totalTime;
                }
                else if (transferTime > totalTime)
                {
                    requestModule = destModule;
                    transferTime = totalTime;
                }

            }
        }
    default:
        {
            return;
        }
    }
}

cMessage* Host::generateMessage(int msgType)
{

    switch(msgType)
    {
        case SEND_FILE_REQUEST :
        {
            char msgname[20];
            char fileName[20];
            sprintf(fileName, "file%d.dll",intuniform(0,20));
            sprintf(msgname, "Request for %s", fileName);
            FileRequest *fileReqMessage = new FileRequest(msgname);
            fileReqMessage->setSourceAddress(getParentModule()->getIndex());
            fileReqMessage->setDestAddress(-1);
            fileReqMessage->setFileName(fileName);
            fileReqMessage->setKind(SEND_FILE_REQUEST);
            return fileReqMessage;
        }
        case SEND_FILE_REQUEST_EVENT: return fileReqEv;
        case SEND_FILE_REQUEST_TIME_OUT : return fileReqTimeoutEv;
        default : return nullptr;
    }
}

File* Host::findFile(const char* fileName)
{
    File *file = nullptr;
    for(int i = 0; i < getParentModule()->par("maxNumFiles").intValue(); i++)
    {
        file = (File*)getParentModule()->getSubmodule("files",i);
        if (file->getName() == std::string(fileName))
        {
            EV << "I HAVE IT !\n";
            return file;
        }
    }
    return nullptr;
}

simtime_t Host::getNextTransmissionTime()
{
    EV << "Next transmision from host " << getParentModule()->getIndex()<< " will be at : " << simTime() + requestInterval << "\n";
    return simTime() + requestInterval;
}

Host::Host()
{
    fileReqEv = new cMessage("requestEvent",SEND_FILE_REQUEST_EVENT);
    fileReqTimeoutEv = new cMessage("Send Request Timeout",SEND_FILE_REQUEST_TIME_OUT);
    lastFileReq = nullptr;
    requestModule = nullptr;
}

Host::~Host()
{
    cancelAndDelete(fileReqEv);
    cancelAndDelete(fileReqTimeoutEv);

}

void Host::fixFileName()
{
    File *file1 = nullptr;
    File *file2 = nullptr;
    bool toBeFixed = false;
    do
    {
        toBeFixed = false;
        for(int i = 0; i < getParentModule()->par("maxNumFiles").intValue() -1 ; i++)
        {
            file1 = (File*)getParentModule()->getSubmodule("files",i);
            for(int j = i+1; j < getParentModule()->par("maxNumFiles").intValue(); j++)
            {
                file2 = (File*)getParentModule()->getSubmodule("files",j);
                if (file1->getName() == file2->getName())
                {
                    file2->setNameAndSeq(file2->getSeqNum()+1);
                    toBeFixed = true;
                }
            }
        }
    }
    while(toBeFixed);
}





