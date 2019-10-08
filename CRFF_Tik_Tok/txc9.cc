//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In the previous models, `tic' and `toc' immediately sent back the
 * received message. Here we'll add some timing: tic and toc will hold the
 * message for 1 simulated second before sending it back. In OMNeT++
 * such timing is achieved by the module sending a message to itself.
 * Such messages are called self-messages (but only because of the way they
 * are used, otherwise they are completely ordinary messages) or events.
 * Self-messages can be "sent" with the scheduleAt() function, and you can
 * specify when they should arrive back at the module.
 *
 * We leave out the counter, to keep the source code small.
 */
enum messageType {
    C,
    D,
    ED
};

class Tic9 : public cSimpleModule
{
  private:
    cMessage *messages[3];
    cMessage *message;
    simtime_t delayC;
    simtime_t delayD;
    simtime_t delayED;

  public:
    Tic9();
    virtual ~Tic9();

  protected:
      virtual cMessage *generateNewMessage(int msgType);
      virtual void sendCopyOf(cMessage *msg);
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void scheduleMessage(cMessage *msg);
};

Define_Module(Tic9);

Tic9::Tic9()
{
    message = messages[0] = messages[1] = messages[2] = nullptr;
}

Tic9::~Tic9()
{
    for(int i = 0; i < 3; i++)
        delete messages[i];
}

void Tic9::initialize()
{
    EV << "Sending initial message\n";
    messages[0] = generateNewMessage(C);
    messages[1] = generateNewMessage(D);
    messages[2] = generateNewMessage(ED);

    delayC = par("delayC");
    delayD = par("delayD");
    delayED = par("delayED");

    for(int i = 0; i < 3; i++)
        scheduleMessage(messages[i]);
}

void Tic9::handleMessage(cMessage *msg)
{
    delete message;
    message = generateNewMessage(msg->getKind());
    EV << "Received: " << msg->getName() << " " << msg->getKind() << "\n";
    if(strcmp(msg->getName(),"event") == 0)
    {
        sendCopyOf(message);
    }
    else
    {
        EV << "Acknowledgement received!\n";
        scheduleMessage(message);
    }
    delete msg;
}


cMessage *Tic9::generateNewMessage(int msgType)
{
    // Generate a message with a different name and type every time.
    cMessage *msg;
    switch(msgType)
        {
        case C : msg = new cMessage("C Message",C);
                break;
        case D: msg = new cMessage("D Message",D);
                break;
        case ED: msg = new cMessage("ED Message",ED);
                break;
        }
    return msg;
}

void Tic9::scheduleMessage(cMessage *msg)
{
    cMessage *event;
    switch(msg->getKind())
    {
    case C : event = new cMessage("event",C);
            scheduleAt(simTime() + delayC, event);
            break;
    case D: event = new cMessage("event",D);
            scheduleAt(simTime() + delayD, event);
            break;
    case ED: event = new cMessage("event",ED);
            scheduleAt(simTime() + delayED, event);
            break;
    }
}

void Tic9::sendCopyOf(cMessage *msg)
{
    // Duplicate message and send the copy.
    cMessage *copy = (cMessage *)msg->dup();
    send(copy, "out");
}

/**
 * Sends back an acknowledgement -- or not.
 */
class Toc9 : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Toc9);

void Toc9::handleMessage(cMessage *msg)
{
    if (uniform(0, 1) < 0.05) {
        EV << "\"Losing\" message.\n";
        bubble("message lost");
        delete msg;
    }
    else {
        EV << "Sending back same message as acknowledgement: "
                << msg->getName() << " " << msg->getKind() << "\n";
        send(msg, "out");
    }
}
