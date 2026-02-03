open List

let rec howMany e l =
  if l = [] then 0
  else if hd l = e then 1 + howMany e (tl l)
  else howMany e (tl l);;





