let num = fst;;
let den = snd;;

let rec gcd i j =
  if i <> 0 then
    if j > i then
      gcd i (j - i)
    else
      gcd (i - j) j
  else j;;

let rat n d =
  let g = gcd n d in (n / g, d / g);;

let ratAdd a b =
  rat ((num a * den b) + (den a * num b)) (den a * den b);;

let ratMul a b =
  rat (num a * num b) (den a * den b);;

let ratDiv a b =
  rat (num a * den b) (den a * num b);;

let ratGt a b =
  (num a * den b) > (den a * num b);;

let euler () =
  let rec eulering c s t =
    if ratGt (rat 1 100000) t
      then s
    else let c' = ratAdd c (rat 1 1) in 
    let t' = ratDiv t c' in eulering c' (ratAdd s t) t'
  in eulering (rat 1 1) (rat 1 1) (rat 1 1);;

(*
   1. turn loop into recursive helper function
   2. turn variable into parameters
   3. assignments turn into let's or arguments
   4. initialized variables turn into arguments

   s = sum of fractions
   t = a fraction in sum
   c = counter for factorial

   e = 1/0! + 1/1! + 1/2! + 1/3! + ... = s

   let eulering s c t =
    if ratGt t (rat 1 1000000) 
      then s
    else let s = eulering (ratAdd s t) (ratDiv t c) (ratAdd c 1) in eulering


*)
(* how euler looks in imperative code:
   double euler() { 
    double c = 1; 
    double s = 0; 
    double t = 1; 

    while (t > ε) { 
      s = s + t; 
      t = t / c; 
      c = c + 1; 
    } 
    return s; 
  }
*)



(*
  CSci 2041 Tests for Lab Assignment 2

    James Moen
    04 Feb 25

  These tests are worth 40 points total.
*)

(* RAT PRINT. Print a pair (N, D) as the fraction N / D. You don't have to know
   how this works. *)

   let ratPrint (n, d) =
    Printf.printf "%i / %i\n" n d ;;
  
  (* BOOL PRINT. Print a BOOL B. You don't have to know how this works either. *)
  
  let boolPrint b =
    Printf.printf "%b\n" b ;;
  
  (* Test the rational arithmetic functions. *)
  
  ratPrint (rat 1 2) ;;                                       (* 2 pts: 1 / 2 *)
  
  ratPrint (rat 10 20) ;;                                     (* 2 pts: 1 / 2 *)
  
  ratPrint (ratAdd (rat 1 2) (rat 1 2)) ;;                    (* 2 pts: 1 / 1 *)
  
  ratPrint (ratAdd (rat 1 3) (rat 1 2)) ;;                    (* 2 pts: 5 / 6 *)
  
  ratPrint (ratMul (rat 1 2) (rat 10 1)) ;;                   (* 2 pts: 5 / 1 *)
  
  ratPrint (ratMul (rat 2 3) (rat 4 5)) ;;                    (* 2 pts: 8 / 15 *)
  
  ratPrint (ratDiv (rat 1 2) (rat 10 2)) ;;                   (* 2 pts: 1 / 10 *)
  
  ratPrint (ratDiv (rat 1 2) (rat 1 3)) ;;                    (* 2 pts: 3 / 2 *)
  
  boolPrint (ratGt (rat 1 2) (rat 1 3)) ;;                    (* 2 pts: true *)
  
  boolPrint (ratGt (rat 1 3) (rat 1 2)) ;;                    (* 2 pts: false *)
  
  (* The big finish. Compute E. *)
  
  ratPrint (euler ()) ;;                             (* 20 pts: 109601 / 40320 *)

 (* RESULTS:
utop # #use "lab2.ml";;
val num : 'a * 'b -> 'a = <fun>
val den : 'a * 'b -> 'b = <fun>
val gcd : int -> int -> int = <fun>
val rat : int -> int -> int * int = <fun>
val ratAdd : int * int -> int * int -> int * int = <fun>
val ratMul : int * int -> int * int -> int * int = <fun>
val ratDiv : int * int -> int * int -> int * int = <fun>
val ratGt : int * int -> int * int -> bool = <fun>
val euler : unit -> int * int = <fun>
val ratPrint : int * int -> unit = <fun>
val boolPrint : bool -> unit = <fun>
1 / 2
- : unit = ()
1 / 2
- : unit = ()
1 / 1
- : unit = ()
5 / 6
- : unit = ()
5 / 1
- : unit = ()
8 / 15
- : unit = ()
1 / 10
- : unit = ()
3 / 2
- : unit = ()
true
- : unit = ()
false
- : unit = ()
109601 / 40320
- : unit = ()
 *)