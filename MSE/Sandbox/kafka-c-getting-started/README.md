Commands to generate executable Producer and Consumer file:

cc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include producer.c   -o producer -lrdkafka -lglib-2.0 -lbsd
cc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include consumer.c  -o consumer  -lrdkafka -lglib-2.0

Commands to start producing and consuming messsges:

./producer getting-started.ini
./consumer getting-started.ini
