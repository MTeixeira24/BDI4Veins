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
    generic/print(MyName, ": bargain agent started")
.

//Initialize beliefs
+!setup/beliefs(ISMERGER) : true <-
    +isMerger(ISMERGER)
.

//Environmental triggers
+!bargain/start(VID) : true <-
    offer/send(VID, 0)
.

+!bargain/receive(VID, AMOUNT) : true <-
    offer/evaluate(VID, AMOUNT)
.

+!bargain/receive/result(VID, RESULT) : true <-
    offer/send(VID, RESULT)
.

+!bargain/receive/payout(VID, PAYOUT) : true <-
    offer/receive(VID, PAYOUT)
.