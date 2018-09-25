lookForGroup.
iteration(0).

// initial-goal
!main.

// initial plan (triggered by the initial goal)

+!main <-

   generic/print(MyName, "Hello World!");
   >>chair(Chair);
   generic/print(MyName, "MyChair:", Chair);
   generic/print(MyName, "Testing Voting Agent");

   perceive/env();
   join/group();
   !nextcycle
   .

+!nextcycle <-
   >>chair(Chair);
   generic/print(MyName, " MyChair:", Chair);
   generic/print(MyName, "Testing Voting Agent");
   !!test
   .

+!test  <-
   generic/print("Testing", MyName, "actions in cycle", "Cycle var does not exist, wont bother implementing lol");
   // send my name to agent 0
   message/send("agent 0", MyName)
   .

+groups(Groups) <-
   generic/print(MyName, "..................Received belief on groups ", Groups )
   .

+group(Chair, Open, Result, MyGroup) <-
   generic/print("..................Received belief on group with open:", Open, " result:", Result, " mygroup:", MyGroup, " ", Chair )
   .

+!group(ID, Chair) <-
   generic/print("------------------ ", MyName, " Group ", ID," opened with Chair ", Chair)
   .

+!submit/vote(Chair) <-
   generic/print(MyName, " I need to submit my vote to chair ", Chair);
   submit/vote(Chair)
   .

// plan for basic voting
+!election/result(Chair, Result) <-
   generic/print(MyName, " heard result ", Result, " from chair ", Chair)
   .

// plan for iterative voting
+!election/result(Chair, Result, Iteration) <-
    generic/print(MyName, " Submit Dissatisfaction");
    submit/dissatisfaction(Chair, Iteration, Result)
    .
