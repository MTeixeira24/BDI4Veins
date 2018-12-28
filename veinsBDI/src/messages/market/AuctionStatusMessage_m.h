//
// Generated file, do not edit! Created by nedtool 5.4 from messages/market/AuctionStatusMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __AUCTIONSTATUSMESSAGE_M_H
#define __AUCTIONSTATUSMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "MarketMessage_m.h"
// }}

/**
 * Class generated from <tt>messages/market/AuctionStatusMessage.msg:20</tt> by nedtool.
 * <pre>
 * //Base message for market applications
 * packet AuctionStatusMessage extends MarketMessage
 * {
 *     int auctionId;
 *     int context;
 *     int managerId;
 * }
 * </pre>
 */
class AuctionStatusMessage : public ::MarketMessage
{
  protected:
    int auctionId;
    int context;
    int managerId;

  private:
    void copy(const AuctionStatusMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AuctionStatusMessage&);

  public:
    AuctionStatusMessage(const char *name=nullptr, short kind=0);
    AuctionStatusMessage(const AuctionStatusMessage& other);
    virtual ~AuctionStatusMessage();
    AuctionStatusMessage& operator=(const AuctionStatusMessage& other);
    virtual AuctionStatusMessage *dup() const override {return new AuctionStatusMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getAuctionId() const;
    virtual void setAuctionId(int auctionId);
    virtual int getContext() const;
    virtual void setContext(int context);
    virtual int getManagerId() const;
    virtual void setManagerId(int managerId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const AuctionStatusMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, AuctionStatusMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __AUCTIONSTATUSMESSAGE_M_H

