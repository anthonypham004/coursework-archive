open List

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
