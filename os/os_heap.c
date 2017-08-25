#include"os_heap.h"
#include"os_thread.h"


#define HEAP_MAX_SIZE    1024
int heap[HEAP_MAX_SIZE];					//堆的最大容量
int heap_current_index;						//堆当前大小
int heap_locker;							//增加、删除锁


int os_heap_init()
{
	int i;
	for(i=0;i<HEAP_MAX_SIZE;i++)
	{
		heap[i] = 0;
	}

	heap_current_index = 0;
	os_mutex_init(&heap_locker);
}

void os_heap_insert(int value)
{
	os_mutex_lock(&heap_locker);
	heap[heap_current_index] = value;
	
	int swap_pos = heap_current_index;
	int next_pos;
	while(swap_pos > 0)
	{
		next_pos = swap_pos >> 1;
		if(heap[swap_pos] < heap[next_pos])
		{
			int temp = heap[next_pos];
			heap[next_pos] = heap[swap_pos];
			heap[swap_pos] = temp;
			swap_pos = next_pos;
		}
		else
		{
			
			break;
		}
	}
	heap_current_index++;
	os_mutex_unlock(&heap_locker);
	
}

void os_heap_extract(int index)
{

	os_mutex_lock(&heap_locker);
	heap[index] = heap[heap_current_index-1];
	heap_current_index--;
	
	int parent;
	parent = index;
	//printf("parent : %d val %d\n",parent,heap[index]);
	while(parent < heap_current_index)
	{

		
		int great = parent;
		int leftchild = (2 * great)+1;

		if(leftchild < heap_current_index && heap[leftchild] < heap[great])
		{
			great = leftchild;
		}
		int rightchild = (2 * parent) +2;
		if(rightchild < heap_current_index && heap[rightchild] < heap[great])
		{
			great = rightchild;
		}
		if(great == parent)
		{
			break;
		}

		int temp      = heap[great];
		heap[great]   = heap[parent];
		heap[parent]  = temp;
		parent = great;
	}
	os_mutex_unlock(&heap_locker);
}

void os_heap_debug()
{
	printf("min: %d\n",heap[0]);
	int i;
	for(i=0;i<heap_current_index;i++)
	{
		printf("| %d ",heap[i]);
	}
	printf("|\n");
}