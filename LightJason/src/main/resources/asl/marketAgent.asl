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

+!start/auction() : true <-
    generic/print(MyName, ": starting an auction");
    send/resources()
.

//Message triggers

+!receive/auction() : true <-
    generic/print(MyName, ": got notified of an auction");
    send/bid()
.

+!receive/bid() : true <-
    generic/print(MyName, ": received a bid message");
    store/bid()
.

+!receive/round/result() : true <-
    generic/print(MyName, ": received results of round")
.

+!receive/result() : true <-
    generic/print(MyName, ": received auction results")
.