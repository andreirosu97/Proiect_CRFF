/*
 * Server.cc
 *
 *  Created on: Nov 14, 2019
 *      Author: Andrei
*/
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Server : public cSimpleModule
{
  private:
    // state variables, event pointers
    bool channelBusy;
    cMessage *endRxEvent;

    enum { IDLE = 0, TRANSMISSION = 1, COLLISION = 2 };

  public:
    Server();
    virtual ~Server();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


Define_Module(Server);

Server::Server()
{
    endRxEvent = nullptr;
}

Server::~Server()
{
    cancelAndDelete(endRxEvent);
}

void Server::initialize()
{
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

    gate("in")->setDeliverOnReceptionStart(true);
}

void Server::handleMessage(cMessage *msg)
{
    EV << "AM PRIMIT MESAJ " << msg->getName() << "\n";

    sendDirect(new cMessage("Request file from server"), 0, 0, msg->getSenderModule()->gate("in"));
}





