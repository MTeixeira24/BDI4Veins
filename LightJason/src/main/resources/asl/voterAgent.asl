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
+!main.

+!setup/beliefs(IsLeader, UtilityFunct, UtilityFactor, SingleRule, ComRule, RouteMap, Members) : true <-
    initialize/agent(IsLeader, UtilityFunct, UtilityFactor, SingleRule, ComRule, RouteMap, Members);
    return(0)
.