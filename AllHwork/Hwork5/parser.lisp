(defun match (token tokens)
  "Checks if the first token matches the expected one and consumes it."
  (if (and tokens (eq (car tokens) token))
      (cdr tokens)
      (error "Syntax Error: Unexpected token")))

(defun parse-G (tokens)
  "Parses G → x | y | z | w"
  (if (and tokens (member (car tokens) '(x y z w)))
      (match (car tokens) tokens)
      (error "Syntax Error: Expected x, y, z, or w")))

(defun parse-E-prime (tokens)
  "Parses E' → o G E' | ε"
  (if (and tokens (eq (car tokens) 'o))
      (progn
        (setq tokens (match 'o tokens))
        (setq tokens (parse-G tokens))
        (parse-E-prime tokens))
      tokens))  ;; ε case, return unchanged tokens

(defun parse-E (tokens)
  "Parses E → G E'"
  (setq tokens (parse-G tokens))
  (parse-E-prime tokens))

(defun parse-L-prime (tokens)
  "Parses L' → s L' | ε"
  (if (and tokens (eq (car tokens) 's))
      (progn
        (setq tokens (match 's tokens))
        (parse-L-prime tokens))
      tokens))  ;; ε case, return unchanged tokens

(defun parse-L (tokens)
  "Parses L → s L'"
  (setq tokens (match 's tokens))
  (parse-L-prime tokens))

(defun parse-S (tokens)
  "Parses S → s | d L b"
  (cond
    ((and tokens (eq (car tokens) 's))
     (match 's tokens))
    ((and tokens (eq (car tokens) 'd))
     (setq tokens (match 'd tokens))
     (setq tokens (parse-L tokens))
     (match 'b tokens))
    (t (error "Syntax Error: Expected s or d"))))

(defun parse-I-prime (tokens)
  "Parses I' → e S | ε"
  (if (and tokens (eq (car tokens) 'e))
      (progn
        (setq tokens (match 'e tokens))
        (parse-S tokens))
      tokens))  ;; ε case, return unchanged tokens

(defun parse-I (tokens)
  "Parses I → i E S I'"
  (setq tokens (match 'i tokens))
  (setq tokens (parse-E tokens))
  (setq tokens (parse-S tokens))
  (parse-I-prime tokens))

(defun parse (tokens)
  "Main function to start parsing with I as the starting symbol."
  (setq tokens (parse-I tokens))
  (if (null tokens)
      (format t "Parsing successful!~%")
      (error "Syntax Error: Unexpected extra input")))