;  !. Return the factorial of N for small N ≥ 0. This helps test SCANNER.ML.

(define !
  (lambda (n)
    (if
      (= n 0)
      1
      (* n (! (- n 1))))))
