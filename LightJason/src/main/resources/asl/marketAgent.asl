//--------------//
//Define beliefs//
//--------------//


//----------------//
//Entry point goal//
//----------------//
!main.


//---------------------------//
//Definition of logical rules//
//---------------------------//


//---------------//
//Plan definition//
//---------------//

//Entry point plan
+!main : true <-
    generic/print(MyName, ": market agent started")
.

//Initialize beliefs
+!setup/beliefs(PID, LID, PSPEED, MANAGER, MEMBERS, WTP, ENDOWMENT, PREFERREDSPEED, PREFERREDNODES) : true <-
    +platoonId(PID);
    +leaderId(LID);
    +platoonSpeed(PSPEED);
    +members(MEMBERS);
    +isManager(MANAGER);
    +preferredPath(PREFERREDNODES);
    +preferredSpeed(PREFERREDSPEED);
    set/market/parameters(WTP, ENDOWMENT, PREFERREDSPEED, PREFERREDNODES)
.
//Environmental triggers

+!start/auction(CONTEXT) : true <-
    generic/print(MyName, ": starting an auction");
    >>members(MEMBERS);
    setup/auction(CONTEXT, MEMBERS)
.

//Message triggers

+!receive/auction(AUCTIONID, CONTEXT, MANAGERID) : true <-
    generic/print(MyName, ": got notified of an auction");
    setup/bidder(AUCTIONID, CONTEXT, MANAGERID)
.

+!receive/bid() : true <-
    generic/print(MyName, ": received a bid message");
    store/bid()
.

+!receive/bids(BIDLIST) : true <-
    generic/print(MyName, ": received a list of <id,bid> tuples");
    store/bid/list(BIDLIST)
.

+!receive/round/result(STATUS) : true <-
    generic/print(MyName, ": received results of round");
    send/bid(STATUS)
.

+!receive/result(STATUS) : true 
    <- STATUS == 2; generic/print(MyName, "Won the auction"); >>preferredPath(P); >>preferredSpeed(S); send/pay(P,S)
    <- STATUS != 2; generic/print(MyName, "Lost the auction")
.

+!receive/pay(PAY, PROPERTY) : true <-
    generic/print(MyName, "Got pay");
    finalize/auction(PAY, PROPERTY)
.

+!distribute/pay(PAY, PROPERTY) : true <-
    distribute/payment(PAY, PROPERTY)
.

+!receive/wtp(WTPLIST) : true <-
    store/wtp(WTPLIST)
.