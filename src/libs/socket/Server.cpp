#include "Server.h"
#include "../socket/sock_dep.h" /* socket dependencies */
#include "../../xml/parser/XMLParser.h"
#include "../palette/palette.h"
#include "../../utils/K.h"
#define DEBUG 1
#include "../debug/dg_msg.h"
#include <regex>
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;

Server::Server( const char* configFileName ) {
  this->socketFD = 0;
  this->clientCount = 0;
  this->listening = false;
  this->connected = false;
  this->processing = false;
  this->allowConnections = false;
  this->msgQueue = new queue<map<int, Mensaje*>*>;
  this->logger = Logger::instance();
  this->config = XMLParser::parseServerConf( configFileName );
}

Server::~Server() {
  delete this->msgQueue;
}

void Server::initialize() {
  if( this->connected ) {
    this->logger->warn( CONNECTION_ACTIVE );
    DEBUG_WARN( CONNECTION_ACTIVE );
    return;
  }

  int cfd; // client file descriptor
  struct sockaddr_storage client_addr; // client address information
  struct addrinfo hints, *servinfo, *p; // configuration structs
  int rv;

  // init hints struct with 0
  memset( &hints, 0, sizeof( hints ) );

  // set hints struct values
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use host IP

  // fill configuration structs
  if( ( rv = getaddrinfo( NULL, to_string( this->config->getPort() ).c_str(), &hints, &servinfo ) ) != 0 ) {
    this->logger->error( "Error al obtener la direccion, " + string( gai_strerror( rv ) ) );
    exit( -1 );
  }

  int yes = 1;
  // loop through results and bind to one of them
  for( p = servinfo; p != NULL; p = p->ai_next ) {
    // try to create TCP socket
    if( ( this->socketFD = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 ) {
      this->logger->error( SOCKET_ERROR );
      continue; // try next one
    }

    // allow port reuse to avoid bind error
    if( setsockopt( this->socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
      this->logger->error( SOCKET_REUSE_ERROR );
      exit( -1 );
    }

    // bind socket
    if( bind( this->socketFD, p->ai_addr, p->ai_addrlen ) == -1 ) {
      close( this->socketFD );
      this->logger->error( BIND_ERROR );
      continue; // try next one
    }

    break; // socket created and binded
  }
  freeaddrinfo( servinfo ); // free memory

  if( p == NULL ) {
    this->logger->error( BIND_CERROR );
    exit( -1 );
  }

  // listen for connections
  if( listen( this->socketFD, this->BACKLOG ) == -1 ) {
    this->logger->error( LISTEN_ERROR );
    exit( -1 );
  }

  this->listening = true;
  this->connected = true;
  this->processing = true;
  thread processor( &Server::processQueue, this );
  processor.detach();
  this->logger->info( SERVER_START );
  DEBUG_NOTICE( SERVER_START );

  // accept connections
  thread t2( &Server::listenForConnections, this, cfd, client_addr );
  t2.detach();
}

void Server::listenForConnections( int cfd, struct sockaddr_storage client_addr ) {
  // accept connections
  while( this->listening ) {
    socklen_t sinSize = sizeof client_addr;
    if( ( cfd = accept( this->socketFD, (struct sockaddr*) (&client_addr), &sinSize ) ) == -1 ) {
      this->logger->error( "Error al aceptar Cliente" );
      exit( -1 );
    }
    this->clientCount++;
    this->allowConnections = ( this->clientCount <= this->config->getMaxClients() );

    thread tCheckAliveSend( &Server::checkAliveSend, this, cfd);
    tCheckAliveSend.detach();

    thread process( &Server::receiveClientData, this, cfd, client_addr );
    process.detach();
  }
}

// get sockaddr, IPv4 
void* Server::getInAddr( struct sockaddr* sa ) {
  if( sa->sa_family == AF_INET ) {
    return &( ( (struct sockaddr_in*) sa )->sin_addr );
  }
}

void Server::receiveClientData( int cfd, struct sockaddr_storage client_addr ) {
  int numBytesRead;
  char clientIP[ INET_ADDRSTRLEN ]; // connected client IP
  Mensaje* msgToRecv = new Mensaje;

  // get connected host IP in presentation format
  inet_ntop( client_addr.ss_family,
      this->getInAddr( (struct sockaddr*) (&client_addr) ), clientIP,
      sizeof clientIP);

  if( this->allowConnections ) {
    cout << endl << notice( "Se inicio una conexion con el host: " ) << clientIP
      << endl;
    this->logger->info( "Se inicio una conexion con el host: " + string( clientIP ) );

    if( send( cfd, "Aceptado", 8, 0 ) == -1 ) {
      this->logger->error( "Error al enviar que se acepto la conexion" );
    }

    timeval timeout;
    timeout.tv_sec = this->MAX_UNREACHABLE_TIME;
    timeout.tv_usec = 0;
    bool receiving = true;
    mutex theMutex;
    while( receiving ) {
      // seteo el timeout de recepcion de mensajes
      if( setsockopt( cfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof( timeout ) ) < 0 ) {
	cout << "Error sockopt" << endl;
	exit( 1 );
      }

      if( ( numBytesRead = recv( cfd, msgToRecv, sizeof( Mensaje ), 0 ) ) == -1 ) {
	close( cfd );
	this->logger->warn( CONNECTION_TIMEOUT );
	DEBUG_WARN( CONNECTION_TIMEOUT );
      }

      if( numBytesRead > 0 ) {
	if( numBytesRead != 1 ) {
	  theMutex.lock();
	  cout << endl << "FD cliente: " << notice( to_string( cfd ) ) << endl;
	  cout << "ID del mensaje recibido: " << notice( msgToRecv->id ) << endl;
	  cout << "Tipo del mensaje recibido: " << notice( msgToRecv->tipo ) << endl;
	  cout << "Valor del mensaje recibido: " << notice( msgToRecv->valor ) << endl;

	  map<int,Mensaje*>* clientMsgFD = new map<int,Mensaje*>();
	  clientMsgFD->insert( pair<int,Mensaje*>( cfd, msgToRecv ) );
	  this->msgQueue->push( clientMsgFD );
	  theMutex.unlock();
	}
      } else {
	receiving = false;
	cout << endl << warning( "El cliente " ) << clientIP
	  << warning( " se desconecto" ) << endl;
	this->logger->warn( "El Cliente " + string( clientIP ) + " se desconecto" );
	this->closeClient( cfd );
      }
    }
  } else {
    cout << endl << warning( "El cliente " ) << clientIP << warning( " se rechazo" ) << endl;
    this->logger->warn( "El cliente " + string(clientIP) + " se rechazo" );
    usleep( 1000000 );
    this->closeClient( cfd );
  }
}

void Server::checkAliveSend( int cfd ) {
  char buf[1] = { '1' };

  while( true ) {
    if( !( this->connected ) ) return;
    // 4s timed send
    usleep(4000000);
    send( cfd, &buf, 1, 0 );
  }
}

void Server::processQueue() {
  bool msgIsValid;
  Mensaje* respuesta = new Mensaje;
  mutex theMutex;

  while( this->processing ) {
    if( !( this->msgQueue->empty() ) ) {
      theMutex.lock();
      //cout << "Saco Msj de la cola" << endl;
      map<int,Mensaje*>* data = this->msgQueue->front();
      this->msgQueue->pop();

      map<int,Mensaje*>::iterator it = data->begin();
      //cout << "FD cliente: " << it->first << " --  Mensaje: " << (it->second)->valor << endl;

      this->logger->info( "Msj de cliente: " + string( ( (it->second)->valor ) ) );

      msgIsValid = this->processMsg( string((it->second)->tipo), string(((it->second)->valor)) );
      if( msgIsValid ) {
	strcpy( respuesta->valor, "Mensaje Correcto" );
	this->logger->info( respuesta->valor );
      } else {
	strcpy( respuesta->valor, "Mensaje Incorrecto" );
	this->logger->warn( respuesta->valor );
      }
      thread tSending( &Server::sendData, this, it->first, respuesta , sizeof(Mensaje) );
      tSending.detach();

      delete data;

      theMutex.unlock();
    }
  }
  //cout<<"Corto processor"<<endl;
  delete respuesta;
}

bool Server::processMsg( string tipo, string valor ){
  const int MAX_INT = 2147483647;
  bool respuesta = false;
  regex r;
  const char* expr;

  if( tipo == K::typeInt ){
    //expr = "^-?(2?1?[0-4]?|2?0?[0-9]?|[0-1]?[0-9]?[0-9]?)([0-9]){1,7}$";//menor que +-2148000000
    expr = "^-?[0-9]+$";
    r = regex(expr);
    if((regex_match(valor, r)) && (atoi(valor.c_str()) >= -MAX_INT) && (atoi(valor.c_str()) <= MAX_INT)) //ese casteo de char* a int no se si se puede
      respuesta = true;

  } else {

    if( tipo == K::typeDouble ){
      expr = "^-?([0-9]+e-?[//+]?[0-9]{1,3}|[0-2][//.][0-9]{0,2}e-?[//+]?[0-9]{1,3}|[0-9]+[//.][0-9]+)$";
      r = regex(expr);
      if (regex_match(valor, r)) respuesta = true;

    } else {

      if( tipo == K::typeString ){
	expr = "^.+$";
	r = regex(expr);
	if( regex_match( valor, r ) ) respuesta = true;

      } else {

	if( tipo == K::typeChar ){
	  expr = "^.$";
	  r = regex(expr);
	  if( regex_match( valor, r ) ) respuesta = true;
	}
      }
    }
  }
  return respuesta;
}

void Server::sendData( int cfd, Mensaje* data, int dataLength ){
  if( send( cfd, data, dataLength, 0 ) == -1 ) {
    this->logger->warn( SEND_FAIL );
    DEBUG_WARN( SEND_FAIL );
  }
}

void Server::closeClient( int cfd ) {
  mutex theMutex;
  close( cfd );
  theMutex.lock();
  this->clientCount--;
  cout << " cantidad " << this->clientCount << endl;
  this->logger->info( "Cantidad de Clientes Conectados: " + to_string( this->clientCount ) );
  theMutex.unlock();
}

void Server::shutdown() {
  if( this->connected ) 
    this->closeConnection();
  
  logger->warn( SERVER_CLOSE );
  DEBUG_WARN( SERVER_CLOSE );
  exit( 0 );
}

void Server::closeConnection() {
  close( this->socketFD );
  this->listening = false;
  this->connected = false;
  this->processing = false;
  this->logger->warn( SERVER_DISCONNECT );
  DEBUG_WARN( SERVER_DISCONNECT );
}