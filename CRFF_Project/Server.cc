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

using namespace omnetpp;


class Server : public cSimpleModule
{
  private:
    // state variables, event pointers
    bool channelBusy;
    double x;
    double y;
    const double propagationSpeed = 299792458.0;

    enum { IDLE = 0, TRANSMISSION = 1, COLLISION = 2 };

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};


Define_Module(Server);

void Server::initialize()
{
    channelBusy = false;

    x = par("x").doubleValue();
    y = par("y").doubleValue();

    gate("in")->setDeliverOnReceptionStart(true);
}

void Server::handleMessage(cMessage *msg)
{
    double hostX;
    double hostY;
    if (msg->getKind() == SEND_FILE_REQUEST)
    {
        for(int i = 0; i < getParentModule()->par("numHosts").intValue(); i++)
        {
            cModule *module = getParentModule()->getSubmodule("host",i)->getSubmodule("host");

            hostX = module->par("x").doubleValue();
            hostY = module->par("y").doubleValue();
            double dist = std::sqrt((x-hostX) * (x-hostX) + (y-hostY) * (y-hostY));
            simtime_t radioDelay = dist / propagationSpeed;

            if (((FileRequest *)msg)->getSourceAddress() != i)
            {
                FileRequest *message = (FileRequest *)msg->dup();
                sendDirect(message, radioDelay, 0, module->gate("in"));
            }
            else
            {
                cMessage* akgMessage = new cMessage("Request sent to hosts", SEND_FILE_REQUEST_AKG);
                sendDirect(akgMessage, radioDelay, 0, module->gate("in"));
            }
        }
    }
}





