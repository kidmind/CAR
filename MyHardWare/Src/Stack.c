#include "Stack.h"
#include "stdlib.h"

void Stack_Init( struct Stack *s , u8 MaxSize)
{
	s->Data = malloc( MaxSize );
	s->MaxSize = MaxSize;
	s->SP = -1;
}
u8 PUSH( struct Stack *s , Stack_ElemType PushData )
{
	if( s->SP >= s->MaxSize-1 )
	{
		return 0;
	}
	s->SP++;
	s->Data[s->SP] = PushData;
	return 1;
}

Stack_ElemType POP( struct Stack *s )
{
	if( s->SP == -1 )
	{
		return 0;
	}
	(s->SP)--;
	return s->Data[s->SP+1];
}

FlagStatus Stack_Empety( struct Stack *s )
{
	if( s->SP==-1 )
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
