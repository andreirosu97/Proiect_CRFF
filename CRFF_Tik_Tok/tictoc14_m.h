//
// Generated file, do not edit! Created by nedtool 5.5 from tictoc14.msg.
//

#ifndef __TICTOC14_M_H
#define __TICTOC14_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0505
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>tictoc14.msg:13</tt> by nedtool.
 * <pre>
 * message TicTocMsg14
 * {
 *     int source;
 *     int destination;
 *     int hopCount = 0;
 * }
 * </pre>
 */
class TicTocMsg14 : public ::omnetpp::cMessage
{
  protected:
    int source;
    int destination;
    int hopCount;

  private:
    void copy(const TicTocMsg14& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const TicTocMsg14&);

  public:
    TicTocMsg14(const char *name=nullptr, short kind=0);
    TicTocMsg14(const TicTocMsg14& other);
    virtual ~TicTocMsg14();
    TicTocMsg14& operator=(const TicTocMsg14& other);
    virtual TicTocMsg14 *dup() const override {return new TicTocMsg14(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const TicTocMsg14& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, TicTocMsg14& obj) {obj.parsimUnpack(b);}


#endif // ifndef __TICTOC14_M_H

