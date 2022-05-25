Commands to generate executable Producer and Consumer file:

g++ -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include Consumer.cpp -o ConsumerCpp -lrdkafka -lglib-2.0

g++ -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include Producer.cpp -o ProducerCpp -lrdkafka -lglib-2.0 -lbsd

Commands to start producing and consuming messsges:

./ProducerCpp Config.ini

./ConsumerCpp Config.ini
