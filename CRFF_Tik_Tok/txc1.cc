#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc1);

void Txc1::initialize()
{

    if (strcmp("tic", getName()) == 0) {
        cMessage *msg = new cMessage("tictocMsg");
        EV << "Sending initial message\n";
        send(msg, "out");
    }
}

void Txc1::handleMessage(cMessage *msg)
{
    EV << "Received message `" << msg->getName() << "', sending it out again\n";
    if(strcmp("tic", getName()) == 0) { //Daca sunt nod tic primesc mesaj toc
            if(msg->isSelfMessage())
            {
                send(msg, "out");
            }
            else
            {

                scheduleAt(simTime() + 50, msg);
            }
    }
    else
        send(msg, "out");
}
