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
+!setup/beliefs(PID, LID, PSPEED, MANAGER, MEMBERS, WTP, ENDOWMENT) : true <-
    +platoonId(PID);
    +leaderId(LID);
    +platoonSpeed(PSPEED);
    +members(MEMBERS);
    +isManager(MANAGER);
    set/market/parameters(WTP, ENDOWMENT)
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

+!receive/round/result() : true <-
    generic/print(MyName, ": received results of round")
.

+!receive/result() : true <-
    generic/print(MyName, ": received auction results")
.