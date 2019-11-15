/*
 * Server.cc
 *
 *  Created on: Nov 14, 2019
 *      Author: Andrei
*/
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "Globals.h"

#define BOARDCAST_START_DELAY 1

using namespace omnetpp;

class Server : public cSimpleModule
{
  private:
    // state variables, event pointers
    double x;
    double y;
    const double propagationSpeed = 299792458.0;

    enum { IDLE = 0, BUSY = 1} state;

    cMessage* broadcastStart;
    FileRequest *lastFileReq;

  public:
    Server();
    virtual ~Server();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


Define_Module(Server);

void Server::initialize()
{
    state = IDLE;

    x = par("x").doubleValue();
    y = par("y").doubleValue();
}

void Server::handleMessage(cMessage *msg)
{
    double hostX;
    double hostY;
    if(state !=  IDLE && !msg->isSelfMessage())
    {
        EV << "Server is busy, request message was ignored, it will be resent by the host.\n";
        return;
    }

    if (msg->getKind() == SEND_FILE_REQUEST)
    {
        state = BUSY;
        cMessage* akgMessage = new cMessage("Request sent to hosts", SEND_FILE_REQUEST_AKG);

        cModule *module = msg->getSenderModule();
        hostX = module->par("x").doubleValue();
        hostY = module->par("y").doubleValue();
        double dist = std::sqrt((x-hostX) * (x-hostX) + (y-hostY) * (y-hostY));
        simtime_t radioDelay = dist / propagationSpeed;
        lastFileReq = (FileRequest *)msg->dup();

        sendDirect(akgMessage, radioDelay, 0, msg->getSenderModule()->gate("in"));
        scheduleAt(simTime()+BOARDCAST_START_DELAY, broadcastStart);
    }
    else if (msg->getKind() == SERVER_START_BC) //This way we give time to the host to change state
    {
        for(int i = 0; i < getParentModule()->par("numHosts").intValue(); i++)
        {
            cModule *module = getParentModule()->getSubmodule("host",i)->getSubmodule("host");

            hostX = module->par("x").doubleValue();
            hostY = module->par("y").doubleValue();
            double dist = std::sqrt((x-hostX) * (x-hostX) + (y-hostY) * (y-hostY));
            simtime_t radioDelay = dist / propagationSpeed;

            if (lastFileReq->getSourceAddress() != i)
            {
                FileRequest * message = (FileRequest *)lastFileReq->dup();
                sendDirect(message, radioDelay, 0, module->gate("in"));
            }
        }
        state = IDLE;
    }
}

Server::Server()
{
    broadcastStart = new cMessage("start broadcast", SERVER_START_BC);
}

Server::~Server()
{
    cancelAndDelete(broadcastStart);
}





