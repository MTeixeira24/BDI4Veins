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

+!set/speed(SPEED) <-
    generic/print("Agent ",MyName, " the speed of my platoon is", SPEED);
    utility/set/platoon/speed(SPEED);
    +platoonspeed(SPEED).

+!requestjoin(JID, JSPEED, JPREFERENCE) <-
    !handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE);
    L1 = collection/list/create(JSPEED, JPREFERENCE);
    open/vote("join", L1).
    //transmit/other/vote/join(JSPEED, JPREFERENCE).
    //start a list of votes;
    //set belief of open vote in state of awaiting ack from members +openJoinBallot(JSPEED, JPREFERENCE, [])

+!handle/vote/notification(CANDIDATES, CONTEXT) <-
    generic/print("Agent", MyName, " got notification to vote on a join: ", CANDIDATES);
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Tolerance, Speed, 1.1, CONTEXT);
    generic/print(VVECTOR);
    !sendvote(VVECTOR).

+!sendvote(VVECTOR)
    : >>isChair(_) <-
        generic/print("Agent", MyName, "Im the chair so no need to pass through omnet");
        !handle/submit/vote(VVECTOR)
    : ~>>isChair(_) <- 
        generic/print("Agent", MyName, "Sending the vote down omnet");
       transmit/other/vote/list(VVECTOR). 

+!handle/submit/vote(VOTE) <-
    generic/print("Got vote", VOTE);
    vote/store(VOTE).

+!handle/tie(CANDIDATES, CONTEXT) <-
    generic/print("There has been a tie. Submitting my vote to tie brake");
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Tolerance, Speed, 1.1, CONTEXT);
    vote/store(VVECTOR).

+!handlesubmitvote(VOTER, VOTE)
    : MyName == VOTER <-
        generic/print("Got my own vote");
        vote/store/dep(VOTER, VOTE)
    : MyName != VOTER <-
        generic/print("Got vote from", VOTER);
        vote/store/dep(VOTER,VOTE).

+!ischair(PID) <-
    +isChair(PID);
    generic/print("Agent ", MyName, " is chair of platoon ", PID).

+!submitvote(VOTER, VOTE) <-
    generic/print("GOT VOTE");
    !handlesubmitvote(VOTER, VOTE).

//Belief triggered plans for all voting agents
+!foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !attemptjoindep(PID, LID).

+!attemptjoindep(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    generic/print("Agent ", MyName, " sending a request to join");
    >>preferedspeed(SPEED);
    >>tolerance(TOLERANCE);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, TOLERANCE).

+!notify/joiner/win(JID) <-
    generic/print("APROVED!");
    vote/send/results(JID, 1).

+!notify/joiner/lose(JID) <-
    generic/print("REJECTED!");
    vote/send/results(JID, 0).

+!maneuver/complete(L) <-
    generic/print("Agent ", MyName, " maneuver complete starting a vote to set speed");
    utility/storemember(L);
    open/vote("speed", [0]).
    //vote/open/ballot/speed().

+!handle/results(VALUE) <-
    >>preferedspeed(Speed); >>tolerance(Tolerance);
    Util = utility/generate/utility(1.1, VALUE, Tolerance, Speed);
    !decide/stay(Util).

+!addmember(L) <-
    utility/storemember(L).
        

+!ballotopen() <-
    generic/print("Agent ", MyName, " got notification of a new ballot starting").


+!inplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).


+!handlerejection(PID) <-
    generic/print("Agent ", MyName, " was rejected from platoon ", PID, "trying the next one");
    NPID = utility/next/platoon();
    generic/print("Next is", NPID);
    !startjoin(NPID).

+!startjoin(PID) 
    : PID >= 0 <-
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
    !startjoin(PID).

+!decide/stay(UTIL)
    <- >>minimumUtility(MUTIL); UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <- >>minimumUtility(MUTIL); UTIL <= MUTIL; generic/print("Agent", MyName, "leaves").