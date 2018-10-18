//Defines an iterative voter agent

minimumUtility(0.8).

//Initial Goal
!main.

//Definition of rules
generateutility(JSPEED, JPREFERENCE, PSPEED, PredictedUtility)
    :-  >>factor(Factor); >>preferedspeed(SpeedPreference);
        PredictedUtility = utility/predictplatoonspeed(JSPEED, JPREFERENCE, SpeedPreference, Factor, PSPEED).

//Plan definition
+!main <-
    Factor = utility/get/factor();
    +factor(Factor).

+!set/initial/beliefs(PID, LID, PSPEED, CHAIR, MEMBERS) <-
    !inplatoon(PID, LID);
    !set/speed(PSPEED);
    utility/storememberlist(MEMBERS);
    !ischair(CHAIR).

+!set/prefered/path(LNODES) <-
    +preferedpath(LNODES);
    generic/print(MyName, "preferred path is:", LNODES).

+!set/prefered/speed(SPEED) <-
    +preferedspeed(SPEED);
    >>factor(Factor).

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
    >>factor(Factor); >>preferedspeed(Speed); >>currentspeed(CurrentSpeed);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Factor, Speed, CurrentSpeed, CONTEXT);
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
    >>factor(Factor); >>preferedspeed(Speed); >>currentspeed(CurrentSpeed);
    VVECTOR = utility/break/tie/vote(CANDIDATES, Factor, Speed, CurrentSpeed, CONTEXT, TIES);
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
    >>factor(Factor);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, Factor).

+!maneuver/complete(L) <-
    utility/storemember(L);
    open/vote("speed", [0]).
    
+!@update/speed(SPEED) 
    <- >>currentspeed(S); -currentspeed(S); +currentspeed(SPEED)
    <- ~>>currentspeed(_); +currentspeed(SPEED).

+!handle/results(VALUE) <-
    >>preferedspeed(PreferredSpeed); >>factor(Factor); >>currentspeed(CurrentSpeed);
    Util = utility/generate/utility(VALUE, Factor, PreferredSpeed, CurrentSpeed);
    generic/print(VALUE, Factor, PreferredSpeed, Util);
    utility/save(Factor, PreferredSpeed, CurrentSpeed, Util); //Used to gather data
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
        !attemptjoindep(PID, LID)
    : PID < 0 <-
        generic/print("Agent ", MyName, "rejected by all. Aborting negotiations").

+!pushplatoon(PID, PSPEED, LID) <-
    >>factor(Factor);
    >>preferedspeed(Speed);
    >>currentspeed(CurrentSpeed);
    generic/print("PushPlatoon");
    utility/store/platoon(PID, PSPEED, LID, Factor, Speed, CurrentSpeed).

+!startrequests() <- 
    PID = utility/next/platoon();
    !startjoin(PID).

+!pushplatoon/start(PID, PSPEED, LID) <-
    generic/print("Enter PushPlatoon", PID, PSPEED, LID);
    >>factor(Factor);
    >>preferedspeed(Speed);
    >>currentspeed(CurrentSpeed);
    utility/store/platoon/start(PID, PSPEED, LID, Factor, Speed, CurrentSpeed).
    generic/print("Exit PushPlatoon").

+!decide/stay(UTIL)
    <- >>minimumUtility(MUTIL); UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <- >>minimumUtility(MUTIL); UTIL <= MUTIL; generic/print("Agent", MyName, "leaves").