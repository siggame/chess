//Copyright (C) 2009 - Missouri S&T ACM AI Team
//Please do not modify this file while building your AI
//See AI.h & AI.cpp for that
#ifndef VC_STRUCTURES_H
#define VC_STRUCTURES_H

namespace client
{

struct Connection;
struct _Piece;
struct _Move;
struct _Player;


struct _Piece
{
  Connection* _c;
  int id;
  int owner;
  int file;
  int rank;
  int hasMoved;
  int type;
};
struct _Move
{
  Connection* _c;
  int id;
  int fromFile;
  int fromRank;
  int toFile;
  int toRank;
  int promoteType;
};
struct _Player
{
  Connection* _c;
  int id;
  char* playerName;
  float time;
};

}

#endif
