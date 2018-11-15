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
currentRoute([n1,n3,n6]).
//currentRoute(n1,d,[n3,n6])
//Initial goal
!main.

//Rule definition

findnode(TargetNode, [L|ListRoutes], NewRoute)
    :- L != TargetNode; $findnode(TargetNode, ListRoutes, NewRoute)
    :- L == TargetNode; NewRoute = ListRoutes
.

//Plan definition

//Entry point plan, send the first destination to the controller
+!main : >>currentRoute(L) <-
    [N|_] = L; //Get the first node to reach
    send/target(N). //Send this node to the controller

//Controller says that it has reached the target node
+!target/reached(N) : (>>currentRoute(L); S = collection/size(L)) 
    <- S > 0; -currentRoute(L); collection/list/remove(L, 0); +currentRoute(L); [NN|_] = L; send/target(NN).
    <- S == 0. //We have reached the target

//A link that we wanted to cross is closed
+!link/closed(N1, N2) <-
    -link(N1, N2, _);
    +link(N1, N2, 0);
    >>link(N1, T, 1); //Get an alternative link
    !find/alternative(T).

+!find/alternative(T) <-
    >>route(_,_,_,L);
    $findnode(T,L,R);
    -currentRoute(_);
    +currentRoute(R);
    send/target(T).
