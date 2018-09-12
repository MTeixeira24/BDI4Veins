//Defines a standard voter agent

//Belief base definition
//inplatoon(_id).
//lookforplatoon.
//targetplatoonkoin(_id).
//travelroute([_l1, .., _ln]).
//ischair.
minimumUtility(200).
preferedspeed(80).

//Initial Goal
!main.

//Agent Belief Set up
+!main <-
    FuelConsumptionPreference = utility/generatefuelpreference();
    TravelTimePreference = utility/generatetraveltimepreference(FuelConsumptionPreference);
    +fuelpreference(FuelConsumptionPreference);
    +traveltimepreference(TravelTimePreference);
    generic/print("Agent ", MyName, " of type ", MyType ," started, has preference for fuel: ", FuelConsumptionPreference ," and TravelTimePreference: ", TravelTimePreference).

+ballotopen() <-
    generic/print("Agent ", MyName, " got notification of a new ballot starting").

+!lookforplatoon() <-
    generic/print("Agent ", MyName, " have intention of searching for platoon").
    //transmit/self/searchForPlatoon(). //Set controller layer to send to agent all platoons that are open for joining

+foundplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " found a potential platoon: ", PID);
    !attemptjoin(PID, LID).

+requestjoin(JID, JSPEED) <-
    !handlejoinrequest(JID, JSPEED).

+!attemptjoin(PID, LID) <-
    +targetplatoonjoin(PID);
    +leaderid(LID);
    generic/print("Agent ", MyName, " sending a request to join");
    >>preferedspeed(SPEED);
    transmit/other/sendjoinplatoonrequest(PID, LID, SPEED).

+!handlejoinrequest(JID, JSPEED) <-
    generic/print("Agent ", MyName, " received a request to join the platoon from ", JID).

+inplatoon(PID, LID) <-
    generic/print("Agent ", MyName, " is in platoon ", PID, " whoose leader is: ", LID).

+ischair(PID) <-
    generic/print("Agent ", MyName, " is chair of platoon ", PID).