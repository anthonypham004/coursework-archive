(*
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
