Reference:
"ENCARA2: Real-time Detection of Multiple Faces at Different Resolutions in Video Streams"
M. Castrillón, O. Déniz, C. Guerra, M. Hernández
Journal of Visual Communication and Image Representation, ISSN 1047-3203, vol 18, issue 2, pp. 130-140, April 2007.


Compilation in Linux:
-----------------------------

Three steps:

* 1.- makefiles generation:

	make -f makeMakeObjects.mak

(Using OpenFrameworks the command should be make -f makeMakeObjects-OF.mak)

* 2.- Library generation:

	make -f encara2-lib.mak

* 3.- Sample application generation.

	make -f encara2-test.mak

* 4.- Demo execution

	./bin/encara2-test -path "/path/ENCARA2data"

Remember to add the ENCARA2 lib folder in LD_LIBRARY_PATH to allow the application to locate libencara-raw.so


If compilation options are needed, use extraFlags:

	Examples: 
		make extraFlags="-ggdb -O0" -f makeMakeObjects.mak
		make extraFlags="-ggdb -O0" -f encara2-lib.mak
		make extraFlags="-ggdb -O0" -f encara2-test.mak

Valid options are those of g++ GNU compiler.
