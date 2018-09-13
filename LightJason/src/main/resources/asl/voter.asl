//Defines a standard voter agent

//Belief base definition
//inplatoon(_id).
//lookforplatoon.
//targetplatoonkoin(_id).
//travelroute([_l1, .., _ln]).
//ischair.
minimumUtility(0.8).

//Initial Goal
!main.

//Agent Belief Set up
+!main <-
    Tolerance = utility/generatetolerance();
    SpeedPreference = utility/generatespeedpreference();
    +tolerance(Tolerance);
    +preferedspeed(SpeedPreference);
    generic/print("Agent ", MyName, " of type ", MyType ," started, has preference for speed: ", SpeedPreference ," and tolerance of: ", Tolerance).

+ballotopen() <-
    generic/print("Agent ", MyName, " got notification of a new ballot starting").

+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon").
    //transmit/self/searchForPlatoon(). //Set controller layer to send to agent all platoons that are open for joining

+foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !attemptjoin(PID, LID).

+requestjoin(JID, JSPEED, JPREFERENCE) <-
    !handlejoinrequest(JID, JSPEED, JPREFERENCE).

+!attemptjoin(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    generic/print("Agent ", MyName, " sending a request to join");
    >>preferedspeed(SPEED);
    >>tolerance(TOLERANCE);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED, TOLERANCE).

+!handlejoinrequest(JID, JSPEED, JPREFERENCE) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID, "who preferes speed:", JSPEED, " with a tolerance of ", JPREFERENCE).

+inplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).

+ischair(PID) <-
    generic/print("Agent ", MyName, " is chair of platoon ", PID).