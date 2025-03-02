#pragma once

#include <SDL2/SDL.h>

#include "vartypes.h"

struct node_rect_struct {
  SDL_Rect rect;
  struct node_rect_struct* pointer;
};

typedef struct node_rect_struct node_rect;

node_rect* pushNodeToHead_rect(node_rect* head, SDL_Rect rect);

void popNodeFromTail_rect(node_rect* head, u16 len);

void printList_rect(node_rect* head);

node_rect getNode_rect(node_rect* listHead, u32 index);

// typedef struct {
//   Move move;
//   node_Move* pointer;
// } node_MOVE;

// void addNode_MOVE(node_MOVE* prevNode, Move move);

// Move getMoveFromNode(node_MOVE* listHead, u32 index);