# Bruceforcer 1.0
  By Erik Alvarez / aka Qdo

# Setup
 * Unzip repository
 * make

# Run
 ./bruceforcer -h 'hash with salt'

# optional parameters
  -b	Go straight into bruceforce
  
  -s <N bytes> 
	set max bytesize flag.
	If will stop when hitting this size
  
  -t <N threads> 
	set threads
  
  -n <N> Jump into a wordlength
  
  -d <directorypath/>
	Change directory

# How to works
 Bruceforcer will start searching in the directory folder,
if no password is found, it will continue to the bruteforce part.

 Bruceforcer will start iterating by default at length 2, and work its way up to
longer passwords after the function has tried every combination possible.

 use -n <N> to override the start position of the iterator. everything above 7 takes extremely long time, good luck :)

# Modifications
 Match ALPHABET: functions.h