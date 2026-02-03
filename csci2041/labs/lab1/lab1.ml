open List

let rec howMany e l =
  if l = [] then 0
  else if hd l = e then 1 + howMany e (tl l)
  else howMany e (tl l);;

let rec delete e l =
  if l = [] then []
  else if hd l = e then delete e (tl l)
  else (hd l)::(delete e (tl l));;

let mean l =
  let rec sum m =
    if m = [] then 0.0
    else (hd m) +. sum (tl m)
  in

  let rec length m =
    if m = [] then 0.0
    else 1.0 +. length (tl m)
  in

  sum l /. length l;;



  (*
  Tests for CSci 2041 Computer Laboratory 1
  James Moen

  This is worth 30 points.
*)

open Printf ;;

(* PRINT THINGS. Print a list L. You don't have to know how this works! *)

let printThings f l =
  let rec printingThings l =
    match l with
      [] -> () |
      h :: t -> printf " ; " ; printf f h ; printingThings t
  in printf "[" ;
     (match l with
        [] -> () |
        h :: t -> printf f h ; printingThings t) ;
     printf "]\n" ;;

(* Tests for HOW MANY. *)

printf "%i\n" (howMany 1 []) ;;                      (* 2 pt: 0 *)
printf "%i\n" (howMany 1 [1]) ;;                     (* 2 pt: 1 *)
printf "%i\n" (howMany 2 [1; 2; 3]) ;;               (* 2 pt: 1 *)
printf "%i\n" (howMany 5 [2; 4; 6]) ;;               (* 2 pt: 0 *)
printf "%i\n" (howMany "c" ["c"; "b"; "c"; "d"]) ;;  (* 2 pt: 2 *)
printf "%i\n" (howMany "x" ["a"; "b"; "c"; "d"]) ;;  (* 2 pt: 0 *)

(* Tests for DELETE. *)

printThings "%i" (delete 1 []) ;;                     (* 2 pt: [] *)
printThings "%i" (delete 1 [1]) ;;                    (* 2 pt: [] *)
printThings "%i" (delete 1 [1; 2; 3]) ;;              (* 2 pt: [2 ; 3] *)
printThings "%i" (delete 4 [1; 2; 3]) ;;              (* 2 pt: [1 ; 2 ; 3] *)
printThings "%i" (delete 1 [1; 2; 1; 3; 1; 4]) ;;     (* 2 pt: [2 ; 3 ; 4] *)
printThings "%s" (delete "a" ["x"; "a"; "y"; "a"]) ;; (* 2 pt: [x ; y] *)

(* Tests for MEAN. *)

printf "%f\n" (mean [1.0]) ;;                         (* 2 pts: 1.000000 *)
printf "%f\n" (mean [1.0; 2.0]) ;;                    (* 2 pts: 1.500000 *)
printf "%f\n" (mean [1.0; 0.0; -1.0; 1.0]) ;;         (* 2 pts: 0.250000 *)


(*utop # #use "lab1.ml";;
val howMany : 'a -> 'a list -> int = <fun>
val delete : 'a -> 'a list -> 'a list = <fun>
val mean : float list -> float = <fun>
val printThings : ('a -> 'b, out_channel, unit) format -> 'a list -> unit =
  <fun>
0
- : unit = ()
1
- : unit = ()
1
- : unit = ()
0
- : unit = ()
2
- : unit = ()
0
- : unit = ()
[]
- : unit = ()
[]
- : unit = ()
[2 ; 3]
- : unit = ()
[1 ; 2 ; 3]
- : unit = ()
[2 ; 3 ; 4]
- : unit = ()
[x ; y]
- : unit = ()
1.000000
- : unit = ()
1.500000
- : unit = ()
0.250000
- : unit = ()
*)


