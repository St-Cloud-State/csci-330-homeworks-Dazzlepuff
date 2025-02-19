(defun insert (x lst)
  "Insert x into the sorted list lst in sorted order."
  (if (or (null lst) (<= x (car lst)))
      (cons x lst)
      (cons (car lst) (insert x (cdr lst)))))

(defun insertion-sort (lst)
  "Sort lst using insertion sort."
  (if (null lst)
      nil
      (insert (car lst) (insertion-sort (cdr lst)))))