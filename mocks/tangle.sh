# -*- mode: shell-script -*-
#
# tangle files with org-mode
#
# props to th.ing
# https://github.com/thi-ng/fabric/blob/master/tangle.sh

DIR=`pwd`

emacs -Q --batch \
--eval "(progn
(require 'org)(require 'ob)(require 'ob-tangle)(require 'ob-lob)
(setq org-confirm-babel-evaluate nil)
(mapc (lambda (file)
       (find-file (expand-file-name file \"$DIR\"))
       (org-babel-tangle)
       (kill-buffer)) '(\"mock.org\")))" #2>&1 |grep tangled
