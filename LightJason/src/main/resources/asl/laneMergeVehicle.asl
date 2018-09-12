
!main.

+!main <-
  generic/print("Agent ", MyName, " of type ", MyType ," started");
  !wait.

+!wait <-
  !wait.

+merge(_) <-
  generic/print(MyName, ": In an arterial, sending a message to 3");
  transmit/other/slow(3).
