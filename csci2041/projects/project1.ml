(* POLY. The type of a univariate polynomial with integer coefficients and
   nonnegative integer exponents. EMPTY is a polynomial with no TERMs, which
   may be taken to represent 0. A TERM has a coefficient, an exponent, and a
   pointer to the next TERM, or to EMPTY. *)

type poly =
 Empty |
 Term of int * int * poly ;;

(* PRINT POLY. Print POLY in a way that's allegedly easy to read. You need not
   know how this works. *)

let printPoly poly =

(* PRINTING. Do all the work for PRINT POLY. *)

 let rec printing poly =
  match poly
  with Empty ->
        () |

       Term (coef, expo, terms) ->
        Printf.printf " %c %i x^%i"
         (if coef < 0 then '-' else '+') (abs coef) expo ;
         printing terms

(* This is PRINT POLY's body. *)

 in match poly
    with Empty ->
          Printf.printf "0\n" |

         Term (coef, expo, terms) ->
          Printf.printf "%i x^%i" coef expo ;
          printing terms ;
          Printf.printf "\n" ;;

(*
  YOUR CODE GOES HERE!
*)
exception PolyError

let rec term coef expo poly =
   (* check validity of coef *)
   if coef = 0 then raise PolyError
   (* check validity of expo *)
   else if expo < 0 then raise PolyError
   (* if poly is included, check to see if the next term's exponent is less than the previous one's before constructing *)
   else match poly with
   | Empty -> Term (coef, expo, Empty)
   | Term (coef', expo', poly') ->
      if expo <= expo' then raise PolyError
      else
         (* build the rest of the polynomial by recursively calling term with the details in each poly *)
         Term (coef, expo, term coef' expo' poly');;

let rec polyMap makeCoef makeExpo poly =
   match poly with
   | Empty -> Empty (* Signifies end of polynomial, return with empty poly tail *)
   | Term (coef, expo, poly') ->
      (* Map each poly component to their respective function,
         and repeat the same process for the rest of poly w/ a recursive call *)
      Term (makeCoef coef, makeExpo expo, polyMap makeCoef makeExpo poly');;

let polyMinus poly =
   match poly with
   | Empty -> Empty
   (* After checking for an empty polynomial, pass in a function that negates coef sign to polyMap
      (everything else remains the same) *)
   | _ -> polyMap (fun coef -> coef * -1) (fun expo -> expo) poly;;

let polyTermMultiply leftCoef leftExpo rightPoly =
   match rightPoly with
   | Empty -> Empty (* If poly is empty return empty. I think you get it at this point.*)
   | _ ->
      (* To get product: map functions that return the coef of poly * leftCoef and expo of poly + leftExpo *)
      polyMap (fun coef -> leftCoef * coef) (fun expo -> leftExpo + expo) rightPoly;;

let rec polyPolyAdd leftPoly rightPoly =
   match (leftPoly, rightPoly) with
   | (Empty, Empty) -> Empty
   | (_, Empty) -> leftPoly (* signifies end of left poly operation if empty. return left poly *)
   | (Empty, _) -> rightPoly (* same as above for right poly*)
   | (Term (lc, le, lp), Term (rc, re, rp)) ->
      if le = re then (* if term exponents match, then coefs are able to be combined*)
         if lc + rc = 0 then (* if sum is 0, just skip the term *)
            polyPolyAdd lp rp
         else 
            (* else make a term with the sum of the coefs, the current expo (I just went with le), and the next poly (which will be the continuation of the function) *)
            term (lc + rc) le (polyPolyAdd lp rp)
      else if le > re then
         (* if left expo is bigger, make a term with the current left term while continuing polyPolyAdd with the continuation of leftPoly while staying on the current right term *)
         term lc le (polyPolyAdd lp (Term (rc, re, rp)))
      else
         (* else do the same for the right term instead*)
         term rc re (polyPolyAdd (Term (lc, le, lp)) rp);;
   
let polyPolySubtract leftPoly rightPoly =
   (* not sure how much more efficient adding a match statement here is (if at all), 
      but certain cases will immediately return their respective value instead of following through with the two function calls*)
   match (leftPoly, rightPoly) with
   | (Empty, _) -> polyMinus rightPoly
   | (_, Empty) -> leftPoly
   | (_, _) ->
   polyPolyAdd leftPoly (polyMinus rightPoly);; (* add left poly w/ the negation of rightPoly (i.e. A + (-B) )*)

let rec polyPolyMultiply leftPoly rightPoly =
   match leftPoly with
   | Empty -> Empty
   | Term (lc, le, lp) -> (* match leftPoly with poly components *)
   (* for poly multiplication, sum products of each term of left poly w/ whole right poly*)
      polyPolyAdd 
      (polyTermMultiply lc le rightPoly) (* multiply right poly with term from left poly *)
       (polyPolyMultiply lp rightPoly);; (* move down left poly terms to multiply w/ right poly *)

(* Examples, for debugging only. THEY DO NOT DETERMINE YOUR GRADE! The comments
   show what should be printed if your functions are correct. *)

(* 3 x^5 + 2 x^4 + 2 x^3 − 1 x^2 + 5 x^0. *)
let p = (term 3 5 (term 2 4 (term 2 3 (term (-1) 2 (term 5 0 Empty))))) ;;
printPoly p ;;

(* 7 x^4 + 1 x^2 − 4 x^1 − 3 x^0. *)
let q = (term 7 4 (term 1 2 (term (-4) 1 (term (-3) 0 Empty)))) ;;
printPoly q ;;

(* 2 x^1 + 3 x^0 *)
let r = (term 2 1 (term 3 0 Empty)) ;;
printPoly r ;;

(* -3 x^5 - 2 x^4 - 2 x^3 + 1 x^2 - 5 x^0 *)
printPoly (polyMinus p) ;;

(* 6 x^6 + 4 x^5 + 4 x^4 - 2 x^3 + 10 x^1 *)
printPoly (polyTermMultiply 2 1 p) ;;

(* 3 x^5 + 9 x^4 + 2 x^3 - 4 x^1 + 2 x^0 *)
printPoly (polyPolyAdd p q) ;;

(* 3 x^5 - 5 x^4 + 2 x^3 - 2 x^2 + 4 x^1 + 8 x^0 *)
printPoly (polyPolySubtract p q) ;;

(* 6 x^6 + 13 x^5 + 10 x^4 + 4 x^3 - 3 x^2 + 10 x^1 + 15 x^0 *)
printPoly (polyPolyMultiply p r) ;;

(* My test cases *)
let s = 
   (term 5 5 (term 4 4 (term 3 3 (term 2 2 (term 1 1 (term 1 0 Empty))))));;
let t = polyMinus s;;
printPoly (polyPolyAdd s t);; (* poly added to its negation *)

let u = Empty;;
printPoly(polyMinus u);; (* negating an empty poly *)

printPoly(polyPolySubtract u s);; (* subtracting a poly from an empty poly*)

printPoly(polyPolySubtract s u);; (* subtracting an empty poly from a poly *)

(* Test Results

let p = (term 3 5 (term 2 4 (term 2 3 (term (-1) 2 (term 5 0 Empty))))) ;;
printPoly p ;;
Result: 3 x^5 + 2 x^4 + 2 x^3 - 1 x^2 + 5 x^0

let q = (term 7 4 (term 1 2 (term (-4) 1 (term (-3) 0 Empty)))) ;;
printPoly q ;;
Result: 7 x^4 + 1 x^2 - 4 x^1 - 3 x^0

let r = (term 2 1 (term 3 0 Empty)) ;;
printPoly r ;;
Result: 2 x^1 + 3 x^0

printPoly (polyMinus p) ;;
Result: -3 x^5 - 2 x^4 - 2 x^3 + 1 x^2 - 5 x^0

printPoly (polyTermMultiply 2 1 p) ;;
Result: 6 x^6 + 4 x^5 + 4 x^4 - 2 x^3 + 10 x^1

printPoly (polyPolyAdd p q) ;;
Result: 3 x^5 + 9 x^4 + 2 x^3 - 4 x^1 + 2 x^0

printPoly (polyPolySubtract p q) ;;
Result: 3 x^5 - 5 x^4 + 2 x^3 - 2 x^2 + 4 x^1 + 8 x^0

printPoly (polyPolyMultiply p r) ;;
Result: 6 x^6 + 13 x^5 + 10 x^4 + 4 x^3 - 3 x^2 + 10 x^1 + 15 x^0


Additional Tests
(See "My test cases" for the purposes of these tests)

printPoly (polyPolyAdd s t);;
Result: 0

printPoly(polyMinus u);;
Result: 0

printPoly(polyPolySubtract u s);;
Result: -5 x^5 - 4 x^4 - 3 x^3 - 2 x^2 - 1 x^1 - 1 x^0

printPoly(polyPolySubtract s u);;
Result: 5 x^5 + 4 x^4 + 3 x^3 + 2 x^2 + 1 x^1 + 1 x^0


*)
