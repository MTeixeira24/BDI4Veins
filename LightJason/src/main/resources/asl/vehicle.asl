

!main.

+!main <-
  generic/print("I am vehicle", MyName);
  !wait.

+!wait <-
  WaitingTime > 0;
  generic/print(MyName, ": Waiting...");
  vehicle/wait();
  !wait.
+!wait <-
  WaitingTime <= 0;
  lane/enter();
  !!drive.

+!drive <-
  lane/move();
  !drive.

+!lane/progress(in(X)) <-
  generic/print(MyName,": Advanced to block -> ", X).

+!lane/end() <-
  generic/print(MyName,": Reached end of lane");
  -drive.
