//Defines a standard voter agent

//Belief base definition. Commented beliefs are beliefs that are not held at agent instantiation
//but are expected to be created during execution.
//inplatoon(_id).
//lookforplatoon.
//targetplatoonkoin(_id).
//travelroute([_l1, .., _ln]).
//ischair.
//platoonMembers([L, M1, ..., Mn]).
minimumUtility(0.8).

//Initial Goal
!main.

//Definition of rules
generateutility(JSPEED, JPREFERENCE, PSPEED, PredictedUtility)
    :-  >>tolerance(Tolerance); >>preferedspeed(SpeedPreference);
        PredictedUtility = utility/predictplatoonspeed(JSPEED, JPREFERENCE, SpeedPreference, Tolerance, PSPEED).

//Plan definition
//Agent Belief Set up
+!main <-
    Tolerance = utility/generatetolerance();
    SpeedPreference = utility/generatespeedpreference();
    +tolerance(Tolerance);
    +preferedspeed(SpeedPreference);
    generic/print("Agent ", MyName, " of type ", MyType ," started, has preference for speed: ", SpeedPreference ," and tolerance of: ", Tolerance).

//Goal triggered plans for all voting agents
+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon").
    //transmit/self/searchForPlatoon(). //Set controller layer to send to agent all platoons that are open for joining

+!choosevote(PredictedUtility)
    : >>(minimumUtility(MinUtil), MinUtil < PredictedUtility ) <-
        generic/print("Agent ", MyName, "Casting positive vote:", PredictedUtility, ">", MinUtil);
        !sendvote(1)
    : >>(minimumUtility(MinUtil), MinUtil >= PredictedUtility) <-
        generic/print("Agent ", MyName, "Casting negative vote:", PredictedUtility, "<", MinUtil);
        !sendvote(0).

+!sendvote(VOTE)
    : >>isChair(_) <-
        generic/print("Agent", MyName, "Im the chair so no need to pass through omnet");
        vote/chairstore(VOTE)
    : ~>>isChair(_) <- 
        generic/print("Agent", MyName, "Sending the vote down omnet");
       transmit/other/vote/cast(VOTE). 


+!handleopenvotetojoin(JSPEED, JPREFERENCE, PSPEED) <-
    generic/print("Agent ", MyName, "got notified of a join vote for a vehicle who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE); 
    $generateutility(JSPEED, JPREFERENCE, PSPEED, PredictedUtility);
    generic/print("Agent ", MyName, "utility is:", PredictedUtility);
    !choosevote(PredictedUtility).

//Belief triggered plans for all voting agents
+!foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    generic/print("AAAAAAAAAAAAAAAAAAAAAAAAA");
    !attemptjoin(PID, LID).

+!addmember(L) <-
    utility/storemember(L).
        

+!ballotopen() <-
    generic/print("Agent ", MyName, " got notification of a new ballot starting").

+!openvotetojoin(JSPEED, JPREFERENCE) <-
    >>platoonspeed(PSPEED);
    !handleopenvotetojoin(JSPEED, JPREFERENCE, PSPEED).


+!inplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).


+!handle/speed/vote/notification(CANDIDATES) <-
    generic/print("Agent", MyName, " got notification to vote on platoon speed: ", CANDIDATES);
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Tolerance, Speed, 1.1);
    generic/print(VVECTOR);
    //!sendvote(VOTE).
    transmit/other/vote/list(VVECTOR).

+!handle/submit/vote(VOTE) <-
    generic/print("Got vote", VOTE);
    vote/store(VOTE).

+!handle/tie(CANDIDATES) <-
    generic/print("There has been a tie. Submitting my vote to tie brake");
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Tolerance, Speed, 1.1);
    vote/store(VVECTOR).

+!handle/results(VALUE) <-
    >>preferedspeed(Speed); >>tolerance(Tolerance);
    Util = utility/generate/utility(1.1, VALUE, Tolerance, Speed);
    !decide/stay(Util).
    generic/print("Agent", MyName, "is", Result).

+!decide/stay(UTIL)
    <- minimumUtility(MUTIL); UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <- minimumUtility(MUTIL); UTIL <= MUTIL; generic/print("Agent", MyName, "leaves"). 
//Goal triggered plans for chair agents

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE);
    S = utility/platoonsize();
    vote/openballot("allowJoin", JID, S);
    transmit/other/vote/join(JSPEED, JPREFERENCE).
    //start a list of votes;
    //set belief of open vote in state of awaiting ack from members +openJoinBallot(JSPEED, JPREFERENCE, [])

+!handlesubmitvote(VOTER, VOTE)
    : MyName == VOTER <-
        generic/print("Got my own vote");
        vote/store/dep(VOTER, VOTE)
    : MyName != VOTER <-
        generic/print("Got vote from", VOTER);
        vote/store/dep(VOTER,VOTE).

+!notify/joiner/win(JID) <-
    generic/print("APROVED!");
    vote/send/results(JID, 1).

+!notify/joiner/lose(JID) <-
    generic/print("REJECTED!");
    vote/send/results(JID, 0).

+!openvotetojoin(JSPEED, JPREFERENCE, PSPEED) <-
    +platoonspeed(PSPEED);
    !handleopenvotetojoin(JSPEED, JPREFERENCE, PSPEED).
    //save the sent vote

+!ischair(PID) <-
    +isChair(PID);
    generic/print("Agent ", MyName, " is chair of platoon ", PID).

+!submitvote(VOTER, VOTE) <-
    generic/print("GOT VOTE");
    !handlesubmitvote(VOTER, VOTE).

+!requestjoin(JID, JSPEED, JPREFERENCE) <-
    !handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!maneuver/complete(L) <-
    generic/print("Agent ", MyName, " maneuver complete starting a vote to set speed");
    utility/storemember(L);
    vote/open/ballot/speed().

//Joiner specific plans

+!handlerejection(PID) <-
    generic/print("Agent ", MyName, " was rejected from platoon ", PID, "trying the next one");
    NPID = utility/next/platoon();
    generic/print("Next is", NPID);
    !startjoin(NPID).

+!startjoin(PID) 
    : PID >= 0 <-
        generic/print("DEBUG2", PID);
        LID = utility/get/leader(PID);
        generic/print("Agent ", MyName, "next platoon is", PID, "whos leader is:", LID);
        !attemptjoin(PID, LID)
    : PID < 0 <-
        generic/print("DEBUG3");
        generic/print("Agent ", MyName, "rejected by all. Aborting negotiations").

+!pushplatoon(PID, PSPEED, LID) <-
    >>tolerance(Tolerance);
    >>preferedspeed(Speed);
    utility/store/platoon(PID, PSPEED, LID, Tolerance, Speed).

+!startrequests() <- 
    PID = utility/next/platoon();
    generic/print("DEBUG1", PID);
    !startjoin(PID).

+!attemptjoin(PID, LID) <-
    generic/print("AAAAAAAAAAAAAAAAAAAAAAAAA");
    +targetplatoonjoin(PID);
    +leaderid(LID);
    generic/print("Agent ", MyName, " sending a request to join");
    >>preferedspeed(SPEED);
    >>tolerance(TOLERANCE);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, TOLERANCE).