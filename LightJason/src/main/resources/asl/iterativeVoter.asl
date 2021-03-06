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
    +factor(Factor);
    return(0).

+!set/initial/beliefs(PID, LID, PSPEED, CHAIR, MEMBERS) <-
    !!inplatoon(PID, LID);
    !!set/speed(PSPEED);
    utility/storememberlist(MEMBERS);
    !!ischair(CHAIR);
    return(0).

+!set/prefered/path(LNODES) <-
    +preferedpath(LNODES);
    generic/print(MyName, "preferred path is:", LNODES);
    return(0).

+!set/prefered/speed(SPEED) <-
    +preferedspeed(SPEED);
    >>factor(Factor);
    return(0).

+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon");
    return(0).

+!set/speed(SPEED) <-
    utility/set/platoon/speed(SPEED);
    +platoonspeed(SPEED);
    return(0).

+!requestjoin(JID, JSPEED, JPREFERENCE) <-
    !!handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE);
    L1 = collection/list/create(JSPEED, JPREFERENCE, JID);
    open/vote("join", L1).

+!handle/vote/notification(CANDIDATES, CONTEXT) <-
    generic/print("Agent", MyName, " got notification to vote on: ", CANDIDATES);
    >>factor(Factor); >>preferedspeed(Speed); >>currentspeed(CurrentSpeed); >>preferedpath(LNODES);
    VVECTOR = utility/generate/vote/vector(CANDIDATES, Factor, Speed, CurrentSpeed, CONTEXT, LNODES);
    generic/print(VVECTOR);
    !!sendvote(VVECTOR).

+!sendvote(VVECTOR)
    : >>isChair(_) <-
        ~>>proxyChair(_);
        generic/print("Agent", MyName, "Im the chair so no need to pass through omnet");
        !!handle/submit/vote(VVECTOR, MyName)
    : >> isChair(_) <-
        >>proxyChair(C);
        generic/print("Agent", MyName, "is a proxy chair sending the vote ");
       transmit/other/vote/list(VVECTOR);
       -proxyChair(C)
    : ~>>isChair(_) <- 
       generic/print("Agent", MyName, "Sending the vote down omnet");
       transmit/other/vote/list(VVECTOR)
. 

+!handle/submit/vote(VOTE, ID) <-
    generic/print(MyName,": Got vote", VOTE, "from", ID);
    //CANDIDATESIZE = utility/get/candidates/size();
    vote/store/iterative(VOTE).

+!call/winner/determination(ExtraVotes) <-
    finalize/regroup(ExtraVotes)
.

+!select/vote/method(VOTE, CANDIDATESIZE)
    : CANDIDATESIZE > 2 <- vote/store/iterative(VOTE)
    : CANDIDATESIZE <= 2 <- vote/store(VOTE).

+!handle/tie(CANDIDATES, CONTEXT, TIES) <-
    generic/print("There has been a tie. Submitting my vote to tie brake");
    >>factor(Factor); >>preferedspeed(Speed); >>currentspeed(CurrentSpeed);
    VVECTOR = utility/break/tie/vote(CANDIDATES, Factor, Speed, CurrentSpeed, CONTEXT, TIES);
    vote/store(VVECTOR).

+!ischair(PID)
    : PID < 0 <- >>isChair(C); -isChair(C)
    : PID >= 0 <-   +isChair(PID); utility/get/nodes();
                    generic/print("Agent ", MyName, " is chair of platoon ", PID).

+!foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !!attemptjoindep(PID, LID).

+!attemptjoindep(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    >>preferedspeed(SPEED);
    >>factor(Factor);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, Factor).

+!maneuver/complete(L) <-
    utility/storemember(L);
    open/vote("speed", [0]). //"node, speed"
    
+!start/vote/node(L, ARGS) <-
        generic/print(MyName,": Opening node vote......");
        utility/storemember(L);
        L1 = collection/list/create(ARGS);
        open/vote("node", L1).

+!start/vote/regroup(Candidates, Context) 
    : >>(isChair(PID), PID == 0) <-
        List =collection/list/flatconcat(Candidates, Context);
        open/vote("regroup", List)
    : >>(isChair(PID), PID != 0) <-
        List =collection/list/flatconcat(Candidates, Context);
        generic/print("AGENT", MyName, "IS A PROXY CHAIR");
        +proxyChair(PID);
        open/vote("regroup", List)
.


+!start/vote/speed(ARGS) <-
    generic/print(MyName,": Opening speed vote......");
    L1 = collection/list/create(ARGS);
    open/vote("speed", L1).

+!update/speed(SPEED) 
    : >>currentspeed(S) <-  -currentspeed(S); +currentspeed(SPEED)
    : ~>>currentspeed(_) <- +currentspeed(SPEED).

+!decide/stay(UTIL, MUTIL)
    <-  UTIL > MUTIL; generic/print("Agent", MyName, "stays")
    <-  UTIL <= MUTIL; generic/print("Agent", MyName, "leaves").

+!handle/results(VALUE) <-
    >>preferedspeed(PreferredSpeed); >>factor(Factor); >>currentspeed(CurrentSpeed);
    Util = utility/generate/utility(VALUE, Factor, PreferredSpeed, CurrentSpeed);
    >>minimumUtility(MUTIL);
    generic/print(VALUE, Factor, PreferredSpeed, Util, MUTIL);
    utility/save(Factor, PreferredSpeed, Util, VALUE); //Used to gather 
    !!decide/stay(Util, MUTIL).

+!handle/results/committee(VALUE) <-
    >>preferedpath(LNODES);
    Util = utility/generate/utility/committee(VALUE, LNODES);
    generic/print(VALUE, LNODES, Util);
    !!decide/stay(Util, 4.0).

+!save/utility(UTIL) <-
    utility/save(0,0,UTIL,0).


+!addmember(L) <-
    generic/print("Adding member...", L);
    utility/storemember(L).


+!inplatoon(PID, LID) <-
    +inplatoon(PID, LID);
    save/platoon(PID);
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).


+!handlerejection(PID) <-
    NPID = utility/next/platoon();
    !!startjoin(NPID).

+!startjoin(PID) 
    : PID >= 0 <-
        LID = utility/get/leader(PID);
        generic/print("Agent ", MyName, "next platoon is", PID, "whos leader is:", LID);
        !!attemptjoindep(PID, LID)
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
    !!startjoin(PID).

+!pushplatoon/start(PID, PSPEED, LID) <-
    generic/print("Enter PushPlatoon", PID, PSPEED, LID);
    >>factor(Factor);
    >>preferedspeed(Speed);
    >>currentspeed(CurrentSpeed);
    utility/store/platoon/start(PID, PSPEED, LID, Factor, Speed, CurrentSpeed);
    generic/print("Exit PushPlatoon").