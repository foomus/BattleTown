#include "LinkedList.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "vartypes.h"

// node_rect* createNode_rect(SDL_Rect rect) {
//   node_rect* newNode = malloc(sizeof(node_rect));
//   newNode->rect = rect;
//   newNode->pointer = NULL;
//   return newNode;
// }

node_rect* pushNodeToHead_rect(node_rect* head, SDL_Rect rect) {
  node_rect* newNode = malloc(sizeof(node_rect));
  newNode->rect = rect;
  newNode->pointer = head;
  // head = newNode;
}

void popNodeFromTail_rect(node_rect* head, u16 len) {
  node_rect* pretail = head;
  for (u16 i = 0; i < len - 2; i++) {
    pretail = pretail->pointer;
  }
  pretail->pointer = NULL;  // Cut tail
}

void printList_rect(node_rect* head) {
  node_rect* temp = head;
  while (temp != NULL) {
    printf("%d %d %d %d.... ", temp->rect.x, temp->rect.y, temp->rect.w, temp->rect.h);
    temp = temp->pointer;
  }
  printf("\n");
}

// node_rect getNode_rect(node_rect* listHead, u32 index) {
//   node_rect* node = listHead;
//   for (u32 i = 0; i < index; i++) {
//     node = node->pointer;
//   }
//   return node->rect;
// }

// void addNode_MOVE(node_MOVE* prevNode, Move move) {
//   node_MOVE node = {move, NULL};
//   prevNode->pointer = &node;
// }

// Move getMoveFromNode(node_MOVE* listHead, u32 index) {
//   node_MOVE* node = listHead;
//   for (u32 i = 0; i < index; i++) {
//     node = node->pointer;
//   }
//   return node->rect;
// }
