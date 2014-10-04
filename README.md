ofxLibXtract
============

LibXtract example for openFrameworks

Spectral centroid and MFCCs giving output - requires checking

testApp version was with OF0.7.x
and new ofApp version tested with OF0.8.x



to compile -
1. grab LibXtract from Github
add the entire LibXtract/src to the project

2. take an empty example - throw out the testApp.h and .cpp files from the src folder and replace with the files here:
testApp.h/cpp
libXtract.h and cpp

It should be that simple

But results are not verified. 18/12/13


3. error: libxtract.h not found:
add header file link to

/Users/Yourself/Github/Libxtract

by going into build settings, scroll down to header search path
and make this recursive

as example, mine is "/Users/andrewrobertson/GitHub/LibXtract"



The example will show a spectral centroid moving. Very simple, but you can then get into libXtract and do your own features, MFCCs, Kurtosis etc


tested with wavetable