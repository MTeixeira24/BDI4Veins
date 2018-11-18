//beliefs

//connections between nodes in the route and their state
//0 == closed, 1 == opened
link('s', 'n1', 1).
link('n1', 'n2', 1).
link('n1', 'n3', 1).
link('n2', 'n5', 1).
link('n3', 'n6', 1).
link('n3', 'n4', 1).
link('n5', 'd', 1).
link('n6', 'd', 1).
//potential routes with cost
route('s','d',20,['n1','n3','n6','d']).
route('s','d',25,['n1','n2','n5','d']).
route('s','d',30,['n1','n3','n4','n5','d']).
//what is our current route
currentRoute(['n1','n3','n6','d']).
//Initial goal
!main.

//Rule definition

findroute(Target, Route) :-
    Route = collection/list/create(Target);
    $setnodes(Target, Route)
.

setnodes(Origin, Route) 
    :-
    generic/print("Origin: ",Origin);
    >>(link(N1,N2,1), N1 == Origin);
    collection/list/add(N2, Route);
    $setnodes(N2, Route)
    :-
    ~>>(link(N1,N2,1), Origin == N1)
.


//Plan definition

//Entry point plan, send the first destination to the controller
+!main : >>currentRoute(L) <-
    [N|NL] = L; //Get the first node to reach
    send/target(N); //Send this node to the controller
    -currentRoute(L);
    +currentRoute(NL);
    !!test/target/reached().
    //!!test/find/alternate/link('n1','n3').
    //!!test/find/new/route('n2').

+!test/find/new/route(N) : true <-
    >>route(_,_,_,L);
    $findroute(N,R);
    generic/print("NEW ROUTE: ",R).

+!test/find/alternate/link(N1, N2) : true <-
    -link(N1, N2, _);
    +link(N1, N2, 0);
    >>(link(T1, T2, 1), T1 == N1); //Get an alternative link
    generic/print("Found alternative", T2).


+!test/target/reached() : true <- 
    generic/print("Testing the setting of next target");
    !!target/reached('n1');
    !!target/reached('n3');
    !!target/reached('n6');
    !!target/reached('d').
    

//Controller says that it has reached the target node
+!target/reached(N) 
    : bool/notequal(N, 'd') <- 
    >>currentRoute(L);
    S = collection/size(L); 
    generic/print("I have reached node", N, "current route size", S); 
    !!set/next/target(S, L)
    : bool/equal(N, 'd') <-
    generic/print("Target reached")  
.

//Tell the controller what the next destination is
+!set/next/target(S, L)
    : S > 0 <- 
    [N|NL] = L; 
    -currentRoute(L);
    +currentRoute(NL);
    generic/print("Remaining route:", NL);
    send/target(N)
    : S == 0 <- 
    generic/print("No more future nodes in path").

//A link that we wanted to cross is closed
+!link/closed(N1, N2) <-
    -link(N1, N2, _);
    +link(N1, N2, 0);
    >>(link(T1, T2, 1), T1 == N1); //Get an alternative link
    $findroute(T2,R);
    -currentRoute(_);
    +currentRoute(R);
    send/target(T1).
