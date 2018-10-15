//Defines an iterative voter agent

minimumUtility(0.8).

//Initial Goal
!main.

//Definition of rules
generateutility(JSPEED, JPREFERENCE, PSPEED, PredictedUtility)
    :-  >>tolerance(Tolerance); >>preferedspeed(SpeedPreference);
        PredictedUtility = utility/predictplatoonspeed(JSPEED, JPREFERENCE, SpeedPreference, Tolerance, PSPEED).

//Plan definition
+!main <-
    Tolerance = utility/generatetolerance();
    +tolerance(Tolerance).

+!set/initial/beliefs(PID, LID, PSPEED, CHAIR, MEMBERS, SPEEDS) <-
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

+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon").

+!set/speed(SPEED) <-
    utility/set/platoon/speed(SPEED);
    +platoonspeed(SPEED).

+!requestjoin(JID, JSPEED, JPREFERENCE) <-
    !handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE);
    L1 = collection/list/create(JSPEED, JPREFERENCE, JID);
    open/vote("join", L1).

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
    //CANDIDATESIZE = utility/get/candidates/size();
    vote/store/iterative(VOTE).

+!select/vote/method(VOTE, CANDIDATESIZE)
    : CANDIDATESIZE > 2 <- vote/store/iterative(VOTE)
    : CANDIDATESIZE <= 2 <- vote/store(VOTE).

+!handle/tie(CANDIDATES, CONTEXT, TIES) <-
    generic/print("There has been a tie. Submitting my vote to tie brake");
    >>tolerance(Tolerance); >>preferedspeed(Speed);
    VVECTOR = utility/break/tie/vote(CANDIDATES, Tolerance, Speed, 1.1, CONTEXT, TIES);
    vote/store(VVECTOR).

+!ischair(PID)
    : PID < 0 <- true
    : PID >= 0 <-   +isChair(PID); 
                    generic/print("Agent ", MyName, " is chair of platoon ", PID).

+!foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !attemptjoindep(PID, LID).

+!attemptjoindep(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    >>preferedspeed(SPEED);
    >>tolerance(TOLERANCE);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, TOLERANCE).

+!maneuver/complete(L) <-
    utility/storemember(L);
    open/vote("speed", [0]).
    

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
    >>tolerance(Tolerance);
    >>preferedspeed(Speed);
    utility/store/platoon(PID, PSPEED, LID, Tolerance, Speed).

+!startrequests() <- 
    PID = utility/next/platoon();
    !startjoin(PID).

+!decide/stay(UTIL)
    <- >>minimumUtility(MUTIL); UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <- >>minimumUtility(MUTIL); UTIL <= MUTIL; generic/print("Agent", MyName, "leaves").