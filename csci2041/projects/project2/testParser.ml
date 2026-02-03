(*
   TEST PARSER. Test the Lisp parser.

     James Moen
     08 Apr 25
*)

(* To run this program you must type the following to the OCaml toplevel:

     #use "scanner.ml" ;;
     #use "parser.ml" ;;

   You must also have the file factorial.lsp in your directory. *)

Parser.initialize "factorial.lsp" ;;
Parser.nextThing () ;;

(* You should see the following horrible THING. It's the OCaml representation
   of the Lisp list that defines the factorial function.

     Cons (Symbol "define",
      Cons (Symbol "!",
       Cons
        (Cons (Symbol "lambda",
          Cons (Cons (Symbol "n", Nil),
           Cons
            (Cons (Symbol "if",
              Cons (Cons (Symbol "=", Cons (Symbol "n", Cons (Number 0, Nil))),
               Cons (Number 1,
                Cons
                 (Cons (Symbol "*",
                   Cons (Symbol "n",
                    Cons
                     (Cons (Symbol "!",
                       Cons
                        (Cons (Symbol "-",
                          Cons (Symbol "n", Cons (Number 1, Nil))),
                        Nil)),
                     Nil))),
                 Nil)))),
            Nil))),
        Nil)))

  If your print function from Lab 9 works, then you can call it on this THING
  to see Lisp code in a readable form.
*)
