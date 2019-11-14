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
    cMessage *endTxEvent = new cMessage("send/endTx");
    simtime_t requestInterval;

    // speed of light in m/s
    const double propagationSpeed = 299792458.0;
    simtime_t radioDelay;
    double txRate;
    cPar *pkLenBits;
    enum { IDLE = 0, TRANSMIT = 1 } state;

public:
  Host();
  virtual ~Host();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    simtime_t getNextTransmissionTime();
};

Define_Module(Host);

void Host::initialize()
{
    server = getModuleByPath("server");
    requestInterval = par("requestInterval");
    state = IDLE;

    double x = par("x").doubleValue();
    double y = par("y").doubleValue();

    double serverX = server->par("x").doubleValue();
    double serverY = server->par("y").doubleValue();
    double dist = std::sqrt((x-serverX) * (x-serverX) + (y-serverY) * (y-serverY));
    radioDelay = dist / propagationSpeed;

    cMessage *mesaj = new cMessage("file request event");
    scheduleAt(getNextTransmissionTime(),mesaj);
}

void Host::handleMessage(cMessage *msg)
{
    if(strcmp(msg->getName(),"file request event") == 0)
    {
        cMessage *mesaj = new cMessage("Request file");
        EV << "Mesage : " << msg->getName();
        sendDirect(mesaj, radioDelay, 0, server->gate("in"));
    }
}

simtime_t Host::getNextTransmissionTime()
{
    return simTime() + requestInterval;
}

Host::Host()
{
    endTxEvent = nullptr;
}

Host::~Host()
{
    cancelAndDelete(endTxEvent);
}




