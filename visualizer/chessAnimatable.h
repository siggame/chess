#ifndef CHESS_ANIMATABLE_H
#define CHESS_ANIMATABLE_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{
  struct ChessBoard: public Animatable
  {
  };

  struct ChessPiece: public Animatable
  {
    float x;
    float y;
    int owner;
    int type;
    
  };

  struct ScoreTime: public Animatable
  {
    string playerName;
    int time;

  };

  struct Winner: public Animatable
  {
  };

	struct MovesHist: public Animatable
	{
		std::vector<std::string> moves_list;
	};
} // visualizer

#endif // CHESS_ANIMATABLE_H
