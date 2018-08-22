speed(0).

!main.

+!main <-
  generic/print("Agent ", MyName, " started");
  !wait.

+!wait <-
  !wait.

+speed(X) <-
  X < 1.0;
  generic/print(MyName, ": Received new speed value of", X).

+speed(X) <-
  X >= 1.0;
  >>slowing(_);
  generic/print(MyName, ": Received new speed value of", X).

+speed(X) <-
  X >= 1.0;
  ~>>slowing(_);
  +slowing(1);
  generic/print(MyName, ": Sending a request to omnet");
  transmit/self/slow().