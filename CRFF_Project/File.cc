#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

/**
 * In this step we keep track of how many messages we send and received,
 * and display it above the icon.
 */
class File : public cSimpleModule
{
private:
    int seqNumber;
    cPar *name;        // file name
    cPar *size;           // file size in bits

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
public:
    string getName();
    int getSize();
};

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

int File::getSize() {
    return size->intValue();
}




