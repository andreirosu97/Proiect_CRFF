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
private:
    cModule *server;

    // speed of light in m/s
    const double propagationSpeed = 299792458.0;
    simtime_t radioDelay;
    double txRate;
    cPar *pkLenBits;
    enum { IDLE = 0, TRANSMIT = 1 } state;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Host);

void Host::initialize()
{
    server = getModuleByPath("server");
    state = IDLE;

    double x = par("x").doubleValue();
    double y = par("y").doubleValue();

    double serverX = server->par("x").doubleValue();
    double serverY = server->par("y").doubleValue();
    double dist = std::sqrt((x-serverX) * (x-serverX) + (y-serverY) * (y-serverY));
    radioDelay = dist / propagationSpeed;

    if(getIndex() == 0)
    {
        cMessage *msg = new cMessage("Request file");
        sendDirect(msg, radioDelay, 0, server->gate("in"));
    }
}

void Host::handleMessage(cMessage *msg)
{
    EV << "Mesage : " << msg->getName();
}




