(*
TESTS6. Tests for CSci 2041 Lab 6.
James Moen
04 Mar 25
*)
(* TIME. Print TITLE and the time needed to compute FUNC (). Return the value
of FUNC (). FUNC will typically be an anonymous function. Students don't
have to write this. *)
let time title func =
let t0 = Sys.time ()
in let result = func ()
in let t1 = Sys.time ()
in Printf.printf "%s %f seconds\n" title (t1 -. t0) ;
result ;;

(*MY CODE GOES HERE!!!*)
let rec c n k =
  match (n, k) with
  | (_, 0) -> 1
  | (0, _) -> 0
  | (_, _) ->
      (c (n - 1) k) + (c (n - 1) (k - 1));;

let memyC n k =
  let t = Hashtbl.create ~random: false 1000 in 
  let rec memming n k =
    match (n, k) with
    | (_, 0) -> 1
    | (0, _) -> 0
    | (_, _) ->
        match Hashtbl.find_opt t (n, k) with
        | Some v -> v
        | None ->
            let v = (memming (n-1) k) + (memming (n-1) (k-1)) in
            Hashtbl.add t (n, k) v; v
        in memming n k;;  


(* Tests. The first few tests make sure C works for base cases and one or two
simple cases. All these tests should have VERY small execution times. Times
for your code MAY BE DIFFERENT. *)
time "c test1" (fun () -> c 0 0) ;;
(* c test1 0.000001 seconds
- : int = 1
1 point. *)
time "c test2" (fun () -> c 0 1) ;;
(* c test2 0.000001 seconds
- : int = 0
1 point. *)
time "c test3" (fun () -> c 1 0) ;;
(* c test3 0.000001 seconds
- : int = 1
1 point. *)
time "c test4" (fun () -> c 1 1) ;;
(* c test4 0.000001 seconds
- : int = 1
1 point. *)
time "c test5" (fun () -> c 8 4) ;;
(* c test5 0.000008 seconds
- : int = 70
4 points. *)
time "c test6" (fun () -> c 5 2) ;;
(* c test6 0.000002 seconds
- : int = 10
4 points. *)
(* This test may have a run time measured in tens of seconds. Be patient! You
get extra points for waiting. *)
time "c test7" (fun () -> c 40 10) ;;
(* c test7 40.424319 seconds
- : int = 847660528
8 points. *)
(* These tests make sure MEMY C works for base cases and simple cases too. They
should also have very small execution times, but maybe a little larger than
those of C. It's because MEMY C does work to maintain its hash table. Times
for your code MAY BE DIFFERENT. *)
time "memyC test1" (fun () -> memyC 0 0) ;;
(* memyC test1 0.000003 seconds
- : int = 1
2 points. *)
time "memyC test2" (fun () -> memyC 0 1) ;;
(* memyC test2 0.000002 seconds
- : int = 0
2 points. *)
time "memyC test3" (fun () -> memyC 1 0) ;;
(* memyC test3 0.000002 seconds
- : int = 1
2 points. *)
time "memyC test4" (fun () -> memyC 1 1) ;;
(* memyC test4 0.000003 seconds
- : int = 1
2 points. *)
time "memyC test5" (fun () -> memyC 8 4) ;;
(* memyC test5 0.000012 seconds
- : int = 70
6 points. *)
time "memyC test6" (fun () -> memyC 5 2) ;;
(* memyC test6 0.000002 seconds
- : int = 10
6 points. *)
(* This test should have a very short run time: MUCH less than one second! *)
time "memyC test7" (fun () -> memyC 40 10) ;;
(* memyC test7 0.000095 seconds
- : int = 847660528
10 points. *)
