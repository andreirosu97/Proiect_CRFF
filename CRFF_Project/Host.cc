#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In this step we keep track of how many messages we send and received,
 * and display it above the icon.
 */
class Host : public cSimpleModule
{

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Host);

void Host::initialize()
{
    if(getIndex() == 0)
    {
        cMessage *msg = new cMessage("file");
        send(msg, "gate$o", 0);
    }
}

void Host::handleMessage(cMessage *msg)
{
    int n = gateSize("gate");
    int k = intuniform(0, n-1);
    send(msg, "gate$o", k);
}




