;;;; Bottom-up mergesort in Lisp

;; First, we need a function that merges two sorted lists.
(defun merge-lists (left right)
  "Merge two sorted lists LEFT and RIGHT into one sorted list."
  (cond
    ((null left) right)
    ((null right) left)
    ((<= (first left) (first right))
     (cons (first left) (merge-lists (rest left) right)))
    (t
     (cons (first right) (merge-lists left (rest right))))))

;; Next, we convert the original list into a list of sorted pairs.
(defun make-sorted-pairs (lst)
  "Partition LST into a list of sorted pairs.
If LST has an odd number of elements, the last element is returned as a singleton list."
  (cond
    ((null lst) nil)
    ((null (cdr lst)) (list (list (first lst))))
    (t (let ((a (first lst))
             (b (second lst)))
         (cons (if (<= a b) (list a b) (list b a))
               (make-sorted-pairs (cddr lst)))))))

;; Now we define one “pass” which merges adjacent sublists.
(defun merge-pass (sublists)
  "Merge adjacent sublists in SUBLISTS.
If there is an odd number of sublists, the last one is carried over unchanged."
  (cond
    ((null sublists) nil)
    ((null (rest sublists)) sublists)
    (t (cons (merge-lists (first sublists) (second sublists))
             (merge-pass (cddr sublists))))))

;; Finally, the bottom-up mergesort itself.
(defun bottom-up-mergesort (lst)
  "Sort LST using a bottom-up mergesort algorithm."
  (labels ((merge-all (sublists)
             (if (null (rest sublists))
                 (first sublists)  ; Only one list left: we're done.
                 (merge-all (merge-pass sublists)))))
    (if (null lst)
        nil
        (merge-all (make-sorted-pairs lst)))))