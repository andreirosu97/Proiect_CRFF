#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "File.h"

/**
 * In this step we keep track of how many messages we send and received,
 * and display it above the icon.
 */

Define_Module(File);

void File::initialize()
{
    seqNumber = par("seqNumber");
    name = &par("name");
    name->setStringValue("file" + std::to_string(seqNumber) + ".dll");
    size = &par("size");
}

void File::handleMessage(cMessage *msg)
{
    EV << "ERROR I SHOULD NOT RECIEVE A MESSAGE \n";
}

string File::getName() {
    return name->stdstringValue();
}

int File::getSeqNum()
{
    return seqNumber;
}

int File::getSize() {
    return size->intValue();
}

void File::setNameAndSeq(int seqNum)
{
    seqNumber = seqNum;
    (&par("seqNumber"))->setIntValue(seqNumber);
    name->setStringValue("file" + std::to_string(seqNumber) + ".dll");
}




