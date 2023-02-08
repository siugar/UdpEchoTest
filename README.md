# UdpEchoTest
It's just for the test

This the UDP echo server and client code with C/C++ and POSIX library.

The code can run well in Debian 10. I will show it in the video demo.

HOW TO BUILD:
1. execute ./build in the folder Server to make the executable file 'server' with gcc.
2. execute ./build in the folder Client to make the executable file 'client' with gcc.

HOW TO RUN:
1. server --ip [server ip] --port [server port]

   [Default]
   server ip: 127.0.0.1
   server port: 1212
   
2. client --ip [server ip] --port [server port] --message [your message] --max_retry [max retry count]

   [Default]
   server ip: 127.0.0.1
   server port: 1212
   message: This is the message from client!!!
   max_retry: 10

   Note:
   
   The interval between each retry basically follow the expontential backoff algorithm.
   I slightly change the random factor to prevent the the collisions of clients when the retry interval is bound to 
   the maximum 8s(The requirement in the test). The random factor of the new way is all suitable for the retry 
   interval from small to big with the maximun boundary especially.
   
   wait_interval = ((base * multiplier^n) <= max interval) * (1 - random_interval_ratio[0.0 ~ 0.2))	  

   
   Original Reference: wait_interval = (base * 2^n) +/- (random_interval)  https://www.baeldung.com/resilience4j-backoff-jitter
   
	
	

