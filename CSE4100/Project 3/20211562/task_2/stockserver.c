#include "csapp.h"
#include <pthread.h>

#define NTHREADS 10000                    // Number of worker threads
#define SBUFSIZE 16

struct tree_node {                      //bst node
    int id;                             //id
    int left_stock;                     //left stock on market  
    int price;                          //stock price
    int readcnt;                        //readcnt
    sem_t mutex;                        //mutex    
    struct tree_node *left, *right;     //left, right child node
};

typedef struct {                        
    int *buf;                           // buffer      
    int n;                              // buffer size    
    int front;                          // front index    
    int rear;                           // rear index
    sem_t mutex;                        // mutex for critical section
    sem_t slots;                        // empty slots
    sem_t items;                        // filled items
} sbuf_t;

struct tree_node *root = NULL;          //bst root node

static int byte_cnt = 0;                // byte count
static sem_t mutex;                     // mutex
sbuf_t sbuf;
static int active_clients = 0;       // 현재 연결된 클라이언트 수
static sem_t client_cnt_mutex;       // 동기화를 위한 뮤텍스

void sbuf_init(sbuf_t *sp, int n);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);
static void init_echo_cnt(void);
void echo_cnt(int connfd);
void *thread(void *vargp);

void insert_stock(int id, int left, int price);
void load_stock(const char *filename);
void save_stock_recursive(FILE *fp, struct tree_node *node);
void save_stock(const char *filename);
struct tree_node* find_stock(int id);
void inorder(struct tree_node *node, char *buffer, size_t bufsize);
void padded_write(int connfd, const char *msg);
void handle_command(int connfd, char *buf);
void sigint_handler(int sig);

void sbuf_init(sbuf_t *sp, int n) {     // Init bounded buffer
    sp->buf = Calloc(n, sizeof(int));   // Allocate buffer
    sp->n = n;                          // Set buffer size    
    sp->front = sp->rear = 0;           // Init index  
    Sem_init(&sp->mutex, 0, 1);         // Initialize mutex
    Sem_init(&sp->slots, 0, n);         
    Sem_init(&sp->items, 0, 0);         
}

void sbuf_insert(sbuf_t *sp, int item) {    // Insert item into bounded buffer
    P(&sp->slots);                          // Wait for empty slot
    P(&sp->mutex);                          // Enter critical section
    sp->buf[(++sp->rear) % sp->n] = item;   // Insert item
    V(&sp->mutex);                          // Exit critical section  
    V(&sp->items);                          // Signal that an item is available    
}

int sbuf_remove(sbuf_t *sp) {               // Remove item from bounded buffer
    int item;                               // Item to be removed   
    P(&sp->items);                          // Wait for available item   
    P(&sp->mutex);                          // Enter critical section 
    item = sp->buf[(++sp->front) % sp->n];  // Remove item
    V(&sp->mutex);                          // Exit critical section  
    V(&sp->slots);                          // Signal that a slot is available 
    return item;
}

static void init_echo_cnt(void) {           // Initialize static variables
    Sem_init(&mutex, 0, 1);                 // Initialize mutex
    byte_cnt = 0;                           // init byte count   
}

void echo_cnt(int connfd){                  // Echo server function 
    int n;                                  // Number of bytes read
    char buf[MAXLINE];                      // Buffer for reading lines
    rio_t rio;             

    static pthread_once_t once = PTHREAD_ONCE_INIT;     // Ensure init_echo_cnt is called only once

    Pthread_once(&once, init_echo_cnt);                 // Call init_echo_cnt once
    Rio_readinitb(&rio, connfd);                        // Initialize rio structure
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {  // Read lines from client
        P(&mutex);                                          // Enter critical section   
        byte_cnt += n;                                      // Update byte count  
        printf("server received %d bytes\n", n);            // echo 
        V(&mutex);                                          // Exit critical section      
        handle_command(connfd, buf);                        // Handle command from client
    }
}

void *thread(void *vargp){                  // Worker thread function
    Pthread_detach(pthread_self());         // Detach thread to avoid memory leak
    while(1){
        int connfd = sbuf_remove(&sbuf);    // Remove connection from bounded buffer
        
        P(&client_cnt_mutex);
        active_clients++;                   // Increment active client count
        V(&client_cnt_mutex);

        echo_cnt(connfd);                   // Process the connection
        Close(connfd);                      // Close connection 

                P(&client_cnt_mutex);
        active_clients--;

        
        if (active_clients == 0) {          // If no active clients, save stock data
            save_stock("stock.txt");
        }

        V(&client_cnt_mutex);
    }
   
}

void insert_stock(int id, int left, int price) {                // Insert stock into binary search tree
    struct tree_node *new = Malloc(sizeof(struct tree_node));   // Allocate memory for new node
    new->id = id;                                               // Set stock id 
    new->left_stock = left;                                     // Set left stock  
    new->price = price;                                         // Set price       
    new->readcnt = 0;                                           // Initialize read count    
    Sem_init(&new->mutex, 0, 1);                                // Initialize mutex for node  
    new->left = new->right = NULL;                              // Initialize left and right children to NULL

    if (!root) {                                                // empty tree
        root = new;                                             // Set root to new node
        return;
    }

    struct tree_node *cur = root, *parent = NULL;               // Current node and parent node
    while (cur) {                                               // Traverse the tree to find the correct position                            
        parent = cur;                                           // Update parent node  
        if (id < cur->id)                                       // If id is less than current node's id    
            cur = cur->left;                                    // Move to left child
        else if (id > cur->id)                                  // If id is greater than current node's id      
            cur = cur->right;                                   // Move to right child    
        else {
            free(new);                                          // If id already exists, free new node  
            return;
        }
    }

    if (id < parent->id)                                        // If id is less than parent's id      
        parent->left = new;                                     // Set new node as left child                 
    else
        parent->right = new;                                    // Set new node as right child  
}

void load_stock(const char *filename) {                         // Load stocks from file    
    FILE *fp = fopen(filename, "r");                            
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    int id, left, price;
    while (fscanf(fp, "%d %d %d", &id, &left, &price) != EOF) { // Read stock data from file
        insert_stock(id, left, price);                          // Insert stock into binary search tree    
    }

    fclose(fp);                                                 // Close file 
}

void save_stock_recursive(FILE *fp, struct tree_node *node) {           // Save stocks to file recursively
    if (!node) return;                                                  // If node is NULL, return
    save_stock_recursive(fp, node->left);                               // Recursively save left subtree    
    fprintf(fp, "%d %d %d\n", node->id, node->left_stock, node->price); // Write node data to file
    save_stock_recursive(fp, node->right);                              // Recursively save right subtree 
}

void save_stock(const char *filename) {             // Save stocks to file        
    FILE *fp = fopen(filename, "w");                    
    if (!fp) {                  
        perror("fopen");
        return;
    }
    save_stock_recursive(fp, root);                 // Save stocks recursively to file  
    fclose(fp);
}

struct tree_node* find_stock(int id) {              // Find stock by id in binary search tree
    struct tree_node *cur = root;                   // Start from root
    while (cur) {                                   // Traverse
        if (id == cur->id) return cur;              // If id matches, return current node
        else if (id < cur->id) cur = cur->left;     // If id is less, move to left child
        else cur = cur->right;                      // If id is greater, move to right child 
    }
    return NULL;
}

void inorder(struct tree_node *node, char *buffer, size_t bufsize) {                    // Inorder traversal to accumulate stock data into buffer
    char line[64];
    if (!node) return;                                                                  // If node is NULL, return
    inorder(node->left, buffer, bufsize);                                               // Recursively traverse left subtree
    snprintf(line, sizeof(line), "%d %d %d\n", node->id, node->left_stock, node->price);// Format node data into line
    strncat(buffer, line, bufsize - strlen(buffer) - 1);                                // Append line to buffer, ensuring null termination
    inorder(node->right, buffer, bufsize);                                              // Recursively traverse right subtree   
}


void padded_write(int connfd, const char *msg) {            // Write message to client with padding
    char buf[MAXLINE] = {0};                                // Init buffer                
    strncpy(buf, msg, MAXLINE - 1);                         // Copy message to buffer, ensuring null termination
    Rio_writen(connfd, buf, MAXLINE);                       // Write buffer to connection    
}

void handle_command(int connfd, char *buf) {                // Handle client commands: show, buy, sell
    char cmd[10];                                           // Command buffer                
    int id, amount;                                         // Stock id and amount           

    sscanf(buf, "%s", cmd);                                 // Parse command from buffer    

    if (!strcmp(cmd, "show")) {                             // If command is "show"     
        char bigbuf[MAXLINE] = {0};                         // Buffer to accumulate stock data    
        inorder(root, bigbuf, sizeof(bigbuf));              // Inorder traverse the tree and accumulate data into bigbuf
        padded_write(connfd, bigbuf);                       // Write accumulated data to client
    } 
    else if (!strcmp(cmd, "buy")) {           
        sscanf(buf, "%*s %d %d", &id, &amount);
        struct tree_node *node = find_stock(id);            // Find stock by id

        if (!node) {                                        // invalid id
            padded_write(connfd, "Invalid ID\n");
            return;
        }

        P(&node->mutex);                                        // Enter critical section for stock node                
        if (node->left_stock < amount) {                        // no left stocks
            padded_write(connfd, "Not enough left stock\n");
        } else {
            node->left_stock -= amount;                         // buy stock
            padded_write(connfd, "[buy] success\n");
        }
        V(&node->mutex);                                        // Exit critical section for stock node  
    } 
    else if (!strcmp(cmd, "sell")) {
        sscanf(buf, "%*s %d %d", &id, &amount);
        struct tree_node *node = find_stock(id);                // Find stock by id

        if (!node) {                                            // invalid id   
            padded_write(connfd, "Invalid ID\n");
            return;
        }

        P(&node->mutex);                                        // Enter critical section for stock node    
        node->left_stock += amount;                             // sell stock     
        padded_write(connfd, "[sell] success\n");   
        V(&node->mutex);                                        // Exit critical section for stock node     
    }
}

void sigint_handler(int sig) {                          // Signal handler for SIGINT    
    save_stock("stock.txt");                            // Save stocks to file 
    printf("Server shutting down. Stocks saved.\n");
    exit(0);
}

int main(int argc, char **argv) {
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    Signal(SIGINT, sigint_handler);                     // Register signal handler for SIGINT
    load_stock("stock.txt");                            // Load stocks from file
    Sem_init(&client_cnt_mutex, 0, 1);                  // Initialize mutex for client count    


    listenfd = Open_listenfd(argv[1]);                  // Open listening socket
    sbuf_init(&sbuf, SBUFSIZE);                         // Init bounded buffer    
    for(i = 0; i < NTHREADS; i++) 
        Pthread_create(&tid, NULL, thread, NULL);       // Create threads
    
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);                // Init client address length
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);   // Accept new connection
        sbuf_insert(&sbuf, connfd);                                 // Insert connection into bounded buffer
    }
}
