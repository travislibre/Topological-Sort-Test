/*
 AUTHOR: Travis Libre
 FILENAME: problem1.c
 SPECIFICATION: Reads in a graph and prints out the specified output
*/

#include <stdio.h>
#include <stdlib.h>
#define INFILE "graph.txt"
#define SIZE 26
/*
Precondition
--------------------------------
Can only hold variables up to Z
 */
typedef struct edge{
    int weight;
    char linkedVertex; // The destination vertex
    struct edge *nextEdge; // Next edge in linked list
} edge_t;

typedef struct vertex {
    int status;
    int inDegree;
    char vertex;   // Origin Vertex
    edge_t *linkList; // pointer to Linked List of vertex's edges
    struct vertex *nextVertex; // Next Vertex in vertex list
} vertex_t;
//-------------------------------------------------------------------------------
edge_t * create_edge (char linkedVertex, int weight);
vertex_t * create_vertex (char vertex);
void insert_edge (vertex_t **head, char linkedVertex, int weight);
void insert_vertex (vertex_t **head, char vertex);
vertex_t * findVertex(vertex_t* head, char vertex);
void printEdgeList(edge_t *edge);
void printVertexList(vertex_t *head, int vertices);
void tps(vertex_t *head);
edge_t * findEdge(vertex_t *head, char origin, char destination);
int main() {
    FILE *infile;
    vertex_t *head;
    char character;
    char secondChar;
    int weight;
    int verticesCount;
    int isDirected;
    infile=fopen(INFILE,"r");
    fscanf(infile, "%d", &verticesCount);
    fscanf(infile, "%d", &isDirected);
    if(fscanf(infile, " %c", &character)!=EOF){
        head = create_vertex(character);
    }
    while (fscanf(infile, " %c", &character) != EOF) // Builds adjacency list from text
    { // Outer While Loop
        if(character==(*head).vertex){ // Builds vertices from text if First Vertex is read twice
            do{ //Inner while Loop
                fscanf(infile, " %c", &secondChar);
                fscanf(infile, " %d", &weight); // Grabs weight
                vertex_t *primary = findVertex(head, character);
                // Primary/character represents the source vertex
                // secondChar is the destination vertex
                insert_edge(&primary, secondChar, weight);
                vertex_t *destination = findVertex(head, secondChar);
                destination->inDegree++;
                //printf(" %c ", character);
                //printf(" %c ", secondChar);
                //printf(" %d\n", weight);
                if(isDirected==0){
                    // secondChar becomes primary in order to insert edge backwards
                    insert_edge(&destination, character, weight);
                    // Insert operates again,
                    //but with secondChar replaced with character
                }
            }while(fscanf(infile, " %c", &character) != EOF);
            break;
        } // End Inner while Loop
        insert_vertex(&head, character);

        // End Outer While Loop
    }
    //printf("test\n");
    printVertexList(head, verticesCount);
    tps(head);
    return 0;
}
void tps(vertex_t *head){
    vertex_t *vertexQueue[SIZE];
    char origin[SIZE];
    origin[0] = ' ';
    int front = 0;
    int rear = 0;
    int first=0;
    vertex_t *temp = head;
    while(temp!=NULL){ // Finds all initial inDegrees of zero and queues them

        if(temp->inDegree==0){
            printf("\nTest: %c",temp->vertex);
            vertexQueue[rear] = temp;
            temp->inDegree--; // Prevents double input
            origin[rear]='-';
            rear++;

        }
        temp = temp->nextVertex;
    }
    int index = 0;
    while(index<rear){
        edge_t *edgeTemp = vertexQueue[index]->linkList;
        while(edgeTemp!=NULL) {
            vertex_t *vertexTemp = findVertex(head,edgeTemp->linkedVertex);
            vertexTemp->inDegree--;
            if(vertexTemp->inDegree==0){
                // IF QUEUEABLE
                vertexQueue[rear] = vertexTemp;
                vertexTemp->inDegree--;// Prevents double input
                origin[rear]=vertexQueue[index]->vertex;
                rear++;
            }
            else if(vertexTemp->inDegree<0){ // If the node has already been inserted
                int j=0;
                while (j<rear && vertexQueue[j]!=vertexTemp){
                    j++;
                }
            }
            edgeTemp=edgeTemp->nextEdge;
        }
        index++;
    }
    // Creating Critical Path
    int critPath[SIZE];
    char critCheck;
    rear--;
    int decrement = rear; // Decrement decrements downwards
    critPath[decrement] = 1;
    critCheck = origin[decrement]; // Last element is critical
    while(decrement>0){
        decrement--;
        if(critCheck==vertexQueue[decrement]->vertex){
            critPath[decrement]=1;
            critCheck=origin[decrement];
        }
        else{
            critPath[decrement]=0;
        }
    }                                                   //maxPathLog[i] = maxPath;
    //Creating Max_Path

    int maxPath=0;
    int maxPathLog[SIZE]; // Keeps track of changes in max path
    int i=0;
    while(i<=rear){
        if(critPath[i]==1){
            if(origin[i]!='-'){
                printf("\ntest: i = %d",i);
                vertex_t *tempVert = findVertex(head,origin[i]);
                printf("\n%c",tempVert->vertex);
                edge_t *edge = findEdge(head, origin[i], vertexQueue[i]->vertex);
                maxPath=maxPath+edge->weight;
                maxPathLog[i] = maxPath;
            }
            else{
                maxPathLog[i] = maxPath;
            }
        }
        else{
            if(origin[i]!='-') {
                edge_t *edge = findEdge(head, origin[i], vertexQueue[i]->vertex);
                maxPathLog[i] = maxPath+edge->weight;
            }
            else{
                maxPathLog[i] = maxPath;
            }
        }

        i++;
    }

    // TEST BELOWx
    i=0;
    printf("\nTopological Search Results:\nVertex   Origin   Max_Path   Critical_Path");
    while(i<=rear){

        printf("\n   %c",vertexQueue[i]->vertex);
        printf("       %c",origin[i]);
        printf("         %d", maxPathLog[i]);
        printf( "              %d", critPath[i]);
        i++;
    }
}
edge_t * findEdge(vertex_t *head, char origin, char destination){
    vertex_t *source = findVertex(head, origin);
    edge_t *findEdge = source->linkList;
    while(findEdge->linkedVertex!=destination && findEdge->nextEdge!=NULL){
        findEdge=findEdge->nextEdge;
    }
    return findEdge;
}
/* NAME: create_edge
 PARAMETERS: linkedVertex char, weight in, the two attributes of an edge
 PURPOSE: Creates an edge in heap, returns pointer to it
 PRECONDITION: Both values are valid, heap has space
 POSTCONDITION: Pointer points to edge_t struct
*/
edge_t * create_edge (char linkedVertex, int weight) {
    edge_t *newp = (edge_t *) malloc(sizeof(edge_t));
    newp->weight=weight;
    newp->linkedVertex = linkedVertex;
    newp->nextEdge=NULL;
    return (newp);
}
/* NAME: create_vertex
 PARAMETERS: vertex char, the two attribute of a vertex
 PURPOSE: Creates a vertex in heap, returns pointer to it
 PRECONDITION: value is valid, heap has space
 POSTCONDITION: Pointer points to vertex_t struct
*/
vertex_t * create_vertex (char vertex) {
    vertex_t *newp = (vertex_t *) malloc(sizeof(vertex_t));
    newp->vertex=vertex;
    newp->linkList=NULL;
    newp->nextVertex=NULL;
    newp->status=1;
    newp->inDegree=0;
    return (newp);
}
/* NAME: insert_edge
 PARAMETERS: linkedVertex char, weight in, the two attributes of an edge, vertex_t **head, origin vertex
 PURPOSE: Creates an edge and links a vertex to it
 PRECONDITION: All values are valid, heap has space
 POSTCONDITION: Pointer points to edge_t struct
*/
void insert_edge (vertex_t **head, char linkedVertex, int weight){
    edge_t *newp = create_edge(linkedVertex, weight);
    edge_t *cur;
    //vertex_t *receiver = findVertex(*head,linkedVertex);//!!!!!!!!!!!!!!!!!!! Vertex May not Exist
    //(*head)->inDegree = (*head)->inDegree + 1;
    if ( (*head)->linkList == NULL) {            // check for an empty list
        (*head)->linkList = newp;                 // set head to newp
    }
    else {
        cur = (*head)->linkList;

        while(cur->nextEdge != NULL){
            cur = cur->nextEdge;
        } // Once end of linkList is reached

        cur->nextEdge = newp;
    }
}
/* NAME: insert_vertex
 PARAMETERS: vertex_t **head, root vertex, char vertex, node character
 PURPOSE: Creates an vertex and links a vertex to it
 PRECONDITION: Both values are valid, heap has space
 POSTCONDITION: Pointer points to vertex_t struct
*/
void insert_vertex (vertex_t **head, char vertex){
    vertex_t *newp = create_vertex(vertex);
    vertex_t *cur;
    if ((*head)->nextVertex == NULL) {            // check for an empty list
        (*head)->nextVertex = newp;                 // set head to newp
    }
    else {
        cur = (*head)->nextVertex;
        while(cur->nextVertex != NULL){
            cur = cur->nextVertex;
        }
        cur->nextVertex = newp;
    }
}
/* NAME: findVertex
 PARAMETERS: vertex_t *head, root vertex, char vertex, node character to be found
 PURPOSE: Identifies a character within the graph
 PRECONDITION: Both values are valid
 POSTCONDITION: Returns a pointer to the vertex_t inside the graph
*/
vertex_t * findVertex(vertex_t* head, char vertex){
    while(head!=NULL){
        if(head->vertex!=vertex){
            head=head->nextVertex;
        }
        else{
            return head;
        }
    }
    return NULL;
}

/* NAME: printEdgeList
 PARAMETERS: edge_t *edge
 PURPOSE: Prints out an edge list, with formatting
 PRECONDITION: None
 POSTCONDITION: Prints list to console
*/
void printEdgeList(edge_t *edge){
    while(edge!= NULL){
        printf(" (%c,%i) ->", edge->linkedVertex, edge->weight);
        edge = edge->nextEdge;
    }
    printf(" (nil)\n");
}
/* NAME: printVertexList
 PARAMETERS: vertex_t *head, int vertices
 PURPOSE: Prints out a vertex list, with formatting and vertices count
 PRECONDITION: None
 POSTCONDITION: Prints to console
*/
void printVertexList(vertex_t *head, int vertices){
    printf("Adjacency List:\n   Number of Vertices:  %i\n", vertices);
    while(head!=NULL){
        printf(" %d  %c: ",head->inDegree,head->vertex);
        printEdgeList(head->linkList);
        head = head->nextVertex;
    }
}
/*
------------------------------------------REPORT-----------------------------------------------------------------------

i) Graph Analysis
    1. Big O of finding the indegree of each vertex
      O(E), where E is the number of edges
    2. Big O of the determining the critical path
      O(E), as it must permutate backwards through the edges.
      It cannot be any larger than E, as a worst case graph is effectively a linked list.
    3. Big O of the topological sort that tracks the maximum path lengths and critical path
      O(V+E), Must permutate over both every vertice and almost every edge, but only once
    4. Big O of the overall storage requirements for the topological sort that tracks the
maximum path lengths and critical path
      O(V+E), as our implementation must store the list of vertices and the edge nodes, to track path length.

*/