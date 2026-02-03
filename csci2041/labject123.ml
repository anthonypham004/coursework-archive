(*  
  LABJECT123. The final lab and final project for CSci 2041.

    James Moen
    30 Apr 25

  Insert your code wherever it says REPLACE THIS COMMENT WITH YOUR CODE. You
  have written most of this code before. You need write only a small amount of
  new code to complete this assignment.
*)

open Printf ;; (* Define the function PRINTF. *)

(*  THING. A Lisp object. This is used by all subsequent modules.

    ENVIRONMENT is an association list that maps STRINGs from SYMBOLs to the
    THINGs that are the SYMBOL's bindings.

    CLOSURE (PARS, BODY, ENV) is a user function created by LAMBDA. PARS is a
    Lisp list of zero or more distinct SYMBOLs, the function's parameters. BODY
    is a Lisp expression, the function's body. ENV is an ENVIRONMENT that holds
    all SYMBOL bindings when the CLOSURE was created. It helps evaluate BODY.

    CONS builds Lisp lists. CONS (A, D) means the cons whose CAR is A and whose
    CDR is D. The Lisp list (E₁ E₂ ... Eⱼ) is represented as a series of nested
    CONSes, like this: CONS (E₁, CONS (E₂ ..., CONS (Eⱼ, NIL) ... )).

    NIL is the empty Lisp list. It also means FALSE. Any Lisp object other than
    NIL means TRUE. The Lisp symbol T is often used to mean TRUE.

    NUMBER K is the integer K.

    PRIMITIVE H is a Lisp function that's built into the evaluator. H is an
    OCaml function (FUN ARGS ENV -> BODY) that takes a Lisp list of arguments
    ARGS and an environment ENV. It returns the result of the OCaml expression
    BODY.

    SYMBOL S is a symbol whose characters are in the OCaml string S. *)

type
  thing =
    Closure of thing * thing * environment |
    Cons of thing * thing |
    Nil |
    Number of int |
    Primitive of (thing -> environment -> thing) |
    Symbol of string
and
  environment = (string * thing) list ;;

(* ------------------------------------------------------------------------- *)

(* EVALUATISH. EVALUATOR provides a function EVALUATE that evaluates a THING,
   and an exception EVALUATOR ERROR with an error message inside. *)

module type Evaluatish =
sig
  val evaluate: thing -> thing ;;
  exception EvaluatorError of string ;;
end ;;

(* EVALUATOR. The Lisp evaluator. *)

module Evaluator: Evaluatish =
struct

(* OOPS. Call this with a descriptive error MESSAGE in case of error. *)

  exception EvaluatorError of string ;;

  let oops message =
    raise (EvaluatorError message) ;;

(* ENV GET. Search the ENVIRONMENT ENV for the value of a SYMBOL whose string
   is NAME, and return that value. If we can't find it, then call continuation
   ETC. *)

  let envGet env name etc =
    let rec envGetting env =
      match env
      with [] ->
             etc () |
           (otherName, otherValue) :: otherEnv ->
             if name = otherName
             then otherValue
             else envGetting otherEnv
    in envGetting env ;;

(* ENV MAKE. Return a new empty ENVIRONMENT. *)

  let envMake () =
    [] ;;

(* ENV PUT. Return a new ENVIRONMENT that's like ENV except that a SYMBOL with
   the string NAME is bound to VALUE. *)

  let envPut name value env =
    (name, value) :: env ;;

(* TEE. The SYMBOL T. We use it to mean TRUE when we can't think of anything
   else. *)

  let tee = Symbol "t" ;;

(* GLOBAL. The global environment. It's a variable so DEFINE (see below) can
   change it. Initially it binds SYMBOLs T and NIL. *)

  let global = ref (envMake ()) ;;

  global := envPut "nil" Nil (! global) ;;
  global := envPut "t"   tee (! global) ;;

(* LOOKUP. Return the value of a SYMBOL whose string is NAME. First search the
   local ENVIRONMENT ENV. If we can't find NAME there, then search GLOBAL, the
   global ENVIRONMENT. It's an error if we can't find NAME there either. *)

  let lookup env name =
    envGet env name
      (fun () ->
        envGet (! global) name
          (fun () ->
            oops ("Unbound name " ^ name))) ;;

(* EVALUATING. Dispatcher. Evaluate the Lisp expression THING in the local
   ENVIRONMENT ENV. A CONS is a function call whose CAR must evaluate to either
   a CLOSURE or a PRIMITIVE, and whose CDR is a Lisp list of arguments. A
   SYMBOL is a name with a value in ENV. Anything else evaluates to itself. *)

  let rec evaluating thing env =
    match thing
    with Cons(func, args) ->
           (match (evaluating func env)
            with Closure (pars, body, bodyEnv) ->
                   apply pars args env body bodyEnv |
                 Primitive howTo ->
                   howTo args env |
                 _ ->
                   oops "Closure or primitive expected") |
         Symbol name ->
           lookup env name |
         _ ->
           thing

(* APPLY. Apply a user function whose parameters are the SYMBOLs in the Lisp
   list PARS, whose arguments are the expressions in the Lisp list ARGS, and
   whose body is the Lisp expression BODY. The arguments are evaluated in the
   ENVIRONMENT ARGS ENV, and the BODY is evaluated in the ENVIRONMENT BODY
   ENV. *)

  and apply pars args argsEnv body bodyEnv =
    let rec applying pars args bodyEnv =
      match (pars, args)
      with (Nil, Nil) ->
             evaluating body bodyEnv |
           (Nil, Cons (_, _)) ->
             oops "More arguments than parameters" |
           (Cons (_, _), Nil) ->
             oops "Fewer arguments than parameters" |
           (Cons (Symbol name, pars), Cons (arg, args)) ->
             applying pars args
               (envPut name (evaluating arg argsEnv) bodyEnv) |
           _ ->
             oops "Bad application"
    in applying pars args bodyEnv ;;

(* EVALUATE. Evaluate THING in the global ENVIRONMENT. *)

  let evaluate thing =
    evaluating thing (envMake ()) ;;

(* MAKE ARITHMETIC. Return an OCaml function that takes two NUMBER arguments,
   evaluates them both in ENV, and computes a new NUMBER from them using the
   OCaml function OP. If it doesn't work then assert an error MESSAGE. *)

  let makeArithmetic op message =
    (fun args env ->
      match args
      with Cons (left, Cons (right, Nil)) ->
             (let left = evaluating left env
              in let right = evaluating right env
                 in match (left, right)
                    with (Number left, Number right) ->
                           Number (op left right) |
                         _ ->
                           oops message) |
           _ ->
             oops message) ;;

(* MAKE RELATION. Return an OCaml function that takes two NUMBER arguments,
   evaluates them both in ENV, compares them using the OCaml function OP, and
   returns either NIL or T. If it doesn't work then assert an error MESSAGE. *)

  let makeRelation op message =
    (fun args env ->
      match args
      with Cons (left, Cons (right, Nil)) ->
             (let left = evaluating left env
              in let right = evaluating right env
                 in match (left, right)
                    with (Number left, Number right) ->
                           if op left right
                           then tee
                           else Nil |
                         _ ->
                           oops message) |
           _ ->
             oops message) ;;

(* PRIMITIVE. Bind a symbol with the string NAME to a PRIMITIVE that contains
   the OCaml function HOW TO. *)

  let primitive name howTo =
    global := envPut name (Primitive howTo) (! global) ;;

(* *. Multiply two NUMBERs and return a NUMBER. We must write the name of the
   OCaml multiplication function with extra blanks so it won't be read as a
   comment. *)

  primitive "*" (makeArithmetic ( * ) "* expected two numbers") ;;

(* +. Add two NUMBERs and return a NUMBER. *)

  primitive "+" (makeArithmetic (+) "+ expected two numbers") ;;

(* -. Negate a single NUMBER, or subtract two NUMBERs, returning a NUMBER. *)

  primitive "-"
    (fun args env ->
      match args
      with Cons (right, Nil) ->
             (match (evaluating right env)
              with Number right ->
                     Number (- right) |
                   _ ->
                     oops "- expected a number") |
           Cons (left, Cons (right, Nil)) ->
             let left = evaluating left env
             in let right = evaluating right env
                in (match (left, right)
                    with (Number left, Number right) ->
                           Number (left - right) |
                         _ ->
                           oops "- expected two numbers") |
           _ ->
             oops "- expected one or two numbers") ;;

(* /. Divide two NUMBERs and return a NUMBER. If the second NUMBER is 0 then we
   assert an error instead. *)

  primitive "/"
    (fun args env ->
      match args
      with Cons (left, Cons (right, Nil)) ->
             let left = evaluating left env
             in let right = evaluating right env
                in (match (left, right)
                    with (_, Number 0) ->
                           oops "/ tried to divide by 0" |
                         (Number left, Number right) ->
                           Number (left / right) |
                         _ ->
                           oops "/ expected two numbers") |
           _ ->
             oops "/ expected two numbers") ;;

(* <, <=, <>, >, >=. Comparisons that test two NUMBERs. *)

  primitive "<"  (makeRelation (<)  "< expected two numbers") ;;
  primitive "<=" (makeRelation (<=) "<= expected two numbers") ;;
  primitive "<>" (makeRelation (<>) "<> expected two numbers") ;;
  primitive ">"  (makeRelation (>)  "> expected two numbers") ;;
  primitive ">=" (makeRelation (>=) "< expected two numbers") ;;

(* "=". Test if two THINGs other than Lisp lists are equal. *)

  primitive "="
    (fun args env ->
      match args
      with Cons (left, Cons (right, Nil)) ->
             (let left = evaluating left env
              in let right = evaluating right env
                 in match (left, right)
                    with (Nil, Nil) ->
                           tee |
                         (Number left, Number right) ->
                           if left = right
                           then tee
                           else Nil |
                         (Symbol left, Symbol right) ->
                           if left = right
                           then tee
                           else Nil |
                         _ ->
                           Nil) |
           _ ->
             oops "= expected two arguments") ;;

(* AND. If there are no arguments then return T. Otherwise evaluate arguments
   left to right. If one returns NIL then return NIL without evaluating the
   others. Otherwise return the result of evaluating the last argument. *)

  primitive "and"
    (fun args env ->
      let rec anding args =
        match args
        with Nil ->
               tee |
             Cons (arg, Nil) ->
               evaluating arg env |
             Cons (arg, args) ->
               if (evaluating arg env) = Nil
               then Nil
               else anding args |
             _ ->
               oops "AND expected zero or more arguments"
      in anding args) ;;

(* CAR. Return the first element of a nonempty Lisp list. *)

  primitive "car"
   (fun args env ->
     match args
     with Cons (arg, Nil) ->
            (match (evaluating arg env)
             with Cons (first, _) ->
                   first |
                  _ ->
                    oops "CAR expected a CONS") |
          _ ->
            oops "CAR expected a CONS") ;;

(* CDR. Return a Lisp list without its first element. *)

  primitive "cdr"
   (fun args env ->
     match args
     with Cons (arg, Nil) ->
            (match (evaluating arg env)
             with Cons (_, rest) ->
                    rest |
                  _ ->
                    oops "CDR expected a CONS") |
          _ ->
            oops "CDR expected a CONS") ;;

(* CONS. Return a new Lisp list whose first element is the first argument, and
   whose remaining elements are in the second argument. *)

  primitive "cons"
    (fun args env ->
      match args
      with Cons (first, Cons (rest, Nil)) ->
             let first = evaluating first env
             in let rest = evaluating rest env
                in (match rest
                    with Cons (_, _) | Nil ->
                           Cons (first, rest) |
                         _ ->
                           oops "CONS expected an object and a list") |
           _ ->
             oops "CONS expected an object and a list") ;;

(* DEFINE. In the GLOBAL environment, bind the first argument, an unevaluated
   SYMBOL, to the result of evaluating the second argument. Finally return the
   SYMBOL. *)

  primitive "define"
    (fun args env ->
      match args
      with Cons (left, Cons (right, Nil)) ->
             (match left
              with Symbol name ->
                     global := envPut name (evaluating right env) (! global) ;
                     left |
                   _ ->
                     oops "DEFINE expected a symbol and an expression") |
           _ ->
             oops "DEFINE expected a symbol and an expression") ;;

(* IF. Accept three arguments: TEST, WHEN TRUE, and WHEN FALSE. First evaluate
   TEST. If it returns NIL, then evaluate the result of evaluating WHEN FALSE.
   Otherwise return the result of evaluating WHEN TRUE. *)

  primitive "if"
    (fun args env ->
      match args
      with Cons (test, Cons (whenTrue, Cons (whenFalse, Nil))) ->
             if evaluating test env = Nil
             then evaluating whenFalse env
             else evaluating whenTrue env |
           _ ->
             oops "IF expected three arguments") ;;

(* IS-CONS. Test if the single argument is a CONS, i.e., a nonempty list. *)

  primitive "is-cons"
    (fun args env ->
      match args
      with Cons (arg, Nil) ->
             (match (evaluating arg env)
              with Cons (_, _) ->
                     tee |
                   _ ->
                     Nil) |
           _ ->
             oops "IS-CONS expected one argument") ;;

(* IS-FUNCTION. Test if the single argument is a CLOSURE or a PRIMITIVE. *)

  primitive "is-function"
    (fun args env ->
      match args
      with Cons (arg, Nil) ->
             (match (evaluating arg env)
              with Closure (_, _, _) | Primitive (_) ->
                     tee |
                   _ ->
                     Nil) |
           _ ->
             oops "IS-FUNCTION expected one argument") ;;

(* IS-NUMBER. Test if the single argument is a NUMBER. *)

  primitive "is-number"
    (fun args env ->
      match args
      with Cons (arg, Nil) ->
             (match (evaluating arg env)
              with Number _ ->
                     tee |
                   _ ->
                     Nil) |
           _ ->
             oops "IS-NUMBER expected one argument") ;;

(* IS-SYMBOL. Test if the single argument is NIL or a SYMBOL. *)

  primitive "is-symbol"
    (fun args env ->
      match args
      with Cons (arg, Nil) ->
             (match (evaluating arg env)
              with Nil | Symbol _ ->
                     tee |
                   _ ->
                     Nil) |
           _ ->
             oops "IS-SYMBOL expected one argument") ;;

(* IS MEMBER. Helper for ARE PARAMETERS. Test if THING is a member of the Lisp
   list THINGS. *)

  let isMember thing things =
    let rec isMembering things =
      match things
      with Cons (first, rest) ->
             thing = first || isMembering rest |
           _ ->
             false
    in isMembering things ;;

(* ARE PARAMETERS. Helper for LAMBDA. Test if THINGS is a Lisp list of SYMBOLS,
   in which no SYMBOL appears more than once. *)

  let rec areParameters things =
    match things
    with Nil ->
           true |
         Cons (first, rest) ->
           (match first
            with Symbol _ ->
                   not (isMember first rest) && (areParameters rest) |
                 _ ->
                   false) |
         _ ->
           false ;;

(* LAMBDA. Return a closure. Its parameter list is the unevaluated first
   argument, which mist be a Lisp list of distinct SYMBOLs. Its body is the
   unevaluated second argument. Its ENVIRONMENT is ENV. *)

  primitive "lambda"
    (fun args env ->
      match args
      with Cons (pars, Cons (body, Nil)) ->
             if areParameters pars
             then Closure (pars, body, env)
             else oops "LAMBDA expected parameters and a body" |
           _ ->
             oops "LAMBDA expected parameters and a body") ;;

(* LIST. Return a Lisp list whose elements are the result of evaluating all the
   arguments. If there are no arguments then return NIL. *)

  primitive "list"
    (fun args env ->
      let rec listing args =
        match args
        with Nil ->
               Nil |
             Cons (arg, args) ->
               Cons (evaluating arg env, listing args) |
             _ ->
               oops "LIST expected zero or more arguments"
      in listing args) ;;

(* NOT. Test if the single argument is NIL. *)

  primitive "not"
    (fun args env ->
      match args
      with Cons (arg, Nil) ->
             if (evaluating arg env) = Nil
             then tee
             else Nil |
           _ ->
             oops "NOT expected one argument") ;;

(* OR. If there are no arguments then return NIL. Otherwise evaluate arguments
   left to right. If one returns a value other than NIL then return that value,
   without evaluating the others. Otherwise return the result of evaluating the
   last argument. *)

  primitive "or"
   (fun args env ->
     let rec oring args =
       match args
       with Nil ->
              Nil |
            Cons (arg, Nil) ->
              evaluating arg env |
            Cons (arg, args) ->
              let value = evaluating arg env
              in if value = Nil
                 then oring args
                 else value |
            _ ->
              oops "OR expected zero or more arguments"
     in oring args) ;;

(* QUOTE. Return the single argument without evaluating it. *)

  primitive "quote"
    (fun args _ ->
      match args
      with Cons (arg, Nil) ->
             arg |
           _ ->
             oops "QUOTE expected one argument") ;;
end ;;

(* ------------------------------------------------------------------------- *)

(* SCANNERISH. The type of the module SCANNER. Only INITIALIZE and NEXT TOKEN
   are visible outside SCANNER. *)

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

(* ------------------------------------------------------------------------- *)

(* PARSISH. PARSER provides an exception CAN'T PARSE and a function MAKE PARSER
   that returns a new parser. *)

module type Parsish =
sig
  exception Can'tParse of string;;
  val initialize : string -> unit;;
  val nextThing : unit -> thing;;
end ;;

(* PARSER. Read Lisp expressions from a file. *)

module Parser: Parsish =
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
end ;;

(* ------------------------------------------------------------------------- *)

(* PRINTISH. PRINT provides an exception BAD THING and a function PRINT THING
   that prints a Lisp THING. *)

module type Printish =
sig

  val printingThing: thing -> unit;;

end ;;

(* PRINTER. Print Lisp THINGs to standard output. *)

module Printer: Printish =
struct 
  let rec printingThing thing = 
    let rec printingThings things =
      match things with
      | Nil -> printf ""
      | Cons(firstThing, restThing) -> printf " ";printingThing firstThing; printingThings restThing 
      | _ -> ()
    in match thing with
    | Number(n) -> printf "%i" n
    | Symbol(s) -> printf "%s" s
    | Closure(_,_,_) -> printf "[Closure]"
    | Primitive(_) -> printf "[Primitive]"
    | Cons(fThing, rThing) -> printf "(" ;printingThing fThing; printingThings rThing; printf ")" 
    | Nil -> printf "nil"
end ;;

(* ------------------------------------------------------------------------- *)

(* LISPISH. Functions visible in the module LISP. It provides only REPL, which
   is a Read-Evaluate-Print-Loop for the Lisp interpreter. *)

module type Lispish =
sig
  val repl: unit -> unit
end ;;

(* LISP. Read Lisp programs from files and execute them. *)

module Lisp: Lispish =
struct

(* COMMAND ARGUMENTS. Call the continuation ETC on every STRING argument from
   the command line, one at a time, left to right. You need not know how this
   function works. Like many things in OCaml, it is far more complicated than
   it should be. *)

  let commandArguments etc =
    Arg.parse
      [ ("", (Arg.String (fun _ -> ())), "Zero or more arguments expected") ]
      etc
      "Argument expected" ;;

  let repl () =
    let rec replFromChannel channel =
      try
        (* Read from the channel *)
        let lexbuf = Lexing.from_channel channel in
        (* Initialize the parser with the channel *)
        Parser.initialize "";
        (* Set up a fake scanner input *)
        Scanner.input := channel;
        Scanner.nextChar ();
        (* Process all expressions in the channel *)
        let rec process () =
          try
            let thing = Parser.nextThing () in
            let result = Evaluator.evaluate thing in
            Printer.printingThing result;
            print_newline ();
            process ()
          with
          | Parser.Can'tParse msg ->
              print_endline msg;
              process ()
          | Evaluator.EvaluatorError msg ->
              print_endline ("Error: " ^ msg);
              process ()
          | End_of_file -> ()
        in
        process ()
      with
      | Sys_error msg ->
          print_endline ("Error: " ^ msg)
    in
    
    (* Process command line arguments or stdin *)
    let processFile path =
      try
        let channel = open_in path in
        replFromChannel channel;
        close_in channel
      with
      | Sys_error msg ->
          print_endline ("Error: " ^ msg)
    in
    
    (* Get command line arguments *)
    let files = ref [] in
    commandArguments (fun arg -> files := arg :: !files);
    match List.rev !files with
    | [] -> replFromChannel stdin
    | paths -> List.iter processFile paths

end ;;

(* ------------------------------------------------------------------------- *)

(* The big finish. Run it! *)

Lisp.repl () ;;
