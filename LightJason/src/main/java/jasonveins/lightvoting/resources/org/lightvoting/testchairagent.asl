iteration(0).

!main.

+!main <-
    generic/print("....Chair....");
    !nextcycle
  .

+!nextcycle <-
    perceive/group();
    check/conditions();
    !nextcycle
    .

+!start/criterion/fulfilled() <-
    generic/print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!! I'm now starting the election.");

    start/election()
    .

+!vote/received(Traveller, Vote) <-
   store/vote(Traveller, Vote)
   .

+!diss/received(Traveller, Diss, Iteration) <-
   generic/print("Receiving dissatisfaction" );
   store/diss(Traveller, Diss, Iteration)
   .


+!all/votes/received() <-
   generic/print(" All voters submitted their votes" );
   compute/result()
   .

+!all/dissValues/received(Iteration) <-
   generic/print("Received all diss values");
   remove/voter()
   .
