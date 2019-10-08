//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * Derive the Txc3 class from cSimpleModule. In the Tictoc1 network,
 * both the `tic' and `toc' modules are Txc3 objects, created by OMNeT++
 * at the beginning of the simulation.
 */
class Txc3 : public cSimpleModule
{
    private:
        int counter;  // Note the counter here
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Txc3);


void Txc3::initialize()
{
    // Initialize is called at the beginning of the simulation.
    // To bootstrap the tic-toc-tic-toc process, one of the modules needs
    // to send the first message. Let this be `tic'.
    counter = 10;
    WATCH(counter);

    // Am I Tic or Toc?
    if (strcmp("tic", getName()) == 0) {
        // create and send first message on gate "out". "tictocMsg" is an
        // arbitrary string which will be the name of the message object.
        cMessage *msg = new cMessage("tictocMsg");
        EV << "Sending initial message\n";
        send(msg, "out");
    }
}

void Txc3::handleMessage(cMessage *msg)
{

    if(!msg->isSelfMessage())
        counter--;

    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through
    // gate `out'. Because both `tic' and `toc' does the same, the message
    // will bounce between the two.
    if (counter == 0) {
            // If counter is zero, delete message. If you run the model, you'll
            // find that the simulation will stop at this point with the message
            // "no more events".
            EV << getName() << "'s counter reached zero, deleting message\n";
            delete msg;
        }
    else
    {

        if(strcmp("tic", getName()) == 0) { //Daca sunt nod tic primesc mesaj toc
                if(msg->isSelfMessage())
                {
                    EV << getName() << "'s counter is " << counter << ", sending back message\n";
                    send(msg, "out"); // send out the message
                }
                else
                {

                    scheduleAt(simTime() + 50, msg);
                }
        }
        else
        {
            EV << getName() << "'s counter is " << counter << ", sending back message\n";
            send(msg, "out"); // send out the message
        }
    }
}
