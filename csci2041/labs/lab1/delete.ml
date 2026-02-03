open List

let rec delete e l =
  if l = [] then []
  else if hd l = e then delete e (tl l)
  else (hd l)::(delete e (tl l))
