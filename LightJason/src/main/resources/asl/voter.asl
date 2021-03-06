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
    //SpeedPreference = utility/generatespeedpreference();
    +tolerance(Tolerance).
    //+preferedspeed(SpeedPreference);
    //generic/print("Agent ", MyName, " of type ", MyType ," started, has preference for speed: ", SpeedPreference ," and tolerance of: ", Tolerance).

+!set/initial/beliefs(PID, LID, PSPEED, CHAIR, MEMBERS) <-
    !inplatoon(PID, LID);
    !set/speed(PSPEED);
    utility/storememberlist(MEMBERS);
    !ischair(CHAIR).

+!set/prefered/path(LNODES) <-
    generic/print("AUX");
    generic/print(LNODES);
    +preferedpath(LNODES);
    generic/print(MyName, "preferred path is:", LNODES).

+!set/prefered/speed(SPEED) <-
    +preferedspeed(SPEED);
    >>tolerance(Tolerance).
    //generic/print("Agent ", MyName, " of type ", MyType ," started, has preference for speed: ", SPEED ," and tolerance of: ", Tolerance).

//Goal triggered plans for all voting agents
+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon").
    //transmit/self/searchForPlatoon(). //Set controller layer to send to agent all platoons that are open for joining

+!set/speed(SPEED) <-
    //generic/print("Agent ",MyName, " the speed of my platoon is", SPEED);
    utility/set/platoon/speed(SPEED);
    +platoonspeed(SPEED).

+!requestjoin(JID, JSPEED, JPREFERENCE) <-
    !handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE);
    L1 = collection/list/create(JSPEED, JPREFERENCE, JID);
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
        !handle/submit/vote(VVECTOR, MyName)
    : ~>>isChair(_) <- 
       generic/print("Agent", MyName, "Sending the vote down omnet");
       transmit/other/vote/list(VVECTOR). 

+!handle/submit/vote(VOTE, ID) <-
    generic/print("Got vote", VOTE, "from", ID);
    vote/store(VOTE).

+!handle/tie(CANDIDATES, CONTEXT, TIES) <-
    generic/print("There has been a tie. Submitting my vote to tie brake");
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/break/tie/vote(CANDIDATES, Tolerance, Speed, 1.1, CONTEXT, TIES);
    vote/store(VVECTOR).

+!ischair(PID)
    : PID < 0 <- true
    : PID >= 0 <-   +isChair(PID); 
                    generic/print("Agent ", MyName, " is chair of platoon ", PID).

//Belief triggered plans for all voting agents
+!foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !attemptjoindep(PID, LID).

+!attemptjoindep(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    //generic/print("Agent ", MyName, " sending a request to join");
    >>preferedspeed(SPEED);
    >>tolerance(TOLERANCE);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, TOLERANCE).

+!maneuver/complete(L) <-
    //generic/print("Agent ", MyName, " maneuver complete starting a vote to set speed");
    utility/storemember(L);
    open/vote("speed", [0]).
    //vote/open/ballot/speed().

+!handle/results(VALUE) <-
    >>preferedspeed(Speed); >>tolerance(Tolerance);
    Util = utility/generate/utility(1.1, VALUE, Tolerance, Speed);
    generic/print(VALUE, Tolerance, Speed, Util);
    utility/save(1.1, Tolerance, Speed, Util); //Used to gather data
    !decide/stay(Util).

+!addmember(L) <-
    utility/storemember(L).


+!inplatoon(PID, LID) <-
    +inplatoon(PID, LID);
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).


+!handlerejection(PID) <-
    NPID = utility/next/platoon();
    !startjoin(NPID).

+!startjoin(PID) 
    : PID >= 0 <-
        LID = utility/get/leader(PID);
        generic/print("Agent ", MyName, "next platoon is", PID, "whos leader is:", LID);
        !attemptjoin(PID, LID)
    : PID < 0 <-
        generic/print("Agent ", MyName, "rejected by all. Aborting negotiations").

+!pushplatoon(PID, PSPEED, LID) <-
    >>factor(Factor);
    >>preferedspeed(Speed);
    >>currentspeed(CurrentSpeed);
    utility/store/platoon(PID, PSPEED, LID, Factor, Speed, CurrentSpeed).

+!startrequests() <- 
    PID = utility/next/platoon();
    !startjoin(PID).

+!decide/stay(UTIL)
    <- >>minimumUtility(MUTIL); UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <- >>minimumUtility(MUTIL); UTIL <= MUTIL; generic/print("Agent", MyName, "leaves").