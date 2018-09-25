//
// Generated file, do not edit! Created by nedtool 5.4 from messages/Ack.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __ACK_M_H
#define __ACK_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "NegotiationMessage_m.h"
// }}

/**
 * Class generated from <tt>messages/Ack.msg:20</tt> by nedtool.
 * <pre>
 * packet Ack extends NegotiationMessage
 * {
 * }
 * </pre>
 */
class Ack : public ::NegotiationMessage
{
  protected:

  private:
    void copy(const Ack& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Ack&);

  public:
    Ack(const char *name=nullptr, short kind=0);
    Ack(const Ack& other);
    virtual ~Ack();
    Ack& operator=(const Ack& other);
    virtual Ack *dup() const override {return new Ack(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Ack& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Ack& obj) {obj.parsimUnpack(b);}


#endif // ifndef __ACK_M_H

