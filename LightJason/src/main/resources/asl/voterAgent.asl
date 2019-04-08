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


+!handle/vote/start(VoteType, Args) <-
    L1 = collection/list/create(Args);
    vote/open(VoteType, L1)
.

+!handle/vote/notification(CANDIDATES, CONTEXT) <-
    generic/print("Agent", MyName, " got notification to vote on: ", CANDIDATES);
    vote/send(CANDIDATES, CONTEXT)
.

+!handle/vote/submit(VOTE, ID) <-
    generic/print(MyName,": Got vote", VOTE, "from", ID);
    //CANDIDATESIZE = utility/get/candidates/size();
    vote/store(VOTE);
.

+!handle/results(VALUE) <-
    generic/print("Vote ended");
    return(0)
.