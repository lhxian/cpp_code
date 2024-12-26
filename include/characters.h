#ifndef _CHARACTERS_H_
#define _CHARACTERS_H_
#include<Windows.h>
enum Character{
  Npc,Log,Goe,
  Deer,
  character_cnt
};
namespace NNpc{
  constexpr INT32 
  w = 64,
  h = 128,
  block_w = 16,
  block_h = 32;
}
namespace NLog{
  constexpr INT32 
  w = 192,
  h = 128,
  block_w = 32,
  block_h = 32;
}
namespace NGoe{
  constexpr INT32 
  w = 192,
  h = 192,
  block_w = 48,
  block_h = 48;
}
// namespace NDeer{
//   constexpr INT32 
//   w = 
// }
#endif // _CHARACTERS_H_