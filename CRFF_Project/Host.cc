#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string>
#include "Globals.h"
#include "File.h"

#define OFFER_REJECT_TIMEOUT 30
#define WAIT_FOR_MORE_OFFERS_TIMEOUT 20
#define NO_RESPONSE_FROM_SERVER_TIMEOUT 10
#define WAIT_FOR_OFFERS_TIMEOUT 60

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
    cMessage* moreOffersTimeoutEv;
    cMessage* anyOffersTimeoutEv;
    cMessage* fileReceivedCompletEv;
    cMessage* fileSendCompletEv;


    FileRequest *lastFileReq;
    FileRequest *getFileReq;
    FilePacket *lastFilePacketRec;
    File* file;

    int failTimes = 0;

    // speed of light in m/s
    const double propagationSpeed = 299792458.0;
    simtime_t radioDelay;
    double txRate;
    double x;
    double y;

    enum  {IDLE, REQUESTING,SELECTING, OFFERING ,RECEIVING, TRANSMITING }state;

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
    void requestNewFile();
    void storeFileFromPacket(FilePacket *packet);
};

Define_Module(Host);

void Host::initialize()
{
    server = getModuleByPath("server");
    requestInterval = par("requestInterval");
    state = IDLE;
    gate("in")->setDeliverOnReceptionStart(true);

    txRate = par("txRate");
    fixFileName();

    x = par("x").doubleValue();
    y = par("y").doubleValue();

    double serverX = server->par("x").doubleValue();
    double serverY = server->par("y").doubleValue();
    double dist = std::sqrt((x-serverX) * (x-serverX) + (y-serverY) * (y-serverY));
    radioDelay = dist / propagationSpeed;

    if(getParentModule()->par("active").boolValue() == true)
        requestNewFile();

}

void Host::handleMessage(cMessage *msg)
{
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
                failTimes = 0;
                sendDirect(fileReqMessage, radioDelay, 0, server->gate("in"));
                scheduleAt(simTime() + NO_RESPONSE_FROM_SERVER_TIMEOUT, generateMessage(SEND_FILE_REQUEST_TIME_OUT));
            }
            else
            {
                EV << "Last request did not get a response yet.\n";
                scheduleAt(getNextTransmissionTime(), msg);
            }
            break;
        }
    case SEND_FILE_REQUEST_TIME_OUT: //Server did not respond sending again.
        {
            ASSERT(state == REQUESTING);
            EV << "Server did not receive my request, resending it.\n";
            scheduleAt(getNextTransmissionTime(),lastFileReq);
            break;
        }
    case SEND_FILE_REQUEST_AKG:
        {
            ASSERT(state == REQUESTING);
            EV << "Akg from server ----> Changing state of host["<< getParentModule()->getIndex() <<"] to SELECTING\n";
            cancelEvent(fileReqTimeoutEv);
            state = SELECTING;
            requestModule = nullptr;
            scheduleAt(simTime() + WAIT_FOR_OFFERS_TIMEOUT, anyOffersTimeoutEv);
            break;
        }
    case SEND_FILE_REQUEST:
        {
            if (msg->isSelfMessage() && state == REQUESTING)
            {
                FileRequest* message = (FileRequest *)lastFileReq->dup();
                sendDirect(message, radioDelay, 0, server->gate("in"));
                scheduleAt(simTime() + NO_RESPONSE_FROM_SERVER_TIMEOUT, generateMessage(SEND_FILE_REQUEST_TIME_OUT));
            }
            else if (state == IDLE)
            {
                FileRequest *message = (FileRequest *)msg->dup();
                const char* fileName = message->getFileName();
                file = findFile(fileName);
                if (file != nullptr) // if file was found
                {
                    EV << "Request for file ----> Changing state of host["<< getParentModule()->getIndex() <<"] to OFFERING\n";
                    state = OFFERING;
                    char newFileName[20];
                    sprintf(newFileName, "Offering for %s", fileName);
                    message->setName(newFileName);
                    message->setTxRate(txRate);
                    message->setFileSize(file->getSize());
                    message->setDestAddress(getParentModule()->getIndex());
                    message->setKind(HOST_FILE_OFFERING);
                    cModule* sourceModule = getParentModule()->getParentModule()->getSubmodule("host",message->getSourceAddress())->getSubmodule("host");
                    double srcX = sourceModule->par("x").doubleValue();
                    double scrY = sourceModule->par("y").doubleValue();
                    double dist = std::sqrt((x-srcX) * (x-srcX) + (y-scrY) * (y-scrY));
                    radioDelay = dist / propagationSpeed;
                    sendDirect(message, radioDelay, 0, sourceModule->gate("in"));
                    scheduleAt(simTime()+OFFER_REJECT_TIMEOUT, moreOffersTimeoutEv);
                }
            }
            break;
        }

    case NO_OFFER_RECEIVED_EVENT:
        {
            ASSERT(state == SELECTING);
            state = REQUESTING;
            EV << "No offer recieved event ---> No Host sent an offer. Failed "<<++failTimes<< "/3 times.\n";
            if(failTimes == 3)
            {
                EV << "Max tries reached. Requesting new file.\n";
                requestNewFile();
            }
            else
            {
                EV << "Retrying to request the file.\n";
                scheduleAt(getNextTransmissionTime(),lastFileReq);
            }
            break;
        }
    case HOST_FILE_OFFERING:
        {
            if (state == SELECTING)
            {
                cancelEvent(anyOffersTimeoutEv); //Host responded with an offer, request not failed.
                FileRequest *message = (FileRequest *)msg->dup();
                simtime_t duration = message->getFileSize() / message->getTxRate();
                cModule* destModule = getParentModule()->getParentModule()->getSubmodule("host",message->getDestAddress())->getSubmodule("host");
                double dstX = destModule->par("x").doubleValue();
                double dstY = destModule->par("y").doubleValue();
                double dist = std::sqrt((x-dstX) * (x-dstX) + (y-dstY) * (y-dstY));
                radioDelay = dist / propagationSpeed;
                simtime_t totalTime = radioDelay + duration;
                EV << "File offer comming from host[" << message->getDestAddress() << "] " << dist << " total time = " << totalTime << "\n";
                if (requestModule == nullptr)
                {
                    requestModule = destModule;
                    transferTime = totalTime;
                    getFileReq = message;
                    scheduleAt(simTime() + WAIT_FOR_MORE_OFFERS_TIMEOUT, generateMessage(RECEIVE_FILE_REQUEST_EVENT));
                }
                else if (transferTime > totalTime) //Better file source found rejecting old one
                {
                    simtime_t duration = getFileReq->getFileSize() / getFileReq->getTxRate();
                    FileRequest *rejectOffer = (FileRequest *)getFileReq->dup();
                    rejectOffer->setName("Reject offer");
                    rejectOffer->setKind(REJECT_OFFER);
                    sendDirect(rejectOffer, transferTime - duration, 0, requestModule->gate("in"));
                    getFileReq = message;
                    requestModule = destModule;
                    transferTime = totalTime;
                }
                else //File source rejected
                {
                    FileRequest *rejectOffer = (FileRequest *)getFileReq->dup();
                    rejectOffer->setName("Reject offer");
                    rejectOffer->setKind(REJECT_OFFER);
                    sendDirect(rejectOffer, radioDelay, 0, destModule->gate("in"));
                }
            }
            break;
        }
    case REJECT_OFFER:
        {
            state = IDLE;
            cancelEvent(moreOffersTimeoutEv);
            EV << "Reject Offer ----> Changing state of host["<< getParentModule()->getIndex() <<"] to IDLE\n";
            break;
        }


    case RECEIVE_FILE_REQUEST_EVENT:
        {
            ASSERT(state = SELECTING);
            state = RECEIVING;
            EV << "Self event ----> Found best host[" << requestModule->getParentModule()->getIndex() << "]\n";
            EV << "Self event ----> Changing state of host["<< getParentModule()->getIndex() <<"] to RECEIVING\n";
            FileRequest *sendFileReq = (FileRequest *)getFileReq->dup();
            sendFileReq->setName("Send file");
            sendFileReq->setKind(RECEIVE_FILE_REQUEST);
            simtime_t duration = getFileReq->getFileSize() / getFileReq->getTxRate();
            sendDirect(sendFileReq, transferTime - duration, 0, requestModule->gate("in"));
            break;
        }
    case RECEIVE_FILE_REQUEST:
        {
            state = TRANSMITING;
            EV << "Sending file ----> Changing state of host["<< getParentModule()->getIndex() <<"] to TRANSMITING\n";
            cancelEvent(moreOffersTimeoutEv);
            FileRequest *recvFileReq = (FileRequest *)msg->dup();
            FilePacket *filePacket = new FilePacket(recvFileReq->getFileName());
            file = findFile(recvFileReq->getFileName());
            filePacket->setSeqNumber(file->getSeqNum());
            filePacket->setBitLength(recvFileReq->getFileSize());
            filePacket->setKind(FILE_PACKAGE);
            simtime_t duration = filePacket->getBitLength() / txRate;
            sendDirect(filePacket, radioDelay, duration, recvFileReq->getSenderModule()->gate("in"));
            scheduleAt(simTime() + duration, fileSendCompletEv);
            break;
        }
    case FILE_PACKAGE_SENT:
        {
            ASSERT(state = TRANSMITING);
            EV << "Send complete ----> Changing state of host["<< getParentModule()->getIndex() <<"] to IDLE\n";
            state = IDLE;
            break;
        }
    case FILE_PACKAGE:
        {
            ASSERT(state = RECEIVING);
            FilePacket *filePacket = check_and_cast<FilePacket *>(msg);
            ASSERT(filePacket->isReceptionStart());
            lastFilePacketRec = filePacket->dup();
            simtime_t endReceptionTime = simTime() + filePacket->getDuration();
            scheduleAt(endReceptionTime, fileReceivedCompletEv);
            break;
        }
    case FILE_PACKAGE_RECEIVED:
        {
            ASSERT(state = RECEIVING);
            storeFileFromPacket(lastFilePacketRec);
            EV << "Packet fully received ----> Changing state of host["<< getParentModule()->getIndex() <<"] to IDLE\n";
            state = IDLE;
            requestNewFile();
            break;
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
            do{
                    sprintf(fileName, "file%d.dll",intuniform(0,par("end")));
            }while(findFile(fileName) != nullptr);

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
        case RECEIVE_FILE_REQUEST_EVENT : return getFileEv;
        default : return nullptr;
    }
}

File* Host::findFile(const char* fileName)
{
    File *file = nullptr;
    for(int i = 0; i < getParentModule()->par("maxNumFiles").intValue(); i++)
    {
        file = (File*)getParentModule()->getSubmodule("files",i);
        if (file->getFileName() == std::string(fileName))
        {
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
    getFileEv = new cMessage("getEvent", RECEIVE_FILE_REQUEST_EVENT);
    moreOffersTimeoutEv = new cMessage("offerTimeOut", REJECT_OFFER);
    anyOffersTimeoutEv = new cMessage("offerTimeOut", NO_OFFER_RECEIVED_EVENT);
    fileReceivedCompletEv = new cMessage("file fully received",FILE_PACKAGE_RECEIVED);
    fileSendCompletEv = new cMessage("file fully sent",FILE_PACKAGE_SENT);
    lastFileReq = nullptr;
    requestModule = nullptr;
}

Host::~Host()
{
    cancelAndDelete(fileReqEv);
    cancelAndDelete(fileReqTimeoutEv);
    cancelAndDelete(getFileEv);
    cancelAndDelete(moreOffersTimeoutEv);
    cancelAndDelete(anyOffersTimeoutEv);
    cancelAndDelete(fileReceivedCompletEv);
    cancelAndDelete(fileSendCompletEv);
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
                if (file1->getFileName() == file2->getFileName())
                {
                    file2->setNameAndSeq(file2->getSeqNum()+1);
                    toBeFixed = true;
                }
            }
        }
    }
    while(toBeFixed);

    for(int i = 0; i < getParentModule()->par("maxNumFiles").intValue()/2 ; i++)
    {
        file1 = (File*)getParentModule()->getSubmodule("files",i);
        file1->deleteFile();
    }

}

void Host::requestNewFile()
{
    lastFileReq = nullptr;
    if (findFile("N/A") == nullptr)
    {
        EV << "Data storage is full, cannot receive new files!\n";
        return;
    }
    scheduleAt(getNextTransmissionTime(),generateMessage(SEND_FILE_REQUEST_EVENT));
}

void Host::storeFileFromPacket(FilePacket *packet)
{
    File *newFile = findFile("N/A");
    if (newFile == nullptr)
        EV << "Data storage is full, cannot receive new files!\n";
    else
    {
        newFile->setNameAndSeq(packet->getSeqNumber());
        newFile->setSize(packet->getBitLength());
    }
}





