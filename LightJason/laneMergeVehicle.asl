speed(0).
merging(true).

!main.

+!main <-
  generic/print("Agent ", MyName, " of type ", MyType ," started asdasd");
  I = 39;
  transmit/other/slow(I);
  !wait.

+!main <-
  generic/print("Agent ", MyName, " of type ", MyType ," started-----");
  !wait.

+!wait <-
  !wait.
