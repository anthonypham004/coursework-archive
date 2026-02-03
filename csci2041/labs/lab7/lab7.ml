open Lazy;;

type 'l lazyList =
	LazyEmpty |
	LazyNode of 'l Lazy.t * 'l lazyList Lazy.t;;
	
exception LazyListError;;

let lazyCons h t =
	LazyNode(h, t);;

let lazyHead l =
	match l with
	| LazyEmpty -> raise LazyListError
	| LazyNode (head, _) -> head;;
	
let lazyTail l =
	match l with
	| LazyEmpty -> raise LazyListError
	| LazyNode (_, tail) -> force tail;;

let rec lazyTake l n =
  if n <= 0 
    then raise LazyListError
  else match l with
  | LazyEmpty -> raise LazyListError
  | LazyNode (head, tail) ->
    (force head)::(lazyTake (lazyTail l) (n-1));;
        
(*
since lazyHead already raises error w/ empty, case can be omitted(?)
better yet DON'T use lazyHead, match l itself, and then force head for cons
*)



(* LAZY INTS. Return a lazy list of integers, from FIRST to LAST, inclusive.
   Print an annoying message each time a new integer is computed. *)

   let lazyInts start stop =
    let rec lazyInting which =
      if which <= stop
      then lazyCons
             (lazy (Printf.printf "Computed integer %i\n" which ; which))
             (lazy (lazyInting (which + 1)))
      else LazyEmpty
    in lazyInting start ;;
  
  (* LAZY FIBS. Return an infinitely long lazy list of Fibonacci numbers. Pretend
     that arithmetic will not overflow. Print an annoying message each time a new
     Fibonacci number is computed. LAZY FIBS would not terminate if written using
     eager evaluation! *)
  
  let lazyFibs () =
    let rec lazyFibbing left right =
      lazyCons
        (lazy (Printf.printf "Computed Fibonacci %i\n" left ; left))
        (lazy (lazyFibbing right (left + right)))
    in lazyFibbing 0 1 ;;
  
  (* Tests, worth 35 points. *)
  
  let strings =
    (lazyCons
      (lazy "I'm")
      (lazy 
        (lazyCons
          (lazy "so")
          (lazy
            (lazyCons
              (lazy "lazy")
              (lazy LazyEmpty)))))) ;;
  
  (* val strings : string lazyList = LazyNode (lazy "I'm", <lazy>) *)
  
  lazyHead strings ;;
  
  (* 2 pts.
     - : string = "I'm" *)
  
  lazyHead (lazyTail strings) ;;
  
  (* 2 pts.
     - : string = "so" *)
  
  lazyHead (lazyTail (lazyTail strings)) ;;
  
  (* 2 pts.
     - : string = "lazy" *)
  
  try
    lazyHead (lazyTail (lazyTail (lazyTail strings)))
  with
    LazyListError ->
      Printf.printf "Oops.\n" ;
      "" ;;
  
  (* 2 pts.
     Oops.
     - : string = "" *)
  
  lazyTake strings 3 ;;
  
  (* 2 pts. 
     - : string list = ["I'm"; "so"; "lazy"] *)
  
  let oneThruNine = lazyInts 1 9 ;;
  
  (* val oneThruNine : int lazyList = LazyNode (<lazy>, <lazy>) *)
  
  lazyTake oneThruNine 3 ;;
  
  (* 5 pts.
     Computed integer 3
     Computed integer 2
     Computed integer 1
     - : int list = [1; 2; 3] *)
  
  lazyTake oneThruNine 9 ;;
  
  (* 5 pts. 
     Computed integer 9
     Computed integer 8
     Computed integer 7
     Computed integer 6
     Computed integer 5
     Computed integer 4
     - : int list = [1; 2; 3; 4; 5; 6; 7; 8; 9] *)
  
  lazyTake oneThruNine 9 ;;
  
  (* 5 pts.
     - : int list = [1; 2; 3; 4; 5; 6; 7; 8; 9] *)
  
  let allTheFibs = lazyFibs () ;;
  
  (* val allTheFibs : int lazyList = LazyNode (<lazy>, <lazy>) *)
  
  lazyTake allTheFibs 10 ;;
  
  (* 5 pts.
     Computed Fibonacci 34
     Computed Fibonacci 21
     Computed Fibonacci 13
     Computed Fibonacci 8
     Computed Fibonacci 5
     Computed Fibonacci 3
     Computed Fibonacci 2
     Computed Fibonacci 1
     Computed Fibonacci 1
     Computed Fibonacci 0
     - : int list = [0; 1; 1; 2; 3; 5; 8; 13; 21; 34] *)
  
  lazyTake allTheFibs 10 ;;
  
  (* 5 pts.
     - : int list = [0; 1; 1; 2; 3; 5; 8; 13; 21; 34] *)