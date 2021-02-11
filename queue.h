
#ifndef Q_NODE
#define Q_NODE
typedef struct node {
	struct node *next;
	Beep *beep;
} q_node;
#endif


void q_push(q_node **queue, Beep *beep);


Beep* q_peek(q_node *queue);


Beep* q_pop(q_node **queue);

int q_samples_left(q_node* queue);

int q_duration(q_node* queue);

void q_print(q_node *queue);

void q_free(q_node **queue);
