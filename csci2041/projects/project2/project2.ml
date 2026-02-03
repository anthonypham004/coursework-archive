(* (*
   SCANNER. A lexical scanner module for Lisp.

     James Moen
     07 Apr 25
*)

(* SCANNERISH. The type of the module SCANNER. Only INITIALIZE, NEXT TOKEN, and
   TOKEN (with its constructors) are visible outside SCANNER. *)

   module type Scannerish =
   sig
     type token =
       CloseParenToken |
       EndToken |
       NumberToken of int |
       OpenParenToken |
       SymbolToken of string ;;
   
    val initialize: string -> unit ;;
   
    val nextToken: unit -> token ;;
   end ;;
   
   (* SCANNER. A lexical scanner for Lisp. *)
   
   module Scanner: Scannerish =
   struct
   
   (* TOKEN. A Lisp token. *)
   
     type token =
       CloseParenToken |
       EndToken |
       NumberToken of int |
       OpenParenToken |
       SymbolToken of string ;;
   
   (* INPUT. Read CHARs from this input channel. *)
   
     let input = ref stdin ;;
   
   (* CH. The CHAR most recently read from INPUT. *)
   
     let ch = ref ' ' ;;
   
   (* NEXT CHAR. Advance CH to the next CHAR from INPUT. If we're at the end of
      INPUT, so there is no next CHAR, then let CH be '\000' instead *)
   
     let nextChar () =
       try ch := input_char ! input
       with End_of_file ->
              ch := '\000' ;;
   
   (* INITIALIZE. Initialize SCANNER so it reads CHARs from a file whose pathname
      is PATH, a STRING. This must be called once before we call NEXT TOKEN. *)
   
     let initialize path =
       input := open_in path ;
       nextChar () ;;
   
   (* NEXT CLOSE PAREN TOKEN. Advance SCANNER past a close parenthesis. *)
   
     let nextCloseParenToken () =
       nextChar ();
       CloseParenToken ;;
   
   (* NEXT COMMENT. Advance SCANNER past a comment. A Lisp comment begins with a
      semicolon and ends at a newline. *)
   
     let rec nextComment () =
       match ! ch
       with '\000' ->
              () |
            '\n' ->
              nextChar () |
            _ ->
              nextChar () ;
              nextComment () ;;
   
   (* NEXT END TOKEN. Advance SCANNER to the end of INPUT. Don't call NEXT CHAR
      because there are no more CHARs to read. *)
   
     let nextEndToken () =
       EndToken ;;
   
   (* NEXT NUMBER TOKEN. Advance SCANNER past a number. If it's not a number then
      advance past a symbol instead. *)
   
     let nextNumberToken () =
       let rec nextNumbering chars =
         match ! ch
         with '\000' | '\n' | ' ' | '(' | ')' ->
                (try
                   NumberToken (int_of_string chars)
                 with
                   Failure _ ->
                     SymbolToken chars) |
              _ ->
                let otherChars = Char.escaped ! ch
                in nextChar () ;
                   nextNumbering (chars ^ otherChars)
       in nextNumbering "" ;;
   
   (* NEXT OPEN PAREN TOKEN. Advance SCANNER past a close parenthesis. *)
   
     let nextOpenParenToken () =
       nextChar () ;
       OpenParenToken ;;
   
   (* NEXT SYMBOL TOKEN. Advance SCANNER past a symbol. *)
   
     let nextSymbolToken () =
       let rec nextSymboling chars =
         match ! ch
         with '\000' | '\n' | ' ' | '(' | ')' ->
                SymbolToken chars |
              _ ->
                let otherChars = Char.escaped ! ch
                in nextChar () ;
                   nextSymboling (chars ^ otherChars)
       in nextSymboling "" ;;
   
   (* NEXT TOKEN. Get the next TOKEN from INPUT. Advance SCANNER past that TOKEN,
      skipping comments and whitespace, then return the TOKEN. *)
   
     let rec nextToken () =
       match ! ch
       with '\000' ->
              nextEndToken () |
   
            ' ' | '\n' ->
              nextChar () ;
              nextToken () |
   
            '(' ->
              nextOpenParenToken () |
   
            ')' ->
              nextCloseParenToken () |
   
            '-' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' ->
              nextNumberToken () |
   
            ';' ->
              nextComment () ;
              nextToken () |
   
            _ ->
              nextSymbolToken () ;;
   end ;;
    *)


(* PARSER CODE aka my stuff *)
type thing = 
  Closure of thing * thing * environment | 
  Cons of thing * thing | 
  Nil | 
  Number of int | 
  Primitive of (thing -> environment -> thing) | 
  Symbol of string 
and 
  environment = (string * thing) list ;;


module type Parserish =
sig
  exception Can'tParse of string;;
  val initialize : string -> unit;;
  val nextThing : unit -> thing;;
end;;

module Parser : Parserish =
struct
  exception Can'tParse of string;;
  let uhoh message = raise (Can'tParse message);; (* my version of an oops function *)

  (* token variable, storing each chunk of input read from file *)
  let token = ref Scanner.EndToken (* placeholder value to initialize token to *)
  (* nextToken utilizes the function from Scanner to set token to next chunk of input *)
  let nextToken () =
    token := Scanner.nextToken ();;

  (* initialize file given through path and set the first token read from it*)
  let initialize path =
    Scanner.initialize path; token := Scanner.nextToken ();;

  (* compares the value of var token with the possible token types and returns the corresponding thing *)
  let rec nextThing () =
    match !token with
    (* nextThing shouldn't encounter list ends. Return error message. *)
    | CloseParenToken -> uhoh "ERROR: Unexpected list end ')'. Did you mean to start a list with '('?"
    (* end of file shouldn't be handled by parser. Return error message. *)
    | EndToken -> uhoh "ERROR: End of file unproperly handled during parsing."
    (* move on to the next token while (whilst?) returning the number value *)
    | NumberToken n -> nextToken () ; Number n
    (* start reading the first thing in the list and have nextThings handle the list contents *)
    | OpenParenToken -> nextToken (); nextThings ()
    (* kinda self explanatory but the string "nil" will return nil *)
    | SymbolToken "nil" -> nextToken (); Nil
    (* return whatever symbol the token is *)
    | SymbolToken s -> nextToken (); Symbol s
  (* handles the contents of a list upon parser reaching an open parenthesis *)
  and nextThings () =
    match !token with
    (* continue reading the next token and end the list with Nil *)
    | CloseParenToken -> nextToken (); Nil
    (* list shouldn't abruptly end with no close parenthesis. Return error message. *)
    | EndToken -> uhoh "ERROR: Unproperly closed list by end of file. Did you forget to use ')'?"
    (* anything between the parentheses is concatenated with whatever's read next *)
    | _ -> 
        let car = nextThing () in (* head of list *)
        let cdr = nextThings () in (* tail of list *)
        Cons (car, cdr) (* cons the current item as head with remaining list as tail *)
end;;

(*
If token is CloseParenToken then raise Can'tParse.
If token is EndToken then raise Can'tParse.
If token is NumberToken n then return Number n.
If token is OpenParenToken then read a Lisp list and return it.  
If token is SymbolToken "nil" then return Nil.
If token is SymbolToken s then return Symbol s.   
*)


(* Test *)
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
