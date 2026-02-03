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
  

     
    
(*
  CSci 2041 Tests for Lab Assignment 3

    James Moen
    11 Feb 25

  It's worth 40 points.
*)

(* BST. An unbalanced Binary Search Tree of KEYs. I've commented it out because
   you've already defined this type in your code.

type 'key bst = BstEmpty | BstNode of 'key * 'key bst * 'key bst ;;

*)

(* BST INSERT. Return a new BST that's like TREE, but with KEY. We need this to
   make the test cases below. *)

   let bstInsert tree key =
    let rec inserting subtree =
      match subtree
      with BstEmpty -> BstNode(key, BstEmpty, BstEmpty) |
           BstNode(otherKey, leftSubtree, rightSubtree) ->
             if key < otherKey
             then BstNode(otherKey, inserting leftSubtree, rightSubtree)
             else if key > otherKey
                  then BstNode(otherKey, leftSubtree, inserting rightSubtree)
                  else subtree
    in inserting tree ;;
  
  (* BST IS IN. Test if KEY is in TREE. It may help with debugging. Or not. *)
  
  let bstIsIn key tree =
    let rec isInning subtree =
      match subtree
      with BstEmpty -> false |
           BstNode(otherKey, leftSubtree, rightSubtree) ->
             if key < otherKey
             then isInning leftSubtree
             else if key > otherKey
                  then isInning rightSubtree
                  else true
    in isInning tree ;;
  
  (* Let T be a BST. We'll make it by adding nodes one at a time, so OCaml will
     print many intermediate BST's that we don't care about. Ignore those. *)
  
  let t = BstEmpty        ;;
  let t = bstInsert t 100 ;;
  let t = bstInsert t 70  ;;
  let t = bstInsert t 137 ;;
  let t = bstInsert t 53  ;;
  let t = bstInsert t 86  ;;
  let t = bstInsert t 74  ;;
  let t = bstInsert t 212 ;;
  let t = bstInsert t 149 ;;
  let t = bstInsert t 997 ;;
  
  (* This is the one we care about, but OCaml will indent it less clearly.
  
     BstNode (100,
       BstNode (70,
         BstNode (53, BstEmpty, BstEmpty),
         BstNode (86,
           BstNode (74, BstEmpty, BstEmpty),
           BstEmpty)),
       BstNode (137,
         BstEmpty,
         BstNode (212,
           BstNode (149, BstEmpty, BstEmpty),
           BstNode (997, BstEmpty, BstEmpty))))
  
     We'll delete nodes from it, one at a time. We'll start by deleting a node
     from a left subtree, with no children. *)
  
  let t = bstDelete t 149 ;;
  
  (* 5 points if you get this.
  
     BstNode (100,
       BstNode (70,
         BstNode (53, BstEmpty, BstEmpty),
         BstNode (86,
           BstNode (74, BstEmpty, BstEmpty),
           BstEmpty)),
       BstNode (137,
         BstEmpty,
         BstNode (212,
           BstEmpty,
           BstNode (997, BstEmpty, BstEmpty)))) *)
  
  (* Delete a node from a right subtree, with no children. *)
  
  let t = bstDelete t 997 ;;
  
  (* 5 points if you get this.
  
     BstNode (100,
       BstNode (70,
         BstNode (53, BstEmpty, BstEmpty),
         BstNode (86,
           BstNode (74, BstEmpty, BstEmpty),
           BstEmpty)),
       BstNode (137,
         BstEmpty,
         BstNode (212, BstEmpty, BstEmpty))) *)
  
  (* Delete a node from a right subtree, with one child. *)
  
  let t = bstDelete t 86 ;;
  
  (* 5 points if you get this.
  
     BstNode (100,
       BstNode (70,
         BstNode (53, BstEmpty, BstEmpty),
         BstNode (74, BstEmpty, BstEmpty)),
       BstNode (137,
         BstEmpty,
         BstNode (212, BstEmpty, BstEmpty))) *)
  
  (* Delete a node with two children. *)
  
  let t = bstDelete t 100 ;;
  
  (* 10 points if you get this. I'm assuming you replaced 100 with the largest
     key 74 in the left subtree, then deleted its node.
  
     BstNode (74,
       BstNode (70,
         BstNode (53, BstEmpty, BstEmpty),
         BstEmpty),
       BstNode (137,
         BstEmpty,
         BstNode (212, BstEmpty, BstEmpty))) *)
  
  (* Delete a node from the left subtree, with one child. *)
  
  let t = bstDelete t 70 ;;
  
  (* 5 points if you get this.
  
     BstNode (74,
       BstNode (53, BstEmpty, BstEmpty),
       BstNode (137,
         BstEmpty,
         BstNode (212, BstEmpty, BstEmpty))) *)
  
  (* Delete a node from the right subtree, with one child. *)
  
  let t = bstDelete t 137 ;;
  
  (* 5 points if you get this.
  
     BstNode (74,
       BstNode (53, BstEmpty, BstEmpty),
       BstNode (212, BstEmpty, BstEmpty)) *)
  
  (* The big finish. Delete all the remaining nodes! *)
  
  let t = bstDelete t 53  ;;
  let t = bstDelete t 212 ;;
  let t = bstDelete t 74  ;;
  
  (* 5 points if you get BstEmpty in the end. *)