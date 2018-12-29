//
// Generated file, do not edit! Created by nedtool 5.4 from messages/market/BidMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __BIDMESSAGE_M_H
#define __BIDMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "MarketMessage_m.h"
typedef std::vector<int> IntList;
// }}

/**
 * Class generated from <tt>messages/market/BidMessage.msg:22</tt> by nedtool.
 * <pre>
 * //Base message for market applications
 * packet BidMessage extends MarketMessage
 * {
 *     int auctionId;
 *     int context;
 *     int bidValue;
 *     int wtp;
 *     int property;
 *     IntList propertyList;
 * }
 * </pre>
 */
class BidMessage : public ::MarketMessage
{
  protected:
    int auctionId;
    int context;
    int bidValue;
    int wtp;
    int property;
    IntList propertyList;

  private:
    void copy(const BidMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BidMessage&);

  public:
    BidMessage(const char *name=nullptr, short kind=0);
    BidMessage(const BidMessage& other);
    virtual ~BidMessage();
    BidMessage& operator=(const BidMessage& other);
    virtual BidMessage *dup() const override {return new BidMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getAuctionId() const;
    virtual void setAuctionId(int auctionId);
    virtual int getContext() const;
    virtual void setContext(int context);
    virtual int getBidValue() const;
    virtual void setBidValue(int bidValue);
    virtual int getWtp() const;
    virtual void setWtp(int wtp);
    virtual int getProperty() const;
    virtual void setProperty(int property);
    virtual IntList& getPropertyList();
    virtual const IntList& getPropertyList() const {return const_cast<BidMessage*>(this)->getPropertyList();}
    virtual void setPropertyList(const IntList& propertyList);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BidMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BidMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __BIDMESSAGE_M_H

