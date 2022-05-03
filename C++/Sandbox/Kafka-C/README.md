Commands to generate executable Producer and Consumer file:

cc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include Producer.c -o Producer -lrdkafka -lglib-2.0 -lbsd
cc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include Consumer.c -o Consumer  -lrdkafka -lglib-2.0

Commands to start producing and consuming messsges:

./Producer Config.ini
./Consumer Config.ini
