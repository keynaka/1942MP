#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include "../data/player_data.h"
#include "../logger/Logger.h"
class Transmitter {
  private:
    int peerFD;
    Logger* logger;

  public:
    Transmitter( int peerFD, Logger* logger );
    ~Transmitter();
    bool sendData( PlayerData* data );
    bool receiveData( char id[2], int size );
    bool receiveData( PlayerData* data );
};
#endif
