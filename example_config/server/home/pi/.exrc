" vi.exrv fuer vi
" Author: Klaus Franken, kfr@suse.de
" Version: 06.06.97

" automatisch einruecken

" suchen case-insenitiv
" set ignorecase

" Koordinatenanzeige aktivieren
set ruler

" shell to start with !
" set shell=sh

" zeige passende klammern
set showmatch

" anzeige INSERT/REPLACE/...
set showmode

" Tastatur-Belegung fuer diverse  vi's
" Autor: Werner Fink   <werner@suse.de> 
" Version: 20.05.1997

" keys in display mode
map OA  k
map [A  k
map OB  j
map [B  j
map OD  h
map [D  h
map     h
map     h
map OC  l
map [C  l
map [2~ i
map [3~ x
map [1~ 0
map OH  0
map [H  0
map [4~ $
map OF  $
map [F  $
map [5~ 
map [6~ 
map [E  ""
map [G  ""
map OE  ""
map Oo  :
map Oj  *
map Om  -
map Ok  +
map Ol  +
map OM  
map Ow  7
map Ox  8
map Oy  9
map Ot  4
map Ou  5
map Ov  6
map Oq  1
map Or  2
map Os  3
map Op  0
map On  .

" keys in insert mode
map! Oo  :
map! Oj  *
map! Om  -
map! Ok  +
map! Ol  +
map! OM  
map! Ow  7
map! Ox  8
map! Oy  9
map! Ot  4
map! Ou  5
map! Ov  6
map! Oq  1
map! Or  2
map! Os  3
map! Op  0
map! On  .
