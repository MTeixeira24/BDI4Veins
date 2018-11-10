//beliefs

//connections between nodes in the route and their state
//0 == closed, 1 == opened
link(s, n1, 1).
link(n1, n2, 1).
link(n1, n3, 1).
link(n2, n5, 1).
link(n3, n6, 1).
link(n3, n4, 1).
link(n5, d, 1).
link(n6, d, 1).
//potential routes with cost
route(s,d,20,[n1,n3,n6]).
route(s,d,25,[n1,n2,n5]).
route(s,d,30,[n1,n3,n4,n5]).
//what is our current route
currentRoute(s,d,[n1,n3,n6]).

//Initial goal
!main.

//Rule definition

//=====================//
//==DEFINE RULES HERE==//
//=====================//

//Plan definition

//Entry point plan, send the first destination to the controller
+!main : >>currentRoute(_, _, L) <-
    [N|_] = L; //Get the first node to reach
    send/target(N). //Send this node to the controller

//Controller says that it has reached the target node
+!target/reached(N) : >>currentRoute(C, D, L) 
    <- S = collection/size(L); S > 0; collection/list/remove(L, 0); -currentRoute(C,D,L); +currentRoute(N,D,L); [NN|_] = L; send/target(NN).
    <- S = collection/size(L); S == 0. //We have reached the target

//A link that we wanted to cross is closed
+!link/closed(N1, N2) <-
    -link(N1, N2, _);
    +link(N1, N2, 0).

//handle the fact that a link is closed
+link(N1, N2, S) <- true. 
