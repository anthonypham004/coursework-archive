(* lab 1 *)
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



(* lab 2 *)
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



(* lab 3 *)
type 'key bst =
   BstEmpty | 
   BstNode of 'key * 'key bst * 'key bst;;


(* bstMaxKey helper definition *)
exception BadEmptyBst
let rec bstMaxKey subtree =
  match subtree with
  (* No higher node pointed, return current node's key *)
  | BstNode(currKey, _, BstEmpty) -> currKey
  (* Right subtree exists, recursively call function with right subtree (moving higher up in value) *)
  | BstNode(_, _, rightSubtree) -> bstMaxKey rightSubtree
  (* Node is empty therefore invalid, raise exception *)
  | BstEmpty -> raise BadEmptyBst

let rec bstDelete tree key =
  let rec deleting subtree =
    match subtree with
    (* Case 1: Empty BST *)
    | BstEmpty -> BstEmpty
    (* Case 2: Empty subtrees *)
    | BstNode(otherKey, BstEmpty, BstEmpty) ->
      if otherKey = key 
        then BstEmpty 
      else subtree
    (* Case 3: empty right subtree *)
    | BstNode(otherKey, leftSubtree, BstEmpty) ->
      (* Return subtree built so far if this next one is the key *)
      if otherKey = key
        then leftSubtree
      (* Otherwise keep going w/ recursion *)
      else BstNode(otherKey, deleting leftSubtree, BstEmpty)
    (* Case 4: empty left subtree *)
    | BstNode(otherKey, BstEmpty, rightSubtree) ->
      (* Same idea as case 3 lol *)
      if otherKey = key
        then rightSubtree
      else
        BstNode(otherKey, BstEmpty, deleting rightSubtree)
    (* Case 5: non-empty subtrees *)
    | BstNode(otherKey, leftSubtree, rightSubtree) -> (* TODO: finish *)
      if otherKey = key
        (* Get the closest value of subtree and delete its node to replace this key with to maintain order *)
        then let maxLeftKey = bstMaxKey leftSubtree in BstNode(maxLeftKey, bstDelete leftSubtree maxLeftKey, rightSubtree)
      else
        BstNode(otherKey, deleting leftSubtree, deleting rightSubtree)
        
  in deleting tree;;



(* lab 4 *)
let choose etc things =
  let rec choosing things =
    match things with
    | [] -> ()
    | first::rest -> (etc first; choosing rest)
  in choosing things;;


let allbut things thing =
  let rec allbutting things =
    match things with
    | [] -> []
    | head::tail ->
        if head = thing then tail
        else head::(allbutting tail)
  in allbutting things;;


let permute etc things =
  let rec permuting permuted unpermuted =
    match unpermuted with
    | [] -> etc permuted (* maybe add "choose" if needed *)
    | _ -> choose (fun thing -> permuting (thing::permuted) (allbut unpermuted thing)) unpermuted
  in permuting [] things;;

  (* 
  Breaking second case down (messy, but it's for me to keep track of this as I go):

    anon function (etc for choose): (fun thing -> permuting (thing::permuted) (allbut unpermuted thing)) 
    - recursively call permuting for matching unpermuted where...
      - thing is added to permuted and passed in as permuted
      - the rest is passed as unpermuted
      - this means the unpermuted list gets shorter as each thing is added to permuted and removed from unpermuted
    
    The choose function assists in doing this process for each element in unpermuted
    - both for the initial call and the recursive call, each element is operated on using the etc function
    - each thing and iteration of the list experiences the shrinking and transfer of the list head

    Each item in the list should get the chance to be the head and each shrinking iteration of the list experiences a different order of the transfers

    Once a call for unpermuted is empty, a permuted list is done and etc (print function) gets called on it. This should cover all the bases.
  *)



(* lab 5 *)
(* MAKE STREAM. Return a new stream. THIS is the first element of the stream.
   STATE is an object that stores the stream's state somehow. The function NEXT
   takes THIS and STATE as arguments. It returns a 2-tuple with a new THIS and
   a new STATE inside it. *)

   let makeStream this state next =
    ((this, state), next) ;;
  
  (* FIRST. Return the first element of a stream. *)
  
  let first ((this, state), next) =
    this ;;
  
  (* REST. Return a stream with its first element removed. *)
  
  let rest ((this, state), next) =
    (next this state, next) ;;
  
  (* TAKE. Return a list of the first COUNT elements TAKEn from STREAM. *)
  
  let rec take count stream =
    match count
    with 0 -> [] |
         _ -> (first stream) :: take (count - 1) (rest stream) ;;
  
  (* NATURALS. A infinite stream of 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 ... We don't use
     STATE here, so we let it be the dummy unit object (). *)
  
  let naturals =
    makeStream 0 () (fun this state -> (this + 1, ())) ;;

(* My code *)
  let odds =
    makeStream 1 () (fun this state -> (this + 2, ()));;

  let trim count stream =
    let rec trimming counter snippet =
      if counter = count 
        then snippet
      else
        trimming (counter + 1) (rest snippet)
    in trimming 0 stream;;
  
  let scale factor stream =
    makeStream
    (factor * (first stream))
    (rest stream)
    (fun this state ->
      (factor * (first state)), (rest state));;

  let sum left right =
    makeStream
    ((first left) + (first right)) (* add first elements of both streams*)
    ((rest left), (rest right)) (* pass tuple of rest of left and right as state*)
    (fun this (stateLeft, stateRight) -> 
      ((first stateLeft) + (first stateRight)), (rest stateLeft, rest stateRight));;



(* lab 6 *)
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