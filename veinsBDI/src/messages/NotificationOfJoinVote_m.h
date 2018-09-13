//
// Generated file, do not edit! Created by nedtool 5.4 from messages/NotificationOfJoinVote.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __NOTIFICATIONOFJOINVOTE_M_H
#define __NOTIFICATIONOFJOINVOTE_M_H

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
 * Class generated from <tt>messages/NotificationOfJoinVote.msg:24</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet NotificationOfJoinVote extends NegotiationMessage
 * {
 *     //Relevant information about joiner vehicle preferences
 *     double preferedSpeed;
 *     double tolerance;
 * }
 * </pre>
 */
class NotificationOfJoinVote : public ::NegotiationMessage
{
  protected:
    double preferedSpeed;
    double tolerance;

  private:
    void copy(const NotificationOfJoinVote& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NotificationOfJoinVote&);

  public:
    NotificationOfJoinVote(const char *name=nullptr, short kind=0);
    NotificationOfJoinVote(const NotificationOfJoinVote& other);
    virtual ~NotificationOfJoinVote();
    NotificationOfJoinVote& operator=(const NotificationOfJoinVote& other);
    virtual NotificationOfJoinVote *dup() const override {return new NotificationOfJoinVote(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual double getPreferedSpeed() const;
    virtual void setPreferedSpeed(double preferedSpeed);
    virtual double getTolerance() const;
    virtual void setTolerance(double tolerance);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const NotificationOfJoinVote& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, NotificationOfJoinVote& obj) {obj.parsimUnpack(b);}


#endif // ifndef __NOTIFICATIONOFJOINVOTE_M_H

