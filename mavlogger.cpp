/*
*	Hello! Use this to compile it:
*	g++ mavlogger.cpp -std=c++11 -I /usr/local/include/mavsdk -lmavsdk -lmavsdk_mavlink_passthrough -o mavlogger
*
*	It assumes you are using tcp. If you are not then change the connection_url accordingly.
*	If you are using udp the connection might be 127.0.0.1:14540
*	If you use "udp://0.0.0.0:14540", 
*	it should attempt to connect to an open udp connection port 14540
*	 
*/
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

using namespace mavsdk;

const int SIZEOFLIST = 400;

int msgArray[SIZEOFLIST] = { 0 };

bool logMessages(mavlink_message_t message)
{
	if(message.compid == 1)
		msgArray[message.msgid]++;

	return true;
};

void createFile()
{
	std::ofstream log;
	log.open( "log.txt" );

	for(int i = 0; i < SIZEOFLIST; i++)
	{
		if(msgArray[i] > 0)
			log << "ID: " << i << " Count: " << msgArray[i] << "\n";
	}

	log.close();

	printf("Log Created\n");
};


int main(int argc, char **argv)
{
	
	
	Mavsdk dc;
	std::string connection_url = "tcp://0.0.0.0:5760";
	ConnectionResult connection_result = dc.add_any_connection(connection_url);

	std::printf("Connection %s", connection_result_str(connection_result) );

	System& system = dc.system();

	while (!dc.is_connected())
	{
		printf("Waiting for Heartbeat\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));	
	}

	auto mavPassthrough = std::make_shared<MavlinkPassthrough>( system );

	mavPassthrough->intercept_incoming_messages_async(&logMessages);

	int timer = 10;

	printf("Starting to collect data for %d seconds\n", timer);

	int count = 0;
	while(count < timer)
	{
		printf("Collecting Data.. Will turn off when connection ends\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		count++;
	}

	printf ("Ending...\n");

	createFile();

	printf( "End!\n" );

	return 0;
}
