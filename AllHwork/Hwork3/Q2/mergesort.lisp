(defun partition (lst)
  (cond
    ((null lst) (list nil nil))                   ; No items: return two empty lists.
    ((null (cdr lst)) (list (list (car lst)) nil))  ; One item: return list with the item in the first half.
    (t (let ((subparts (partition (cddr lst))))    ; Otherwise, take two items:
         (list (cons (car lst) (first subparts))     ; First item into the first list.
               (cons (cadr lst) (second subparts)))))))

(defun merge-lists (left right)
  (cond
    ((null left) right)
    ((null right) left)
    ((<= (first left) (first right))
     (cons (first left) (merge-lists (rest left) right)))
    (t
     (cons (first right) (merge-lists left (rest right))))))

(defun mergesort (lst)
  (if (or (null lst) (null (cdr lst)))
      lst  ; Base case: a list with 0 or 1 element is already sorted.
      (let* ((parts (partition lst))
             (left (mergesort (first parts)))
             (right (mergesort (second parts))))
        (merge-lists left right))))