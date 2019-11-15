/*
 * File.h
 *
 *  Created on: Nov 14, 2019
 *      Author: Andrei
 */

#ifndef FILE_H_
#define FILE_H_
using namespace omnetpp;
using namespace std;

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
    int getSeqNum();
    void setNameAndSeq(int seqNum);
};



#endif /* FILE_H_ */
